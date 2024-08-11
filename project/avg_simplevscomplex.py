import matplotlib.pyplot as plt

# Data for the bar chart
approaches = ['Simple Approach', 'Complex Approach', 'Serial']
execution_times = [26.59, 35.52, 62.4]

# Create the bar chart
plt.bar(approaches, execution_times, color=['blue', 'orange', 'red'])

# Add title and labels
plt.title('Comparison of Mean Execution Times')
plt.xlabel('Approach')
plt.ylabel('Execution Time (seconds)')

# Display the execution time values on top of the bars
for i, time in enumerate(execution_times):
    plt.text(i, time + 0.5, f'{time} s', ha='center')

# Show the plot
plt.show()
