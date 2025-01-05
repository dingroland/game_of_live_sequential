import subprocess
import os

# Define paths and parameters
exe_path = r".\GameOfLife.exe"
base_path = r"C:\Users\Carlos Tichy\Documents\MAI\1stSem\Advanced_Programming\step1000_in_250generations"

# Output CSV files for times
cpu_time_csv = "ai24m033_cpu_time.csv"
openmp_time_csv = "ai24m033_openmp_time.csv"

# Initialize the CSV files (clear contents if already exist)
open(cpu_time_csv, "w").close()
open(openmp_time_csv, "w").close()


# Function to run the command and store results directly to a CSV
def run_command(mode, input_file, save_file, csv_file):
    print(f"Running in {mode} mode for file {input_file}...")
    command = f'{exe_path} --load "{input_file}" --save "{save_file}" --generations 250 --measure --mode {mode}'
    try:
        result = subprocess.run(command, capture_output=True, text=True, shell=True)
        if result.returncode == 0:
            # Write raw output times directly to the CSV file
            with open(csv_file, "a") as file:
                file.write(result.stdout.strip() + "\n")
            print(f"Completed {mode} mode for file {input_file}.")
        else:
            print(f"Error in {mode} mode for file {input_file}: {result.stderr.strip()}")
    except Exception as e:
        print(f"Exception in {mode} mode for file {input_file}: {e}")


# Loop for file sizes from 1000 to 10000 in increments of 1000
for size in range(1000, 10001, 1000):
    input_filename = f"random{size}_in.gol"
    input_file = os.path.join(base_path, input_filename)

    # Output file names
    save_file_seq = os.path.join(base_path, f"ai24m033_{size}_cpu_out.gol")
    save_file_par = os.path.join(base_path, f"ai24m033_{size}_openmp_out.gol")

    # Check if the input file exists
    if not os.path.exists(input_file):
        print(f"Error: The input file '{input_file}' does not exist. Skipping...")
        continue

    # Run sequential mode and append times to the CPU CSV
    run_command("seq", input_file, save_file_seq, cpu_time_csv)

    # Run parallel mode and append times to the OpenMP CSV
    run_command("omp", input_file, save_file_par, openmp_time_csv)

print("All executions completed.")
