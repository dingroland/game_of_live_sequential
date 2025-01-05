import subprocess
import os

NUM_THREADS = 8

# Define paths and parameters
exe_path = r"./game_of_life"
base_path = r"./step1000_in_250generations/"

# Output CSV files for times
cpu_time_csv = "ai24m033_cpu_time.csv"
openmp_time_csv = "ai24m033_openmp_time.csv"

# Initialize the CSV files (clear contents if already exist)
open(cpu_time_csv, "w").close()
open(openmp_time_csv, "w").close()


def run_command(mode, input_file, save_file, csv_file, threads=None):
    print(f"Running in {mode} mode for file {input_file}...")

    # Ensure OpenMP uses the correct thread count via environment variable
    if mode == "omp" and threads:
        os.environ["OMP_NUM_THREADS"] = str(threads)

    command = f'{exe_path} --load "{input_file}" --save "{save_file}" --generations 250 --measure --mode {mode}'
    
    # Add threads flag only for OpenMP mode if supported in your C++ code
    if mode == "omp" and threads:
        command += f" --threads {threads}"

    try:
        result = subprocess.run(command, capture_output=True, text=True, shell=True)
        if result.returncode == 0:
            with open(csv_file, "a") as file:
                file.write(result.stdout.strip() + "\n")
            print(f"Completed {mode} mode for file {input_file}.")
        else:
            print(f"Error in {mode} mode for file {input_file}: {result.stderr.strip()}")
    except Exception as e:
        print(f"Exception in {mode} mode for file {input_file}: {e}")

for size in range(1000, 10001, 1000):
    input_filename = f"random{size}_in.gol"
    input_file = os.path.join(base_path, input_filename)
    save_file_seq = os.path.join(base_path, f"ai24m033_{size}_cpu_out.gol")
    save_file_par = os.path.join(base_path, f"ai24m033_{size}_openmp_out.gol")

    if not os.path.exists(input_file):
        print(f"Error: The input file '{input_file}' does not exist. Skipping...")
        continue

    # Run both sequential and parallel tests
    run_command("seq", input_file, save_file_seq, cpu_time_csv)
    run_command("omp", input_file, save_file_par, openmp_time_csv, threads=NUM_THREADS)

print("All executions completed.")
