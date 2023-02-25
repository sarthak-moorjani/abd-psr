import random
import string
import sys

def get_random_string(length):
    letters = string.ascii_lowercase
    result_str = ''.join(random.choice(letters) for i in range(length))
    return result_str

all_keys = []

with open('../inputs/input.txt', 'r') as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 3 and parts[0] == "put":
            all_keys.append(parts[1])
clients = 32
total_keys = 100000
for i in range(clients):      
    keys_present = int(0.5*total_keys)
    print(len(all_keys))
    
    write_selected_keys = random.sample(all_keys, keys_present)

    with open('../inputs/write_workload_input_' + str(i) + '.txt', 'w') as f:
        print("Writing to file: \n")
        for key in write_selected_keys:
            f.write("put  " + key + " " + get_random_string(10) + "\n")
    j = len(write_selected_keys)
    print('Generating write workload:\n')
    while j < int(total_keys):
        write_new_key = get_random_string(24)
        if write_new_key not in all_keys:
            with open('../inputs/read_write_workload_input_' + str(i) + '.txt', 'w') as f:
                f.write("put  " + write_new_key + " " + get_random_string(10) + "\n")
            j = j+1


    lines = open('../inputs/write_workload_input_' + str(i) + '.txt').readlines()
    random.shuffle(lines)
    open('../inputs/write_workload_input_' + str(i) + '.txt', 'w').writelines(lines)
    print(str(i) + '.done')

