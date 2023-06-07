#include <stdio.h>

// Function to perform counting sort based on the digit represented by exp
void countingSortB(long int* arr, long int n, int exp) {
    long int* output = (long int*)malloc(n * sizeof(long int));
    int count[2] = {0};

    // Counting occurrences of 0s and 1s in the current digit
    for (long int i = 0; i < n; i++)
        count[(arr[i] >> exp) & 1]++;

    // Calculating cumulative count array
    count[1] += count[0];

    // Building the output array
    for (long int i = n - 1; i >= 0; i--) {
        output[count[(arr[i] >> exp) & 1] - 1] = arr[i];
        count[(arr[i] >> exp) & 1]--;
    }

    // Copying the output array to the original array
    for (long int i = 0; i < n; i++)
        arr[i] = output[i];
}

// Function to perform binary radix sort
void binaryRadixSort(long int* arr, int n) {
    // Finding the maximum element to determine the number of digits
    long int max = arr[0];
    for (long int i = 1; i < n; i++) {
        if (arr[i] > max)
            max = arr[i];
    }

    // Sorting the array using counting sort for each digit
    for (int exp = 0; (max >> exp) > 0; exp++)
        countingSortB(arr, n, exp);
}


