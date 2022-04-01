#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include "include/baseline.h"

pthread_mutex_t lock;

void print_matrix(int M, int N, int64_t* matrix[]){
    for(int i = 0; i < N; i ++) {
        for(int j = 0; j < M; j ++) {
            printf("%lld ", matrix[i][j]);
            if(j == M - 1){
                printf("\n");
            }
        }
    }
}

int64_t compute_dot_product(int64_t* A, int64_t* B, int length){
    int64_t result;
    for(int i = 0; i < length; i ++){
        // A·B = Sum(A[i] * B[i]) for i: 0 -> M-1
        result += A[i] * B[i];
    }
    return result;
}


int main(){

    // We first scan M, N, which is the length of the
    // columns and rows for this array.
    int M, N;
    scanf("%d",&M);
    scanf("%d",&N);
    printf("We got M: %d, N: %d\n", M, N);

    // Declare 2D array and the result array
    int64_t matrix[N][M];
    int64_t result[N * (N + 1) / 2];

    //Scan the matrix
    for(int i = 0; i < N; i ++){
        for(int j = 0; j < M; j ++){
            scanf("%lld ",&matrix[i][j]);
        }
    }

    //Init mutex lock
    pthread_mutex_init(&lock, NULL);

    return 0;
}
