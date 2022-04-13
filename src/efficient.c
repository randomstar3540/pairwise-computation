#include "include/efficient.h"

struct thread_args t_args;

// Print out a matrix to stdout
void print_matrix(int M, int N, int64_t *matrix) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            printf("%lld ", matrix[i * M + j]);
            if (j == M - 1) {
                printf("\n");
            }
        }
    }
}

// Scan a matrix contains N rows and M columns from stdin
void scan_matrix(int64_t M, int64_t N, int64_t *matrix) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            scanf("%lld ", &matrix[i * M + j]);
        }
    }
}

// Print out an array to stdin
void print_arr(int64_t *arr, int length) {
    for (int i = 0; i < length; i++) {
        printf("%lld    ", arr[i]);
    }
    printf("\n\n");
}

// Compute dot product of two arrays with size given.
// Default size is equal to BSIZE
int64_t compute_dot_product(const int64_t *A, const int64_t *B, const int64_t size) {
    int64_t result = 0;
    if (size >= 16){
        for (int i = 0; i < size; i+=16) {
            // A·B = Sum(A[i] * B[i]) for i: 0 -> M-1
            result += A[i] * B[i];
            result += A[i + 1] * B[i + 1];
            result += A[i + 2] * B[i + 2];
            result += A[i + 3] * B[i + 3];
            result += A[i + 4] * B[i + 4];
            result += A[i + 5] * B[i + 5];
            result += A[i + 6] * B[i + 6];
            result += A[i + 7] * B[i + 7];
            result += A[i + 8] * B[i + 8];
            result += A[i + 9] * B[i + 9];
            result += A[i + 10] * B[i + 10];
            result += A[i + 11] * B[i + 11];
            result += A[i + 12] * B[i + 12];
            result += A[i + 13] * B[i + 13];
            result += A[i + 14] * B[i + 14];
            result += A[i + 15] * B[i + 15];
        }
        return result;
    }else if (size >= 8){
        for (int i = 0; i < size; i+=8) {
            // A·B = Sum(A[i] * B[i]) for i: 0 -> M-1
            result += A[i] * B[i];
            result += A[i + 1] * B[i + 1];
            result += A[i + 2] * B[i + 2];
            result += A[i + 3] * B[i + 3];
            result += A[i + 4] * B[i + 4];
            result += A[i + 5] * B[i + 5];
            result += A[i + 6] * B[i + 6];
            result += A[i + 7] * B[i + 7];
        }
        return result;
    }else if (size >= 4){
        for (int i = 0; i < size; i+=4) {
            // A·B = Sum(A[i] * B[i]) for i: 0 -> M-1
            result += A[i] * B[i];
            result += A[i + 1] * B[i + 1];
            result += A[i + 2] * B[i + 2];
            result += A[i + 3] * B[i + 3];
        }
        return result;
    }else{
        for (int i = 0; i < size; i++) {
            // A·B = Sum(A[i] * B[i]) for i: 0 -> M-1
            result += A[i] * B[i];
        }
        return result;
    }
}

int64_t *result_address_from_block(int64_t *result, int64_t idx) {
    int64_t *p;
    int64_t row = idx / t_args.n_blk_row * t_args.n_pair;
    if (row == 0) {
        return result;
    } else {
        return result + (t_args.N + t_args.N - row + 1) * (row) / 2;
    }
}

void *block_computation_multithread(void *arg) {
    int64_t *args = arg;
    int64_t blk_A = args[0];
    int64_t blk_B = args[1];

    int64_t *result = result_address_from_block(t_args.result, blk_A);

    // Which rows are these blocks in
    int64_t row_num_A = blk_A / t_args.n_blk_row * t_args.n_pair;
    int64_t row_num_B = blk_B / t_args.n_blk_row * t_args.n_pair;

    // Start address of the blocks
    int64_t *A_start = t_args.matrix + row_num_A * t_args.M + blk_A % t_args.n_blk_row * BSIZE;
    int64_t *B_start = t_args.matrix + row_num_B * t_args.M + blk_B % t_args.n_blk_row * BSIZE;

    // The number of rows we are going to compute in this block
    int64_t row_num = THREAD_NUM;
    if (blk_A / t_args.n_blk_row == t_args.n_blk_col - 1 && t_args.N % THREAD_NUM != 0) {
        row_num = t_args.N % THREAD_NUM;
    }

    // The number of pairs we are going to compute per row
    int64_t pair_num = THREAD_NUM;
    if (blk_B / t_args.n_blk_row == t_args.n_blk_col - 1 && t_args.N % THREAD_NUM != 0) {
        pair_num = t_args.N % THREAD_NUM;
    }

    // The length of the arrays we are going to compute
    int64_t length = BSIZE;
    if (blk_A % t_args.n_blk_row == t_args.n_blk_row - 1 && t_args.M % BSIZE != 0) {
        length = t_args.M % BSIZE;
    }

    int64_t res;
    for (int i = 0; i < row_num; i++) {
#ifdef DEBUG_ON
        printf("result start from offset %llu\n", ((uint64_t)result - (uint64_t)t_args.result) / sizeof (uint64_t));
#endif
        for (int j = 0; j < pair_num; j++) {
            if (row_num_A + i > row_num_B + j) {
                continue;
            }
#ifdef DEBUG_ON
            printf("[DEBUG]DotComputing: %lld with row: %lld\n", row_num_A + i, row_num_B + j);
            printf("[DEBUG]Write to result %lld\n", row_num_B + j -  row_num_A - i);
#endif
            res = compute_dot_product(A_start + t_args.M * i, B_start + t_args.M * j, length);
            result[row_num_B + j - row_num_A - i] += res;
        }
        result += t_args.N - (row_num_A + i);
    }
    return NULL;
}

