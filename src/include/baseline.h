#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <gperftools/profiler.h>

#define THREAD_NUM 8
#define RESULT_LEN (N * (N + 1) / 2)
#define PAIR_REMINDER (RESULT_LEN % THREAD_NUM)
#define PAIR_PER_THREAD (RESULT_LEN / THREAD_NUM)

//#define SELF_CHECK

struct thread_args{
    int starts;         // Index of the first assigned task
    int size;           // How many task we assigned
    int M;              // Columns
    int N;              // Rows
    int64_t* matrix;    // Pointer to data
    int64_t* result;    // Pointer to the result array
};