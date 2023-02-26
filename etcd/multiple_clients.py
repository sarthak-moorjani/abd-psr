import subprocess
import os
import random



num_clients = 10

client_command = "python3 client.py"
file_dir = "/users/Ramya/abd-psr/etcd/etcd_inputs/"
file_list = os.listdir(file_dir)


selected_files = random.sample(file_list, num_clients)

for i in range(num_clients):
    file_path = os.path.join(file_dir, selected_files[i]) 
    full_command = f"{client_command} {file_path}"
    subprocess.Popen(full_command, shell=True)