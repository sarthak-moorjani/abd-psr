#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Error: Expects Cloudlab Username (Case-sensitive)"
  exit 1
fi

USER=$1
declare -a nodes=("$USER@c220g2-011124.wisc.cloudlab.us" "$USER@c220g2-011118.wisc.cloudlab.us" "$USER@c220g2-011122.wisc.cloudlab.us" "$USER@c220g2-011119.wisc.cloudlab.us" "$USER@c220g2-011123.wisc.cloudlab.us")

for i in "${nodes[@]}"
do
    ssh -i ~/.ssh/id_ed25519 -t $i 'rm -rf abd-psr && eval `ssh-agent` && ssh-add /users/'$USER'/.ssh/id_ed25519 && git clone git@github.com:sarthak-moorjani/abd-psr.git'
done
