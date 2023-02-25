import random
import string
import sys


clients = 32
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
            
total_keys = 100000
keys_present = int(0.5*total_keys)

print(len(all_keys))

for i in range(clients):
    selected_keys = random.sample(all_keys, keys_present)
    print(len(selected_keys))
    while len(selected_keys) < total_keys:
        new_key = get_random_string(12)
        if new_key not in all_keys:
            selected_keys.append(new_key)

    random.shuffle(selected_keys)
    with open('read_workload_input_' + str(i) + '.txt', 'w') as f:
        for key in selected_keys:
            f.write("get " + key + "\n")

    print('read_workload_input_' + str(i) + '.txt - Done')

