#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <gperftools/profiler.h>
#include <mpi.h>

#define RESULT_LEN (N * (N + 1) / 2)
#define PAIR_REMINDER (RESULT_LEN % THREAD_NUM)
#define PAIR_PER_THREAD (RESULT_LEN / THREAD_NUM)

//#define PROFILING
#define SELF_CHECK
//#define DEBUG_ON

struct thread_args{
//    int starts;         // Index of the first assigned task
//    int size;           // How many task we assigned
    int64_t M;              // Columns
    int64_t N;              // Rows
    int64_t* self_matrix;    // Pointer to data
    int64_t* recv_matrix;    // Pointer to data
    int64_t* result;    // Pointer to the result array
    int64_t n_row_blk;      // How many rows in one block
    pthread_t * t;
};