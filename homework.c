#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <mpi.h>

//merge_sort from sample code

//#define INITIALIZE 1    // Message initializing data
//#define RECEIVING 2     // Message receivng (child node)
#define SENDING_UP 3    // Message sending (to child node)
#define SENDING_DOWN 4  // Message sending (to parent node)
//#define FINALIZING 5    // Ending the process

//#define DEBUG 1

#define INPUT_SUCCESS 1

int min(int x, int y) {
    return x < y ? x : y;
}

void merge_sort(int arr[], int len) {
    int* a = arr;
    int* b = (int*) malloc(len * sizeof *b);
    int seg, start;
    for (seg = 1; seg < len; seg += seg) {
        for (start = 0; start < len; start += seg + seg) {
            int low = start, mid = min(start + seg, len), high = min(start + seg + seg, len);
            int k = low;
            int start1 = low, end1 = mid;
            int start2 = mid, end2 = high;
            while (start1 < end1 && start2 < end2)
                b[k++] = a[start1] < a[start2] ? a[start1++] : a[start2++];
            while (start1 < end1)
                b[k++] = a[start1++];
            while (start2 < end2)
                b[k++] = a[start2++];
        }
        int* temp = a;
        a = b;
        b = temp;
    }
    if (a != arr) {
        int i;
        for (i = 0; i < len; i++)
            b[i] = a[i];
        b = a;
    }
//    free(b);
}

