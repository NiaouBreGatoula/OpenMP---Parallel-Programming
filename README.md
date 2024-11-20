# Strictly Diagonal Dominant Matrix Operations with OpenMP
<p align="center">
  <img src="https://github.com/user-attachments/assets/773aab4f-87cf-4d8e-a146-b23244573d68" alt="Project Logo" width="800">
</p>
This project implements various parallel operations on a matrix to verify and manipulate strictly diagonally dominant matrices using OpenMP. The code is designed to handle large matrices efficiently by leveraging multithreading and measures the performance of different operations.

---

## Features

1. **Strictly Diagonal Dominance Check**:
   - Verifies if a given matrix \(A(N \times N)\) satisfies the condition:
     \[
     |A_{ii}| > \sum_{j \neq i} |A_{ij}|
     \]
   - This is done in parallel for all rows.

2. **Maximum Absolute Diagonal Element**:
   - Finds the maximum absolute value of the diagonal elements of \(A\) using the `reduction` clause of OpenMP.

3. **Matrix Transformation**:
   - Creates a new matrix \(B\) based on \(A\) and the maximum absolute diagonal value \(m\), where:
     \[
     B_{ij} = m - |A_{ij}| \text{ for } i \neq j \quad \text{and} \quad B_{ii} = m
     \]

4. **Minimum Element in Transformed Matrix**:
   - Calculates the minimum value of matrix \(B\) using:
     - Reduction.
     - Critical section protection.
     - Binary tree reduction.

5. **Performance Measurement**:
   - Measures and compares the execution time of the above operations with different matrix sizes and thread counts.

---

## How to Run

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/OpenMP-Parallel-Programming.git
   cd OpenMP-Parallel-Programming
   gcc OpenMP-Example.c -o omp -fopenmp

