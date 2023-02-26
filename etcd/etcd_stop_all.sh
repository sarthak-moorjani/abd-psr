#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Error: Expects Cloudlab Username (Case-sensitive)"
  exit 1
fi

USER=$1
declare -a nodes=("$USER@clnode288.clemson.cloudlab.us" "$USER@clnode315.clemson.cloudlab.us" "$USER@clnode294.clemson.cloudlab.us" "$USER@clnode304.clemson.cloudlab.us" "$USER@clnode303.clemson.cloudlab.us")

for i in "${nodes[@]}"
do
    ssh -i ~/.ssh/id_ed25519 -t $i 'sudo systemctl stop etcd'
done
