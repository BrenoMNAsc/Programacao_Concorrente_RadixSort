#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#define SIZE 100000000

void popular(long int* array, long int size) {
    srand(time(NULL));

    for (long int i = 0; i < size; i++) {
        array[i] = rand() % size + 1;
    }
}

// Função para obter o dígito mais significativo de um número
int getMSD(long int number, int digit) {
    while (digit > 0) {
        number /= 10;
        digit--;
    }
    return number % 10;
}

// Função para realizar a etapa de classificação local
void localSort(long int* numbers, int size) {
    long int* output = (long int*) malloc(size * sizeof(long int));
    // Find the largest element of the array
    long int max = numbers[0];
    for (int i = 1; i < size; i++) {
        if (numbers[i] > max)
            max = numbers[i];
    }
    int* count = (int*) malloc((max + 1) * sizeof(int));

    // Initialize count array with all zeros.
    for (long int i = 0; i <= max; ++i) {
        count[i] = 0;
    }

    // Store the count of each element
    for (int i = 0; i < size; i++) {
        count[numbers[i]]++;
    }

    // Store the cumulative count of each array
    for (long int i = 1; i <= max; i++) {
        count[i] += count[i - 1];
    }

    // Find the index of each element of the original array in count array, and
    // place the elements in output array
    for (int i = size - 1; i >= 0; i--) {
        output[count[numbers[i]] - 1] = numbers[i];
        count[numbers[i]]--;
    }

    // Copy the sorted elements into original array
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
    int* recvCounts;
    int* displacements;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        long int* numbers = malloc(sizeof(long int) * SIZE);
        popular(numbers, SIZE);
        N = SIZE;
        // Distribuir os números entre os processos
        int numbersPerProcess = SIZE / P;
        int remainder = N % P;
        int displacement = 0;
        for (int i = 0; i < P; i++) {
            int count = numbersPerProcess + (i < remainder ? 1 : 0);
            MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(numbers + displacement, count, MPI_LONG, i, 0, MPI_COMM_WORLD);
            displacement += count;
        }
        free(numbers);
    }

    // Receber o número de elementos atribuídos a cada processo
    int localCount;
    MPI_Recv(&localCount, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Alocar espaço para armazenar os números locais
    localNumbers = (long int*) malloc(localCount * sizeof(long int));

    // Receber os números atribuídos a este processo
    MPI_Recv(localNumbers, localCount, MPI_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Etapa de classificação local
    localSort(localNumbers, localCount);

    // Determinar o número de elementos classificados em cada processo
    recvCounts = (int*) malloc(P * sizeof(int));
    MPI_Allgather(&localCount, 1, MPI_INT, recvCounts, 1, MPI_INT, MPI_COMM_WORLD);

    // Determinar os deslocamentos para os elementos classificados em cada processo
    displacements = (int*) malloc(P * sizeof(int));
    displacements[0] = 0;
    for (int i = 1; i < P; i++) {
        displacements[i] = displacements[i - 1] + recvCounts[i - 1];
    }

    // Alocar espaço para armazenar os números classificados globalmente
    sortedNumbers = (long int*) malloc(N * sizeof(long int));

    // Reunir todos os números classificados em um único processo
    MPI_Gatherv(localNumbers, localCount, MPI_LONG, sortedNumbers, recvCounts, displacements, MPI_LONG, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Imprimir os números classificados
        printf("Sorted numbers: ");
        for (int i = SIZE - 5; i < SIZE; i++) {
            printf("%ld ", sortedNumbers[i]);
        }
        printf("\n");
    }

    // Liberar memória
    free(localNumbers);
    free(sortedNumbers);
    free(recvCounts);
    free(displacements);

    MPI_Finalize();

    return 0;
}
