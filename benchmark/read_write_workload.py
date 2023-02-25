import random
import string
import sys

def get_random_string(length):
    letters = string.ascii_lowercase
    result_str = ''.join(random.choice(letters) for i in range(length))
    return result_str

all_keys = []
key_value_dict = {}
with open('../inputs/input.txt', 'r') as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 3 and parts[0] == "put":
            all_keys.append(parts[1])
            key_value_dict[parts[1]] = parts[2]
clients = 32
for i in range(clients):      
    total_keys = 100000
    keys_present = int(0.25*total_keys)

    print(len(all_keys))
    read_selected_keys  = random.sample(all_keys, keys_present)
    
    write_selected_keys = random.sample(all_keys, keys_present)

    with open('../inputs/read_write_workload_input_' + str(i) + '.txt', 'w') as f:
        print("Writing to file: \n")
        for key in read_selected_keys:
            f.write("get " + key + "\n")
        for key in write_selected_keys:
            f.write("put  " + key + " " + get_random_string(10) + "\n")
    print('Random read keys selected length: ' + str(len((read_selected_keys))))
    print('Random write keys selected length: ' + str(len(read_selected_keys)))
    print('Generating read workload:\n')
    j = len(read_selected_keys)
    while j < int(total_keys/2):
        print(j)
        read_new_key = get_random_string(10)
        if read_new_key not in all_keys:
            with open('../inputs/read_write_workload_input_' + str(i) + '.txt', 'w') as f:
                f.write("get " + read_new_key + "\n")
            j = j+1
    
    j = len(write_selected_keys)
    print('Generating write workload:\n')
    while j < int(total_keys/2):
        write_new_key = get_random_string(24)
        if write_new_key not in all_keys:
            with open('../inputs/read_write_workload_input_' + str(i) + '.txt', 'w') as f:
                f.write("put  " + write_new_key + " " + get_random_string(10) + "\n")
            j = j+1


    lines = open('../inputs/read_write_workload_input_' + str(i) + '.txt').readlines()
    random.shuffle(lines)
    open('../inputs/read_write_workload_input_' + str(i) + '.txt', 'w').writelines(lines)
    print(i + '.done')

