#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Error: Expects Cloudlab Username (Case-sensitive)"
  exit 1
fi

USER=$1
declare -a nodes=("$USER@clnode288.clemson.cloudlab.us" "$USER@clnode315.clemson.cloudlab.us" "$USER@clnode294.clemson.cloudlab.us" "$USER@clnode304.clemson.cloudlab.us" "$USER@clnode303.clemson.cloudlab.us")

for i in "${nodes[@]}"
do
  ssh-copy-id $i
  scp ~/.ssh/id_ed25519.pub $i:/users/$USER/.ssh/id_ed25519.pub
  scp ~/.ssh/id_ed25519 $i:/users/$USER/.ssh/id_ed25519
  sudo ssh -i ~/.ssh/id_ed25519 -t $i 'eval "$(ssh-agent -s)"  && ssh-add /users/$USER/.ssh/id_ed25519'
done