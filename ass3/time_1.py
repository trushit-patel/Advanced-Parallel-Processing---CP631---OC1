# import matplotlib.pyplot as plt

# # Data
# num_particles = [16384, 32768, 65536, 98304]  # Reversed order
# cpu_time = [820, 3210, 13010, 29350]
# gpu_k1_time = [15.136, 13.475, 92.124, 183.467]
# gpu_k2_time = [34.444, 103.584, 329.875, 351.843]

# # Plot CPU time
# plt.figure(figsize=(12, 8))

# plt.subplot(3, 1, 1)
# plt.plot(num_particles, cpu_time, marker='o', color='b')
# plt.title('CPU Time vs Number of Particles')
# plt.xlabel('Number of Particles')
# plt.ylabel('CPU Time (ms)')
# plt.grid(True)

# # Plot GPU k1 time
# plt.subplot(3, 1, 2)
# plt.plot(num_particles, gpu_k1_time, marker='o', color='r')
# plt.title('GPU k1 Time vs Number of Particles')
# plt.xlabel('Number of Particles')
# plt.ylabel('GPU k1 Time (ms)')
# plt.grid(True)

# # Plot GPU k2 time
# plt.subplot(3, 1, 3)
# plt.plot(num_particles, gpu_k2_time, marker='o', color='g')
# plt.title('GPU k2 Time vs Number of Particles')
# plt.xlabel('Number of Particles')
# plt.ylabel('GPU k2 Time (ms)')
# plt.grid(True)

# # Adjust layout
# plt.tight_layout()
# plt.show()


import matplotlib.pyplot as plt

# Data
num_particles = [16384, 32768, 65536, 98304]  # Correct order
cpu_time = [820, 3210, 13010, 29350]
gpu_k1_time = [15.136, 13.475, 92.124, 183.467]
gpu_k2_time = [34.444, 103.584, 329.875, 351.843]

# Plot all times in one graph
plt.figure(figsize=(10, 6))

plt.plot(num_particles, cpu_time, marker='o', color='b', label='CPU Time')
plt.plot(num_particles, gpu_k1_time, marker='o', color='r', label='GPU k1 Time')
plt.plot(num_particles, gpu_k2_time, marker='o', color='g', label='GPU k2 Time')

plt.title('Time vs Number of Particles')
plt.xlabel('Number of Particles')
plt.ylabel('Time (ms)')
plt.grid(True)
plt.legend()

# Show plot
plt.show()
