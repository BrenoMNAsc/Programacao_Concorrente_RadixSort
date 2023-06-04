#include "stdio.h"
#include "stdlib.h"
#include "omp.h"

#define BASE_BITS 8
#define BASE (1 << BASE_BITS)
#define MASK (BASE-1)
#define DIGITS(v, shift) (((v) >> shift) & MASK)

void RadixSortOpenMP(size_t n, unsigned data[n]) {
    unsigned * buffer = malloc(n*sizeof(unsigned));
    int total_digits = sizeof(unsigned)*8;

    //Each thread use local_bucket to move data
    size_t i;
    for(int shift = 0; shift < total_digits; shift+=BASE_BITS) {
        size_t bucket[BASE] = {0};

        size_t local_bucket[BASE] = {0}; // size needed in each bucket/thread
        //1st pass, scan whole and check the count
#pragma omp parallel firstprivate(local_bucket)
        {
#pragma omp for schedule(static) nowait
            for(i = 0; i < n; i++){
                local_bucket[DIGITS(data[i], shift)]++;
            }
#pragma omp critical
            for(i = 0; i < BASE; i++) {
                bucket[i] += local_bucket[i];
            }
#pragma omp barrier
#pragma omp single
            for (i = 1; i < BASE; i++) {
                bucket[i] += bucket[i - 1];
            }
            int nthreads = omp_get_num_threads();
            int tid = omp_get_thread_num();
            for(int cur_t = nthreads - 1; cur_t >= 0; cur_t--) {
                if(cur_t == tid) {
                    for(i = 0; i < BASE; i++) {
                        bucket[i] -= local_bucket[i];
                        local_bucket[i] = bucket[i];
                    }
                } else { //just do barrier
#pragma omp barrier
                }

            }
#pragma omp for schedule(static)
            for(i = 0; i < n; i++) { //note here the end condition
                buffer[local_bucket[DIGITS(data[i], shift)]++] = data[i];
            }
        }
        //now move data
        unsigned* tmp = data;
        data = buffer;
        buffer = tmp;
    }
    free(buffer);
}