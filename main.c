#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include "./RadixSortSequencial.c"
#include "./RdxSortOpenMP.c"
#define SIZE 100000000
double TIME_I, TIME_F;

void popular(long int* array, long int size) {
    srand(time(NULL));

    for (long int i = 0; i < size; i++) {
        array[i] = rand() % size + 1;
    }
}
int main() {
    long int* a = (long int*)malloc(SIZE * sizeof(long int));
    if (a == NULL) {
        printf("Falha na alocação de memória.\n");
        return 1;
    }
    long int* b = (long int*)malloc(SIZE * sizeof(long int));
    if (b == NULL) {
        printf("Falha na alocação de memória.\n");
        return 1;
    }
    
    popular(a, SIZE); // Preenche o array com números aleatórios
    popular(b, SIZE); // Preenche o array com números aleatórios

    TIME_I = omp_get_wtime();
    RadixSort_Sequencial(a, SIZE); // Ordena o array usando o algoritmo Radix Sort
    TIME_F = omp_get_wtime();
    printf("Sequencial %fs \n", TIME_F - TIME_I);
    TIME_I = omp_get_wtime();
    b = RadixSortOpenMPB(SIZE, b); // Ordena o array usando o algoritmo Radix Sort
    TIME_F = omp_get_wtime();
    printf("Paralelo %fs \n", TIME_F - TIME_I);
    free(a); // Libera a memória alocada para o array
    free(b); // Libera a memória alocada para o array

    return 0;
}