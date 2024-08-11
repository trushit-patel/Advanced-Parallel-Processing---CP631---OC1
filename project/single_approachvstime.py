import matplotlib.pyplot as plt

# Data for the bar chart
approaches = ['Parallel', 'Serial']
execution_times = [0.46, 11.8]

# Create the bar chart
plt.bar(approaches, execution_times, color=['blue', 'orange', 'red'])

# Add title and labels
plt.title('Mean Execution Times while solving single puzzles')
plt.xlabel('Approach')
plt.ylabel('Execution Time (seconds)')

# Set y-axis limits and ticks
# plt.ylim(0, 13)  # Extend the y-axis range beyond the max value
plt.yticks(range(0, 15, 2))  # Set ticks in steps of 3

# Display the execution time values on top of the bars
for i, time in enumerate(execution_times):
    plt.text(i, time + 0.5, f'{time} s', ha='center')

# Show the plot
plt.show()
