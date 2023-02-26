# #!/bin/bash

# commands_file="input.txt"


# start_time=$(date +%s.%N)

# while read command; do

#   eval $command
# done < $commands_file


# end_time=$(date +%s.%N)
# time_taken=$(echo "$end_time - $start_time" | bc)


# echo "Time taken: $time_taken seconds"

start=$(date +%s.%N)

cat input_kv.txt | xargs -L1 sh -c 'ETCDCTL_API=3 etcdctl put "$0" "$1"' {}

end=$(date +%s.%N)


runtime=$(echo "$end - $start" | bc)

echo "Time taken: $runtime seconds" >> time.txt

