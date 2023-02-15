#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Error: Expects Cloudlab Username (Case-sensitive)"
  exit 1
fi

USER=$1
declare -a nodes=("$USER@hp152.utah.cloudlab.us" "$USER@hp191.utah.cloudlab.us" "$USER@hp160.utah.cloudlab.us" "$USER@hp177.utah.cloudlab.us")

for i in "${nodes[@]}"
do
    ssh -i ~/.ssh/id_ed25519 -t $i 'cd /users/$USER/abd-psr/  && git pull origin main'
done
