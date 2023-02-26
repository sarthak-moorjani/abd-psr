with open('input.txt', 'r') as input_file, open('input_kv.txt', 'w') as output_file:
    for line in input_file:
        parts = line.strip().split()
        key = parts[3]
        value = parts[4]
        output_file.write(f"{key}={value}\n")
