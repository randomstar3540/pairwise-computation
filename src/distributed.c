#include "include/distributed.h"

struct thread_args t_args;
MPI_Status status;
int PID = 0, BSIZE = 0, T = 0, BLK_NUM = 0, STEP = 0;

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
    if (size % 16 == 0){
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
    }else if (size % 8 == 0){
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
    }else if (size % 4 == 0){
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

int compute_single_row(int64_t M, int64_t N, int64_t *result) {
    int64_t * matrix = malloc(M * sizeof (int64_t));
    int64_t res;
    for (int i = 0; i < M; i++) {
        result[0] += matrix[i] * matrix[i];
    }
    return 0;
}

int compute_single_coulmn(int64_t M, int64_t N, int64_t *result) {
    int idx = 0;
    int64_t * matrix = malloc(N * sizeof (int64_t));
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

    if (offset > t_args.n_row_blk){
        pthread_exit(NULL);
    }

    int64_t res;
    int64_t * row_A;
    int64_t * row_B;

    // Start Row number of the two blocks we are going to compute
    // pairwise(self, other)
    int self = PID * t_args.n_row_blk;
    int other = ((PID + STEP) % BLK_NUM) * t_args.n_row_blk;

    for(int i = offset; i < t_args.n_row_blk; i += T){
        for(int j = 0; j < t_args.n_row_blk; j ++){
            row_A = t_args.self_matrix + ((self + i) * t_args.M);
            row_B = t_args.recv_matrix + (j * t_args.M);
            res = compute_dot_product(row_A,row_B,t_args.M);
            t_args.result[i * t_args.N + other + j] = res;
        }
    }

    pthread_exit(NULL);
}

/*
 * Our function to handle matrix computation
 * M: How many columns
 * N: How many rows
 * Result: array to store the result
 */
int compute_matrix(int64_t M, int64_t N, int64_t *result) {
    int64_t * matrix = calloc(M * N,sizeof(int64_t));

    // Fill in the matrix with random data
    if(PID == 0){
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                matrix[i * M + j] = rand() % 51;
            }
        }
    }

    int64_t *result_temp = calloc(N * N,sizeof(int64_t));
    int64_t *recv_matrix = malloc((N / BLK_NUM )* M * sizeof(int64_t));
    int step = (BLK_NUM + 1) / 2;

    int counter = 0;
    t_args.result = result_temp;
    t_args.self_matrix = matrix;
    t_args.recv_matrix = recv_matrix;
    t_args.M = M;
    t_args.N = N;
    t_args.n_row_blk = N / BLK_NUM;
    t_args.t = calloc(T, sizeof(pthread_t));

#ifdef DEBUG_ON
    if (PID == 0){
        print_matrix(M, N, matrix);
        printf("\n");
        printf("Block per column: %lld\n", t_args.n_row_blk);
        printf("Result len: %lld\n", RESULT_LEN);
        printf("\n\n");
    }
#endif

    // Init, send data to all others process
    if (PID == 0){
        for (int i = 1; i < BLK_NUM; i++){
            MPI_Send(&matrix[i * t_args.n_row_blk * t_args.M], t_args.n_row_blk * M, MPI_INT64_T, i, 1, MPI_COMM_WORLD);
        }
    }else{
        MPI_Recv(&matrix[PID * t_args.n_row_blk * t_args.M], t_args.n_row_blk * M, MPI_INT64_T, 0, 1, MPI_COMM_WORLD, &status);
    }
    // Copy matrix
    for (int i = 0;i < t_args.n_row_blk; i++){
        for (int j = 0;j < M;j++){
            recv_matrix[i * M + j] = matrix[PID * t_args.n_row_blk * t_args.M + i * M + j];
        }
    }
#ifdef DEBUG_ON
    if (PID == 1){
        print_matrix(M,t_args.n_row_blk,recv_matrix);
    }