int compute_single_row(int64_t M, int64_t N, int64_t *result) {
    int64_t matrix[M];
    for (int i = 0; i < M; i++) {
        scanf("%lld ", &matrix[i]);
    }
    int64_t res;
    for (int i = 0; i < M; i++) {
        result[0] += matrix[i] * matrix[i];
    }
    return 0;
}

int compute_single_coulmn(int64_t M, int64_t N, int64_t *result) {
    int64_t matrix[N];
    int idx = 0;
    for (int i = 0; i < N; i++) {
        scanf("%lld ", &matrix[i]);
    }
    for (int i = 0; i < N; ++i) {
        for (int j = i; j < N; ++j) {
            result[idx] = matrix[i] * matrix[j];
            idx++;
        }
    }
    return 0;
}

void *compute_matrix_multithread(void *arg) {
    int64_t offset = (int64_t) arg;
    int64_t index_A = 0;
    int64_t index_B = 0;
    int64_t index_C = offset;
    int64_t blk_num[2];

    for (int64_t i = 0; i < t_args.n_blk_row; i += 1) {
        index_A = 0;
        index_B = 0;
        index_C = offset;
        for (int j = 0; j < t_args.n_blk_col; j += 2) {
            if (index_C == 0) {
                blk_num[0] = index_A * t_args.n_blk_row + i;
                blk_num[1] = j * t_args.n_blk_row + i;
                block_computation_multithread(blk_num);
                index_C = THREAD_NUM;
            }
            index_B = index_A + index_C;

            for (int k = j + index_C; k < t_args.n_blk_col; k += THREAD_NUM) {
                blk_num[0] = index_A * t_args.n_blk_row + i;
                blk_num[1] = k * t_args.n_blk_row + i;
                block_computation_multithread(blk_num);

                blk_num[0] = index_B * t_args.n_blk_row + i;
                blk_num[1] = k * t_args.n_blk_row + i;
                block_computation_multithread(blk_num);
                index_B += THREAD_NUM;
            }
            index_A++;
            index_C = (index_C + 1) % THREAD_NUM;
        }
    }
    pthread_exit(NULL);
}


int compute_matrix(int64_t M, int64_t N, int64_t *result) {
    int64_t M_padded = M - M % BSIZE + BSIZE;
    int64_t (*matrix) = calloc(M_padded * N, sizeof(int64_t));
    scan_matrix(M, N, matrix);
    M = M_padded;

#ifdef DEBUG_ON
    print_matrix(M, N, matrix);
    printf("\n");
#endif

    int counter = 0;
    t_args.result = result;
    t_args.matrix = matrix;
    t_args.M = M;
    t_args.N = N;
    t_args.n_pair = THREAD_NUM;
    t_args.n_blk_row = (M + BSIZE - 1) / BSIZE; // round up
    t_args.n_blk_col = (N + t_args.n_pair - 1) / t_args.n_pair; //round up
    t_args.t = calloc(THREAD_NUM, sizeof(pthread_t));
    t_args.l = calloc(THREAD_NUM * THREAD_NUM, sizeof(pthread_mutex_t));

#ifdef DEBUG_ON
    printf("Block per row: %lld column: %lld\n", t_args.n_blk_row, t_args.n_blk_col);
    printf("Result len: %lld\n", RESULT_LEN);
    printf("\n\n");
#endif

    for (int64_t i = 0; i < THREAD_NUM; ++i) {
        pthread_create(&t_args.t[i], NULL, compute_matrix_multithread, (void *) i);
    }

    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_join(t_args.t[i], NULL);
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

    free(matrix);
    free(t_args.t);
    return 0;
}

int main() {

    // We first scan M, N, which is the length of the
    // columns and rows for this array.
    int M, N;

#ifdef PROFILING
    ProfilerStart("efficient.prof");
#endif

    while (!feof(stdin)) {
        scanf("%d", &M);
        scanf("%d", &N);

#ifdef DEBUG_ON
        printf("We got M: %d, N: %d\n", M, N);
#endif
        // Declare 2D array and the result array
        int64_t *result = calloc(RESULT_LEN, sizeof(int64_t));

        // Scan the matrix
        if (N <= 0 || M <= 0) {
            printf("Error: M and N should be greater than 0\n");
        } else if (N == 1 && M == 1) {
            // Only one number in the matrix
            scanf("%lld ", result);
        } else if (N == 1) {
            // Only one Row in the matrix
            compute_single_row(M, N, result);
        } else if (M == 1) {
            // Only one Column in the matrix
            compute_single_coulmn(M, N, result);
        } else {
            // Normal
            compute_matrix(M, N, result);
        }
        // handle result
//        print_arr(result, RESULT_LEN);
        free(result);
    }

#ifdef PROFILING
    ProfilerStop();
#endif

    return 0;
}
