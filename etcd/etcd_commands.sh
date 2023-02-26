# #!/bin/bash

commands_file="etcd_inputs/read_write_workload/read_write_workload_input_${1}.txt"


start_time=$(date +%s.%N)

n=10000


head -$n $commands_file | bash


# < "outputs_${1}.txt"



end_time=$(date +%s.%N)
time_taken=$(echo "$end_time - $start_time" | bc)


echo "Time taken: $time_taken seconds"

# start=$(date +%s.%N)

# cat input_kv.txt | xargs -L1 sh -c 'ETCDCTL_API=3 etcdctl put "$0" "$1"' {}

# end=$(date +%s.%N)


# runtime=$(echo "$end - $start" | bc)

echo "Time taken: $runtime seconds" >> time.txt