#endif

    // In each step
    for (int cnt = 0;cnt < step; cnt++){
        STEP = cnt;

        for (int64_t t = 0; t < T; t++){
            pthread_create(&t_args.t[t], NULL, compute_matrix_multithread, (void *) t);
        }

        for (int t = 0; t < T; t++){
            pthread_join(t_args.t[t], NULL);
        }

        if (cnt != step - 1){
            // Avoid Deadlock
            if (PID %2 == 0){
                MPI_Send(&recv_matrix[0], t_args.n_row_blk * M, MPI_INT64_T, (PID - 1 + BLK_NUM) % BLK_NUM, 1, MPI_COMM_WORLD);
                MPI_Recv(&recv_matrix[0], t_args.n_row_blk * M, MPI_INT64_T, (PID + 1 + BLK_NUM) % BLK_NUM, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            }else{
                MPI_Recv(&recv_matrix[0], t_args.n_row_blk * M, MPI_INT64_T, (PID + 1 + BLK_NUM) % BLK_NUM, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                MPI_Send(&recv_matrix[0], t_args.n_row_blk * M, MPI_INT64_T, (PID - 1 + BLK_NUM) % BLK_NUM, 1, MPI_COMM_WORLD);
            }
        }
    }

    if (PID != 0){
        // Send results back to process 0
        MPI_Send(&result_temp[0], t_args.n_row_blk * N, MPI_INT64_T, 0, 1, MPI_COMM_WORLD);
    }else{
        int count = 0;
        for (int i = 1;i < BLK_NUM; i++){
            MPI_Recv(&result_temp[i * t_args.n_row_blk * t_args.N], t_args.n_row_blk * t_args.N, MPI_INT64_T, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
    }
    // Reshape result
    int row_counter = 0;
    int delta = step * t_args.n_row_blk;
    for (int i = 0;i < t_args.N; i++){
        for (int j = i;j < t_args.N; j++){
            if (i < step * t_args.n_row_blk && j >= delta){
                result_temp[i * t_args.N + j] = result_temp[j * t_args.N + i];
            }
        }
        row_counter ++;
        if (row_counter >= t_args.n_row_blk){
            delta+=t_args.n_row_blk;
            row_counter = 0;
        }
    }
    // Sort result
    int count = 0;
    for (int i = 0;i < t_args.N; i++){
        int row_counter = 0;
        for (int j = i;j < t_args.N; j++){
            result[count] = result_temp[i * t_args.N + j];
            count++;
        }
    }

#ifdef SELF_CHECK
    if (PID == 0){
        int error = 0;
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
                error ++;
            }
    #ifdef DEBUG_ON
    //        else {
    //            printf("[INFO]Except %lld got %lld.\n", sequential[i], result[i]);
    //        }
    #endif
        }
        free(sequential);
        if (error == 0){
            printf("\n===================================================\n");
            printf("\n[INFO] Self Test PASSED.\n");
            printf("\n===================================================\n\n");
        }else {
            printf("\n===================================================\n");
            printf("\n[INFO] Self Test FAILED with %d error occurred.\n", error);
            printf("\n===================================================\n\n");
        }
    }
#endif

    free(matrix);
    free(t_args.t);
    return 0;
}

void panic(char * s){
    printf("%s",s);
    MPI_Finalize();
    exit(0);
}

int main(int argc, char *argv[]) {

    // We first scan M, N, which is the length of the
    // columns and rows for this array.
    int M = 0, N = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &BLK_NUM);
    MPI_Comm_rank(MPI_COMM_WORLD, &PID);

    if (argc == 4){
        N = atoi(argv[1]);
        M = atoi(argv[2]);
        T = atoi(argv[3]);
    }else{
        panic("Usage: mpirun –np [How many processes] [Filename] [Rows] [Columns] [How many threads]\n\n");
    }

#ifdef DEBUG_ON
    printf("We got M: %d, N: %d, T:%d, BLK:%d, PID:%d\n", M, N, T, BLK_NUM, PID);
#endif

    if (BLK_NUM % 2 == 0) {
        panic("[Computation fails] Number of processes should be odd\n");
    }

#ifdef PROFILING
    ProfilerStart("efficient.prof");
#endif
    if (N % BLK_NUM != 0){
        panic("[Computation fails] numer of rows in this matrix should be divisible by the process runs\n");
    }


    // Declare 2D array and the result array
    int64_t *result = calloc(RESULT_LEN, sizeof(int64_t));

    // Scan the matrix
    if (N <= 0 || M <= 0) {
        printf("[Computation fails] M and N should be greater than 0\n");
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
    if (PID == 0){
        print_arr(result, RESULT_LEN);
    }
    free(result);

#ifdef PROFILING
    ProfilerStop();
#endif
    MPI_Finalize();
    return 0;
}
