#include <stdio.h>
#include <stdlib.h>
long int getMax(long int* a, long int n) {
    long int max = a[0];
    for (long int i = 1; i < n; i++) {
        if (a[i] > max)
            max = a[i];
    }
    return max;
}

void countingSort(long int* a, long int n, long int place)
{
    long int* output = (long int*)malloc(n * sizeof(long int));
    long int count[10] = {0};

    // Contagem da ocorrência de cada dígito
    for (long int i = 0; i < n; i++)
        count[(a[i] / place) % 10]++;

    // Cálculo da frequência acumulada
    for (long int i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // Ordenação dos elementos com base no lugar atual
    for (long int i = n - 1; i >= 0; i--) {
        output[count[(a[i] / place) % 10] - 1] = a[i];
        count[(a[i] / place) % 10]--;
    }

    // Copia os elementos ordenados de volta para o array original
    for (long int i = 0; i < n; i++)
        a[i] = output[i];

    free(output);
}

void RadixSort_Sequencial(long int* a, long int n) {
    long int max = getMax(a, n);

    // Realiza a ordenação para cada lugar (unidades, dezenas, centenas, etc.)
    for (long int place = 1; max / place > 0; place *= 10)
        countingSort(a, n, place);
}