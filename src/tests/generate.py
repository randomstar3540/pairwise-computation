import numpy as np
import sys

int_range = 10000 # Range of the integer in the matrix

large_tests = 4 # Generate how many test files with large matrix
M_range_large = 2500 # Range of total columns
N_range_large = 2500 # Range of total rows

small_tests = 4 # Generate how many test files with small matrix
M_range_small = 100 # Range of total columns
N_range_small = 100 # Range of total rows

fixed_test_A = 4 # Square matrix, M = N
M_fixed_small_A = 50
N_fixed_small_A = 50
M_fixed_large_A = 1000

fixed_test_B = 4 # Matrix has more column than rows, M > N
M_fixed_small_B = 50
N_fixed_small_B = 25
M_fixed_large_B = 1000

fixed_test_C = 4 # Matrix has less column than rows, M < N
M_fixed_small_C = 50
N_fixed_small_C = 150
M_fixed_large_C = 1000



matrix_per_file = 20 # Number of matrix in a file
int16_max = np.iinfo(np.int16).max

def generate_random_matrix(M,N, random=True):
    """_summary_
    Returns:
        M, N, 2D array
    """
    if random:
        M = np.random.randint(1,M)
        N = np.random.randint(1,N)

    return M, N, np.random.randint(-int_range,int_range,size=[N,M])


def sequence_dot_product(A: np.array, B: np.array):
    """_summary_

    Args:
        A (np.array): seqence A
        B (np.array): seqence B

    Returns:
        Result of A·B
    """
    return (A * B).sum()


def compute(M: np.array):
    """_summary_

    Args:
        M (np.array): matrix we are going to compute

    Returns:
        np.array: result
    """
    l = len(M)
    res = np.empty(l *  (l + 1) // 2, dtype=np.int64)
    for i in range(l):
        for j in range(i,l):
            n = sequence_dot_product(M[i],M[j])
            res[l] = n
    return res

def generate_and_write(fname, M, N, random=True):
    print("writing to " + fname + "...")
    f = open(fname, "w+")
    for i in range(matrix_per_file):
        m, n, matrix = generate_random_matrix(M,N, random=random)
        f.write(str(m) + "\n")
        f.write(str(n) + "\n")
        f.write(np.array2string(matrix, max_line_width=int16_max).replace(' [', '').replace('[', '').replace(']', '') + "\n")
        # f.write(np.array2string(result, max_line_width=int16_max).replace(' [', '').replace('[', '').replace(']', ''))
    f.close()

def main():
    np.set_printoptions(threshold=sys.maxsize)
    counter = 1

    for i in range(small_tests):
        generate_and_write("test{}.out".format(counter), M_range_small, N_range_small)
        counter +=1

    for i in range(large_tests):
        generate_and_write("test{}.out".format(counter), M_range_large, N_range_large)
        counter += 1

    # N = N_fixed_small_A
    # counter = 0
    # for M in range(M_fixed_small_A,M_fixed_large_A,M_fixed_small_A):
    #     generate_and_write("fixed_groupA_{}.out".format(counter), M, N, random=False)
    #     counter += 1
    #     N += 50
    #
    # N = N_fixed_small_B
    # counter = 0
    # for M in range(M_fixed_small_B,M_fixed_large_B,M_fixed_small_B):
    #     generate_and_write("fixed_groupB_{}.out".format(counter), M, N, random = False)
    #     counter += 1
    #     N += 50
    #
    # N = N_fixed_small_C
    # counter = 0
    # for M in range(M_fixed_small_C,M_fixed_large_C,M_fixed_small_C):
    #     generate_and_write("fixed_groupC_{}.out".format(counter), M, N, random = False)
    #     counter += 1
    #     N += 50

if __name__ == "__main__":
    main()