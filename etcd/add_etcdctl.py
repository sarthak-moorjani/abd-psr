import os


input_dir = "../inputs/read_write_workload"
etcdctl_output_dir = "./etcd_inputs/read_write_workload"

initialise_input_path = "../inputs/input.txt"
initialise_output_path = "./input.txt"

if not os.path.exists(etcdctl_output_dir):
    os.makedirs(etcdctl_output_dir)


for filename in os.listdir(input_dir):
    if filename.startswith("read_write_workload_input"):
        with open(os.path.join(input_dir, filename), "r") as input_file:
            print(filename)
            contents = input_file.read()
            modified_contents = ""
            for line in contents.splitlines():
                modified_contents += "ETCDCTL_API=3 etcdctl " + line + "\n"

            with open(os.path.join(etcdctl_output_dir, filename), "w") as output_file:
                output_file.write(modified_contents)

# Add etcdctl to Initialse 
with open(initialise_input_path, "r") as input_file:
    print("Adding etcdctl to initialise")
    contents = input_file.read()
    modified_contents = ""
    for line in contents.splitlines():
        modified_contents += "ETCDCTL_API=3 etcdctl " + line + "\n"

    with open(initialise_output_path, "w") as output_file:
        output_file.write(modified_contents)
    print("Done.")
