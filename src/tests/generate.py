import random
from traceback import print_tb
import numpy as np

tests = 10 # Generate how many test files
int_range = 1000 # Range of the integer
M_range = 10 # Range of total columns
N_range = 10 # Range of total rows
int16_max = np.iinfo(np.int16).max

def generate_random_matrix():
    """_summary_
    Returns:
        M, N, 2D array
    """

    M = np.random.randint(1,M_range)
    N = np.random.randint(1,N_range)

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
    res = np.empty(0, dtype=np.int64)
    for i in range(l):
        for j in range(i,l):
            n = sequence_dot_product(M[i],M[j])
            res = np.append(res,n)
    return res


def generate_and_write(fname):
    print(fname)
    M, N , matrix = generate_random_matrix()
    result = compute(matrix)

    # print(str(M))
    # print(str(N))
    # print(str(matrix).replace(' [', '').replace('[', '').replace(']', ''))
    # print(str(result).replace(' [', '').replace('[', '').replace(']', ''))

    f = open(fname, "w+")
    f.write(str(M) + "\n")
    f.write(str(N) + "\n")
    f.write(np.array2string(matrix, max_line_width=int16_max).replace(' [', '').replace('[', '').replace(']', '') + "\n")
    f.write(np.array2string(result, max_line_width=int16_max).replace(' [', '').replace('[', '').replace(']', ''))
    f.close()

def main():
    for i in range(tests):
        generate_and_write("test{}.out".format(i))

if __name__ == "__main__":
    main()