int get_input(const char* location, int** arr, int* len) {
    FILE* file_to_open = fopen(location, "r");
    int length = 0;
    int tmp = 0;
    while (!feof(file_to_open)) {
        fscanf(file_to_open, "%d", &tmp);
        length++;
    }
    length--;//fix the last "space" problem
#if DEBUG == 1
    printf("length: %d\n", length);
#endif
    rewind(file_to_open);
    int* data;
    data = calloc((size_t) length, sizeof(int));
    if (file_to_open == NULL) {
        printf("ERROR: FILE NOT FOUND.");
        exit(EXIT_FAILURE);
    }
    if (arr == NULL) {
        printf("ERROR: MEMORY ALLOCATION FAILURE.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < length; ++i) {
        if (!fscanf(file_to_open, "%d", &data[i]))
            break;
    }
    *arr = data;
    *len = length;
    fclose(file_to_open);
    return INPUT_SUCCESS;
}

void print_output(int* arr, int length) {
    int i = 0;
    FILE* file_to_write = fopen("output.txt", "w");
    for (i = 0; i < length; i++) {
        fprintf(file_to_write, "%d ", arr[i]);
    }
    fclose(file_to_write);
}

void ParallelMergeSort(int*, int, int);

int main(int argc, char** argv) {
    int comm_sz; /*Number of processes*/
    int my_rank; /*My process rank*/
    
    MPI_Status status;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    int* arr;
//    int* receive;
//    int* local_array;

//    arr = (int*) malloc((size_t) sizeof *arr);
//    receive = (int*) malloc((size_t) sizeof *receive);
    
    
    int length = 0;
    if (my_rank == 0) {
        get_input(argv[1], &arr, &length);

#if DEBUG == 1
        for (int i = 0; i < length; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
#endif
        
        int root_height = 0;
        int node_count = 1;
        
        while (node_count < comm_sz - 1) {
            node_count *= 2;
            root_height++;
        }
        
        ParallelMergeSort(arr, length, root_height);
        
    }
    else {
        int info[2];    //Message passing through nodes in an array
        
        /*info[0] node array size
        info[1] node height*/
        
        
        MPI_Recv(info, 2, MPI_INT, MPI_ANY_SOURCE, SENDING_DOWN, MPI_COMM_WORLD, &status);
#if DEBUG == 1
        printf("DEBUG: %d RECEIVE LEFT\n", my_rank);
#endif
        int array_length = info[0];
        int node_height = info[1];
        
        arr = calloc((size_t) array_length, sizeof *arr);
        
        MPI_Recv(arr, array_length, MPI_INT, MPI_ANY_SOURCE, SENDING_DOWN, MPI_COMM_WORLD, &status);

#ifdef DEBUG
        printf("%d Receiving...\n", my_rank);
#endif
        
        ParallelMergeSort(arr, array_length, node_height);
        
        
        MPI_Finalize();
        return 0;
        
    }

#ifdef DEBUG
    printf("%d Leaving...\n", my_rank);
#endif
    
//    MPI_Barrier(MPI_COMM_WORLD);
    
    
    if (my_rank == 0) {
        print_output(arr, length);
#if DEBUG == 1
        for (int i = 0; i < length; i++) {
            printf("%d", arr[i]);
        }
        printf("\n");
#endif
//        free(arr);
        MPI_Finalize();
    }
    
    return 0;
}

void ParallelMergeSort(int* array, int data_length, int height) {
    int parent = 0;
    int my_rank = 0;
    int comm_sz = 0;
    int next_height = 0;
    int right_child = 0;
    
    MPI_Status status;
    
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    // parent/right_child Calculation modified from http://penguin.ewu.edu/~trolfe/ParallelMerge/ParallelMerge.html
    
    parent = ((my_rank) & ~(1 << height));
    next_height = height - 1;
    
    if (next_height >= 0)
        right_child = ((my_rank) | (1 << next_height));

#if DEBUG == 1
    printf("my_rank: %d, height: %d, next_height: %d, right_child: %d\n", my_rank, height, next_height, right_child);
#endif
    
    if (height != 0) {
        int child_length = data_length / 2;
        
        int* left_half = calloc((size_t) child_length, sizeof *left_half);
        int* right_half = calloc((size_t) child_length, sizeof *right_half);
        
        
        int info[2];
        
        /*info[0] node array size
        info[1] node height*/
        
        memcpy(left_half, array, child_length * sizeof *left_half);
        memcpy(right_half, array + child_length, child_length * sizeof *right_half);
#if DEBUG == 1
        printf("Rank %d left array:\n", my_rank);
        for (int i = 0; i < child_length; i++) {
            printf("%d ", left_half[i]);
        }
        
        printf("\n");
        
        printf("Rank %d right array:\n", my_rank);
        for (int i = 0; i < child_length; i++) {
            printf("%d ", right_half[i]);
        }
        printf("\n");

#endif
        info[0] = child_length;
        info[1] = next_height;
        
        MPI_Send(info, 2, MPI_INT, right_child, SENDING_DOWN, MPI_COMM_WORLD);
        MPI_Send(right_half, child_length, MPI_INT, right_child, SENDING_DOWN, MPI_COMM_WORLD);

#if DEBUG == 1
        printf("DEBUG: %d SEND RIGHT\n", my_rank);
#endif
        // Sending data to the right child
        
        ParallelMergeSort(array, child_length, next_height);
        // Passing data down
//        free(right_half);
        
        
        MPI_Recv(array + child_length, child_length, MPI_INT, right_child, SENDING_UP, MPI_COMM_WORLD, &status);
        // Receiving from the right child
        
        
        merge_sort(array, data_length);
        //merge_sort in parent node
#if DEBUG == 1
        printf("Rank %d array (sorted):\n", my_rank);
        for (int i = 0; i < data_length; ++i) {
            printf("%d ", array[i]);
        }
        printf("\n");
#endif
        
    }
    else {
        merge_sort(array, data_length);
    }
    
    // if not parent node, send data to its left
    if (parent != my_rank) {
        MPI_Send(array, data_length, MPI_INT, parent, SENDING_UP, MPI_COMM_WORLD);
#if DEBUG == 1
        printf("DEBUG: %d SEND LEFT\n", my_rank);
#endif
    }
#if DEBUG == 1
    printf("DEBUG: %d ParallelMerge END\n", my_rank);
#endif
}