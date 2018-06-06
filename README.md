# ParallelProgramming_Homework_MPI_MergeSort

## Things need to know

This project uses 2^n processes. The size of input array must be (comm_sz*n).

The data in the input file should end with space(" ") and in LF format. (not a must)

>   Way to test:
```
mpicc -g -Wall -o homework homework.c
mpiexec -n 16 ./homework sample_input.txt
```

Auto test tool included. (./random_test.py ./seq.c)

seq.c needs to be compiled first.

```
gcc -o seq seq.c
```
