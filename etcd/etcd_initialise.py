import subprocess
import time

with open('input.txt', 'r') as f:
    lines = f.readlines()

total_time = 0.0
num_calls = 0

for line in lines:
    line = line.strip()
    start_time = time.monotonic()
    subprocess.run(line, shell=True)
    end_time = time.monotonic()
    
    call_time = end_time - start_time
    total_time += call_time
    num_calls += 1
    
    print(f"Call time: {call_time:.6f} seconds")

if num_calls > 0:
    avg_time = total_time / num_calls
    print(f"Average time per subprocess put call during initialisation: {avg_time:.6f} seconds")
else:
    print("No subprocess calls were made.")

with open('measurements.txt', 'a') as f:
    f.write(f'Time taken to initialise 1M records: {total_time:.6f} seconds\n')
    f.write(f'Average time per put call during initialisation:: {avg_time:.6f} seconds\n')

