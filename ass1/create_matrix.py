import numpy as np

# Define the size of the matrix
n = 1000

# Generate a 1000x1000 matrix where each row is from 1 to 1000
matrix = np.tile(np.arange(1, n + 1), (n, 1))

# Define the output file name
output_file = "matrix_1000x1000.txt"

# Write the matrix to the file
with open(output_file, "w") as f:
    for row in matrix:
        f.write(" ".join(map(str, row)) + "\n")

print(f"Matrix {n}x{n} has been written to {output_file}")
