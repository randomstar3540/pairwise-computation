# All Pairwise Computation
All Pairwise Computation is defined as performing computation (e.g., correlations) between  
every pair of the elements in a given dataset. In this assignment, we consider a set of N  
sequences, each being of length M, which are stored in a two-dimensional (2D) matrix of size  
N by M (i.e., N rows and M columns). We then calculate a dot product for every possible pair  
of sequences (row vectors) in the matrix.  
A dot product of two sequences, or vectors is defined as

$$x \cdot y = \sum_{i=0}^{M-1}{x_iy_i}$$

If we allow a vector to pair with itself, then there are N(N+1)/2 such pairs in total for N vectors.  
Assume N is equal to 5 for example. We then have the following 15 vector pairs:
$$(0,0) (0,1) (0,2) (0,3) (0,4)$$
$$\;\;\;\;\;\;\;\;\; (1,1) (1,2) (1,3) (1,4)$$
$$\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;(2,2) (2,3) (2,4)$$
$$\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;(3,3) (3,4)$$
$$\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;\;(3,4)$$

In the above dot product formula, $(i, j)$ denotes the pair of sequences $i$ and $j$. After the dot  
product computation for pair $(i, j)$, we will have a single value as the output. Let $a_i$ and $a_j$ be  
two sequences, namely the $i^{th}$ and $j^{th}$ row vectors in the 2D matrix, and $c_{ij} = a_i\cdot a_j$. We can  
store the computational results in a one-dimensional (1D) array of size N(N+1)/2 in a row major  
order:
| $c_{00}$ 	| $c_{01}$ 	| $c_{02}$ 	| $c_{03}$ 	| $c_{04}$ 	| $c_{11}$ 	| $c_{12}$ 	| $c_{13}$ 	| $c_{14}$ 	| $c_{22}$ 	| $c_{23}$ 	| $c_{24}$ 	| $c_{33}$ 	| $c_{34}$ 	| $c_{44}$ 	|

And here we aim to design an efficient parallel algorithm to solve the above  
problem on a shared-memory and a distributed memory computing platform and implement the algorithm with Pthreads and openMPI.