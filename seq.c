//
// Created by kimisecond on 5/30/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>


#define INPUT_FAILURE 1
#define INPUT_SUCCESS 0


int min(int x, int y) {
    return x < y ? x : y;
}

void merge_sort(int arr[], int len) {
    int* a = arr;
    int* b = (int*) malloc(len * sizeof(int));
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
    free(b);
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
    printf("length: %d\n",length);
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
    FILE* file_to_write = fopen("output1.txt", "wb");
    for (i = 0; i < length; i++) {
        fprintf(file_to_write, "%d ", arr[i]);
    }
    fclose(file_to_write);
}


int main(int argc, char* argv[]) {
    int* arr;
//    arr = (int*) malloc(sizeof(int));
    int length = 0;
    int i = 0;
    get_input(argv[1], &arr, &length);
//    printf("%s\n", argv[1]);
//    printf("%d\n", length);
//    for (i = 0; i < length; i++) {
//        printf("%d ", arr[i]);
//    }
    merge_sort(arr, length);
    for (i = 0; i < length; i++) {
        printf("%d ", arr[i]);
    }
    print_output(arr, length);
    return 0;
}