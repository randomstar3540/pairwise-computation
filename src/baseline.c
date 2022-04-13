#include "include/baseline.h"


// Print out a matrix to stdout
void print_matrix(int M, int N, int64_t *matrix){
    for(int i = 0; i < N; i ++) {
        for(int j = 0; j < M; j ++) {
            printf("%lld ", matrix[i*M + j]);
            if(j == M - 1){
                printf("\n");
            }
        }
    }
}

// Scan a matrix contains N rows and M columns from stdin
void scan_matrix(int64_t M, int64_t N, int64_t * matrix) {
    for(int i = 0; i < N; i ++){
        for(int j = 0; j < M; j ++){
            scanf("%lld ",&matrix[i * M + j]);
        }
    }
}

// Print out an array to stdin
void print_arr(int64_t* arr, int length){
    for (int i = 0; i < length; i ++) {
        printf("%lld    ", arr[i]);
    }
    printf("\n\n");
}

int64_t compute_dot_product(const int64_t *A, const int64_t *B, int length) {
    int64_t result = 0;
    for (int i = 0; i < length; i++) {
        // A·B = Sum(A[i] * B[i]) for i: 0 -> M-1
        result += A[i] * B[i];
    }
    return result;
}

void locate(int target, int N, int *const result) {
    // Compute the index of the two row we are going to compute
    // From the index of the result
    int first = 0;
    while (target >= N) {
        target -= N;
        N--;
        first++;
    }
    result[0] = first;
    result[1] = target + first;
}

int pairwise_computation(void *args) {
    // Thread function, compute the works assigned
    struct thread_args *p = (struct thread_args *) args;
    int starts = p->starts;
    int size = p->size;
    int M = p->M;
    int N = p->N;
    int64_t *matrix = p->matrix;
    int64_t *result = p->result;

    int idx[2];
    for (int i = starts; i < starts + size; i++) {
        locate(i, N, idx); // Locate the rows we are going to use

        // Compute and store
        result[i] = compute_dot_product(matrix + M * idx[0], matrix + M * idx[1], M);
#ifdef DEBUG_ON
        printf("result[%d] : %lld\n\n", i , result[i]);
#endif
    }
    return 0;
}

int compute_single_row(int64_t M, int64_t N, int64_t * result){
    int64_t matrix[M];
    for(int i = 0; i < M; i ++){
        scanf("%lld ",&matrix[i]);
    }
    int64_t res;
    for (int i = 0; i < M; i ++) {
        result[0] += matrix[i] * matrix[i];
    }
    return 0;
}

int compute_single_coulmn(int64_t M, int64_t N, int64_t * result){
    int64_t matrix[N];
    int idx = 0;
    for(int i = 0; i < N; i ++){
        scanf("%lld ",&matrix[i]);
    }
    for (int i = 0; i < N; ++i) {
        for (int j = i; j < N; ++j) {
            result[idx] = matrix[i] * matrix[j];
            idx ++;
        }
    }
    return 0;
}

