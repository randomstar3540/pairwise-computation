#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <gperftools/profiler.h>

#define THREAD_NUM 8
#define RESULT_LEN (N * (N + 1) / 2)
#define PAIR_REMINDER (RESULT_LEN % THREAD_NUM)
#define PAIR_PER_THREAD (RESULT_LEN / THREAD_NUM)
#define BSIZE 32

#define PROFILING
//#define SELF_CHECK
//#define DEBUG_ON

struct thread_args{
//    int starts;         // Index of the first assigned task
//    int size;           // How many task we assigned
    int64_t M;              // Columns
    int64_t N;              // Rows
    int64_t* matrix;    // Pointer to data
    int64_t* result;    // Pointer to the result array
    int64_t n_pair;
    int64_t n_blk_row;
    int64_t n_blk_col;
    pthread_t * t;
    pthread_mutex_t * l;
};