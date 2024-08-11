# import matplotlib.pyplot as plt

# # Data
# num_puzzles = [20, 40, 60, 80, 95]
# num_puzzles.reverse()

# cpu_time_1 = [0.558, 0.539, 0.520, 0.462, 0.461, 0.424, 0.458, 0.457, 0.468, 3.24, 4.52, 4.34]
# cpu_time_2 = [0.558, 0.539, 0.520, 0.462, 0.461, 0.424, 0.458, 0.457, 0.468, 3.24, 4.52, 4.34]

# # Plot all times in one graph
# plt.figure(figsize=(10, 6))

# plt.plot(num_threads, cpu_time, marker='o', color='b', label='CPU Time')

# plt.title('Time vs Number of Threads (Logarithmic Scale)')
# plt.xlabel('Number of Threads')
# plt.ylabel('Time (seconds)')
# plt.grid(True, which="both", ls="--")
# plt.legend()

# # Set x-axis to logarithmic scale
# plt.xscale('log')

# # Set x-ticks manually if needed to highlight specific points
# plt.xticks(num_threads, labels=num_threads)

# # Show plot
# plt.show()

import matplotlib.pyplot as plt

# Data
num_puzzles = [20, 40, 60, 80, 95]
# num_puzzles.reverse()

serial = [36.49,  43.16, 71.23, 78.95, 84.54]
complex = [39.14, 33.39, 35.25, 35.98, 35.52]
simple = [24.21, 24.09, 26.48, 26.64, 26.68]

# Plot all times in one graph
plt.figure(figsize=(10, 6))

plt.plot(num_puzzles, serial, marker='o', color='b', label='Serial Time')
plt.plot(num_puzzles, simple, marker='o', color='r', label='Simple Time')
plt.plot(num_puzzles, complex, marker='o', color='g', label='Complex Time')

plt.title('Time vs Number of Puzzles')
plt.xlabel('Number of Puzzles')
plt.ylabel('Time (seconds)')
plt.grid(True)
plt.legend()

# Show plot
plt.show()