int main() {

    // We first scan M, N, which is the length of the
    // columns and rows for this array.
    int M, N;
    ProfilerStart("baseline.prof");

    while(!feof(stdin)) {
        scanf("%d",&M);
        scanf("%d",&N);

#ifdef DEBUG_ON
        printf("We got M: %d, N: %d\n", M, N);
#endif
        // Declare 2D array and the result array
        int64_t * result = calloc(RESULT_LEN, sizeof(int64_t));
        pthread_t *t = calloc(THREAD_NUM, sizeof(pthread_t));

        // Scan the matrix
        if (N <= 0 || M <= 0) {
            printf("Error: M and N should be greater than 0\n");
        }else if (N == 1 && M == 1){
            // Only one number in the matrix
            scanf("%lld ",result);
        } else if (N == 1){
            // Only one Row in the matrix
            compute_single_row(M, N, result);
        } else if (M == 1){
            // Only one Column in the matrix
            compute_single_coulmn(M, N, result);
        } else {
            // Normal
            int64_t (*matrix) = calloc(M * N, sizeof(int64_t));

#ifdef DEBUG_ON
            print_matrix(M, N, matrix);
        printf("\n");
#endif
            struct thread_args args[THREAD_NUM];
            int counter = 0;
            scan_matrix(M, N, matrix);

            if (PAIR_PER_THREAD == 0) {
                for (int i = 0; i < RESULT_LEN; i++) {
                    args[i].starts = i;
                    args[i].size = PAIR_PER_THREAD;
                    args[i].M = M;
                    args[i].N = N;
                    args[i].matrix = &matrix[0];
                    args[i].result = result;

                    pthread_create(&t[i], NULL, (void *) pairwise_computation, args);
                }

                for (int i = 0; i < RESULT_LEN; i++) {
                    pthread_join(t[i], NULL);
                }
            } else {
                int tid = 0;
                for (int i = 0; i < RESULT_LEN - PAIR_REMINDER; i += (RESULT_LEN / (THREAD_NUM))) {
                    args[tid].starts = i;
                    args[tid].size = (RESULT_LEN / (THREAD_NUM));
                    args[tid].M = M;
                    args[tid].N = N;
                    args[tid].matrix = &matrix[0];
                    args[tid].result = result;

                    pthread_create(&t[tid], NULL, (void *) pairwise_computation, &args[tid]);
                    tid++;
                }
                for (int i = 0; i < THREAD_NUM; i++) {
                    pthread_join(t[i], NULL);
                }

#ifdef SELF_CHECK
                int64_t * sequential = calloc(RESULT_LEN, sizeof(int64_t));
                int64_t idx = 0;
                for (int i = 0; i < N; ++i) {
                    for (int j = i; j < N; ++j) {
                        sequential[idx] = compute_dot_product(&matrix[i * M], &matrix[j * M], M);
                        idx ++;
                    }
                }

                for(int i = 0; i < RESULT_LEN; i ++){
                    if(sequential[i] != result[i]){
                        printf("[Error]Result incorrect on index: %d. Expected: %lld, Get: %lld.\n", i, sequential[i], result[i]);
                    }
#ifdef DEBUG_ON
                    else {
                        printf("[INFO]Except %lld got %lld.\n", sequential[i], result[i]);
                    }
#endif
                }
                free(sequential);
#endif
//            } else {
//                int tid = 0;
//                for (int i = 0; i < RESULT_LEN; i += (RESULT_LEN / (THREAD_NUM - 1))) {
//                    args[tid].starts = i;
//                    args[tid].size = (RESULT_LEN / (THREAD_NUM - 1));
//                    args[tid].M = M;
//                    args[tid].N = N;
//                    args[tid].matrix = &matrix[0];
//                    args[tid].result = result;
//
//                    pthread_create(&t[tid], NULL, (void *) pairwise_computation, &args[tid]);
//                    tid++;
//                }
//
//                if (PAIR_REMINDER != 0) {
//                    args[tid].starts = RESULT_LEN - PAIR_REMINDER;
//                    args[tid].size = PAIR_REMINDER;
//                    args[tid].M = M;
//                    args[tid].N = N;
//                    args[tid].matrix = &matrix[0];
//                    args[tid].result = result;
//
//                    pthread_create(&t[tid], NULL, (void *) pairwise_computation, &args[tid]);
//                }
//
//                for (int i = 0; i < THREAD_NUM; i++) {
//                    pthread_join(t[i], NULL);
//                }

#ifdef SELF_CHECK
                int64_t * sequential = calloc(RESULT_LEN, sizeof(int64_t));
                int64_t idx = 0;
                for (int i = 0; i < N; ++i) {
                    for (int j = i; j < N; ++j) {
                        sequential[idx] = compute_dot_product(&matrix[i * M], &matrix[j * M], M);
                        idx ++;
                    }
                }

                for(int i = 0; i < RESULT_LEN; i ++){
                    if(sequential[i] != result[i]){
                        printf("[Error]Result incorrect on index: %d. Expected: %lld, Get: %lld.\n", i, sequential[i], result[i]);
                    }
#ifdef DEBUG_ON
                    else {
                        printf("[INFO]Except %lld got %lld.\n", sequential[i], result[i]);
                    }
#endif
                }
                free(sequential);
#endif
                free(matrix);
            }
        }
        // handle result
        free(result);

    }
    ProfilerStop();
//    print_result(result, RESULT_LEN);
    return 0;
}
