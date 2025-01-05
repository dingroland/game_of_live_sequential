import subprocess
import os
import datetime

NUM_THREADS = 8
exe_path = "./game_of_life"
base_path = "./step1000_in_250generations/"
cpu_time_csv = "mai21m033_cpu_time.csv"
openmp_time_csv = "mai21m033_openmp_time.csv"

# Clear the CSV files before writing
open(cpu_time_csv, "w").close()
open(openmp_time_csv, "w").close()

def format_time(ms):
    """Convert milliseconds to hh:mm:ss.ms format."""
    total_seconds = ms / 1000
    formatted_time = str(datetime.timedelta(seconds=total_seconds))
    return formatted_time

def extract_timing(output):
    """Extract timing information from the C++ output."""
    for line in output.splitlines():
        if "computation:" in line:
            computation = float(line.split(":")[1].strip().replace("ms", ""))
        elif "finalization:" in line:
            finalization = float(line.split(":")[1].strip().replace("ms", ""))
        elif "setup:" in line:
            setup = float(line.split(":")[1].strip().replace("ms", ""))
    return format_time(setup), format_time(computation), format_time(finalization)

def run_command(mode, input_file, save_file, csv_file, threads=None):
    """Run the game of life and log results."""
    print(f"Running in {mode} mode for file {input_file}...")
    command = f'{exe_path} --load "{input_file}" --save "{save_file}" --generations 250 --measure --mode {mode}'
    if mode == "omp" and threads:
        os.environ["OMP_NUM_THREADS"] = str(threads)
        command += f" --threads {threads}"

    try:
        result = subprocess.run(command, capture_output=True, text=True, shell=True)
        if result.returncode == 0:
            setup, computation, finalization = extract_timing(result.stdout)
            with open(csv_file, "a") as file:
                file.write(f"{setup};{computation};{finalization}\n")
            print(f"Completed {mode} mode for file {input_file}.")
        else:
            print(f"Error in {mode} mode: {result.stderr.strip()}")
    except Exception as e:
        print(f"Exception in {mode} mode for file {input_file}: {e}")

# Run tests for grid sizes 1000 to 10000 with increments of 1000
for size in range(1000, 10001, 1000):
    input_file = os.path.join(base_path, f"random{size}_in.gol")
    save_file_seq = os.path.join(base_path, f"mai21mxxx_{size}_cpu_out.gol")
    save_file_omp = os.path.join(base_path, f"mai21mxxx_{size}_openmp_out.gol")

    if not os.path.exists(input_file):
        print(f"Error: Input file '{input_file}' not found.")
        continue

    # Run both sequential and OpenMP versions
    run_command("seq", input_file, save_file_seq, cpu_time_csv)
    run_command("omp", input_file, save_file_omp, openmp_time_csv, threads=NUM_THREADS)

print("Benchmarking complete!")
