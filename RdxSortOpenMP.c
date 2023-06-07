#include "stdio.h"
#include "stdlib.h"
#include "omp.h"
long int getMaxB(long int* a, long int n) {
    long int max = a[0];
    for (long int i = 1; i < n; i++) {
        if (a[i] > max)
            max = a[i];
    }
    return max;
}


void RadixSortOpenMPB(long int n, long int* data) {
    long int max = getMaxB(data, n);
    long int * buffer = malloc(n*sizeof(long int));
    long int i;
    for (long int place = 1; max / place > 0; place *= 10) {
        long int bucket[10] = {0};

        long int local_bucket[10] = {0}; // size needed in each bucket/thread
        //1st pass, scan whole and check the count
#pragma omp parallel firstprivate(local_bucket)
        {
#pragma omp for schedule(static) nowait
            for(i = 0; i < n; i++){
                local_bucket[(data[i] / place) % 10]++;
            }
#pragma omp critical
            for(i = 0; i < 10; i++) {
                bucket[i] += local_bucket[i];
            }
#pragma omp barrier
#pragma omp single
            for (i = 1; i < 10; i++) {
                bucket[i] += bucket[i - 1];
            }
            int nthreads = omp_get_num_threads();
            int tid = omp_get_thread_num();
            for(int cur_t = nthreads - 1; cur_t >= 0; cur_t--) {
                if(cur_t == tid) {
                    for(i = 0; i < 10; i++) {
                        bucket[i] -= local_bucket[i];
                        local_bucket[i] = bucket[i];
                    }
                } else { //just do barrier
#pragma omp barrier
                }

            }
#pragma omp for schedule(static)
            for(i = 0; i < n; i++) { //note here the end condition
                //printf("i %ld :",i);
                //printf("n %ld / %ld =",data[i], place);
                //printf("n %ld ",(data[i] / place) % 10 );
                //printf("local bucket %ld\n",local_bucket[(data[i] / place) % 10]++);
                if(data[i] > 0){
                    buffer[local_bucket[(data[i] / place) % 10]++] = data[i];
                }

                
            }

        }
        //now move data
        long int* tmp = data;
        data = buffer;
        buffer = tmp;
    }
    free(buffer);
    for(long int z = n - 1; z > n - 101; z--){
        printf("%ld,", data[z]);
    }
    printf("\n");
}