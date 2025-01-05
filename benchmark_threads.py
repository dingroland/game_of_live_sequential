import subprocess
import os
import matplotlib.pyplot as plt
import time
import multiprocessing

# Define paths and parameters
exe_path = r"./game_of_life"
base_path = r"./step1000_in_250generations/"
input_file = os.path.join(base_path, "random10000_in.gol")  # Test one input size
save_file_seq = os.path.join(base_path, "benchmark_cpu_out.gol")
save_file_par = os.path.join(base_path, "benchmark_omp_out.gol")

# Determine max available threads
MAX_THREADS = multiprocessing.cpu_count()

# Store results
thread_counts = list(range(1, MAX_THREADS + 1))
seq_times = []
omp_times = []

# Function to run the program and measure execution time
def run_command(mode, threads=None):
    command = f'{exe_path} --load "{input_file}" --save "{save_file_seq}" --generations 250 --measure --mode {mode}'
    if mode == "omp" and threads:
        command += f' --threads {threads}'
    
    start_time = time.time()
    result = subprocess.run(command, capture_output=True, text=True, shell=True)
    end_time = time.time()

    if result.returncode == 0:
        return end_time - start_time
    else:
        print(f"Error in {mode} mode: {result.stderr.strip()}")
        return None

# Run Sequential Mode for comparison
print("Running Sequential Mode...")
seq_time = run_command("seq")
seq_times = [seq_time] * MAX_THREADS

# Run OpenMP Mode with varying threads
print("Running OpenMP with multiple threads...")
for threads in thread_counts:
    print(f"Testing with {threads} threads...")
    omp_time = run_command("omp", threads)
    omp_times.append(omp_time)

# Plot the results
plt.figure(figsize=(10, 6))
plt.plot(thread_counts, seq_times, label="Sequential Time", linestyle="--")
plt.plot(thread_counts, omp_times, marker='o', label="OpenMP Time")
plt.xlabel("Number of Threads")
plt.ylabel("Execution Time (s)")
plt.title("Performance Comparison: Sequential vs OpenMP")
plt.legend()
plt.grid(True)
plt.show()

# Identify optimal thread count
optimal_threads = thread_counts[omp_times.index(min(omp_times))]
print(f"Optimal thread count: {optimal_threads}")
