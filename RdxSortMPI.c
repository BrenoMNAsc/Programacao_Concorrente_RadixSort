#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define SIZE 70000000

void popular(long int* array, long int size) {
    srand(time(NULL));

    for (long int i = 0; i < size; i++) {
        array[i] = rand() % size + 1;
    }
}

int getMSD(long int number, int digit) {
    while (digit > 0) {
        number /= 10;
        digit--;
    }
    return number % 10;
}

void localSort(long int* numbers, int size) {
    long int* output = (long int*) malloc(size * sizeof(long int));
    long int max = numbers[0];
    for (int i = 1; i < size; i++) {
        if (numbers[i] > max)
            max = numbers[i];
    }
    int* count = (int*) calloc((max + 1), sizeof(int));

    for (int i = 0; i < size; i++) {
        count[numbers[i]]++;
    }

    for (int i = 1; i <= max; i++) {
        count[i] += count[i - 1];
    }

    for (int i = size - 1; i >= 0; i--) {
        output[count[numbers[i]] - 1] = numbers[i];
        count[numbers[i]]--;
    }

    for (int i = 0; i < size; i++) {
        numbers[i] = output[i];
    }

    free(output);
    free(count);
}

int main(int argc, char** argv) {
    int P, rank, N;
    long int* localNumbers;
    long int* sortedNumbers;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        N = SIZE;
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int numbersPerProcess = N / P;
    int remainder = N % P;

    localNumbers = (long int*) malloc(numbersPerProcess * sizeof(long int));
    sortedNumbers = (long int*) malloc(N * sizeof(long int));

    long int* numbers = NULL;

    if (rank == 0) {
        numbers = malloc(sizeof(long int) * N);
        popular(numbers, N);
    }

    MPI_Scatter(numbers, numbersPerProcess, MPI_LONG, localNumbers, numbersPerProcess, MPI_LONG, 0, MPI_COMM_WORLD);

    localSort(localNumbers, numbersPerProcess);

    MPI_Allgather(localNumbers, numbersPerProcess, MPI_LONG, sortedNumbers, numbersPerProcess, MPI_LONG, MPI_COMM_WORLD);

    if (rank == 0) {
        localSort(sortedNumbers, N);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Sorted numbers: ");
        for (int i = N - 6; i < N; i++) {
            printf("%ld ", sortedNumbers[i]);
        }
        printf("\n");

        free(numbers);
    }

    free(localNumbers);
    free(sortedNumbers);

    MPI_Finalize();

    return 0;
}
