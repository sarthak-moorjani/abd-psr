import subprocess


with open('input.txt', 'r') as f:
    lines = f.readlines()

for line in lines:

    line = line.strip()
    subprocess.run(line, shell=True)