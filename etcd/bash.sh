#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Error: Expects Cloudlab Username (Case-sensitive)"
  exit 1
fi

USER=$1

declare -a nodes=("$USER@hp152.utah.cloudlab.us" "$USER@hp191.utah.cloudlab.us" "$USER@hp160.utah.cloudlab.us" "$USER@hp177.utah.cloudlab.us")


sudo scp -i ~/.ssh/id_ed25519 /Users/ramyabygari/Desktop/CS598-FTS/abd-psr/etcd/node0 ${nodes[0]}:/users/Ramya/
sudo scp -i ~/.ssh/id_ed25519 /Users/ramyabygari/Desktop/CS598-FTS/abd-psr/etcd/node1 ${nodes[1]}:/users/Ramya/
sudo scp -i ~/.ssh/id_ed25519 /Users/ramyabygari/Desktop/CS598-FTS/abd-psr/etcd/node2 ${nodes[2]}:/users/Ramya/
sudo scp -i ~/.ssh/id_ed25519 /Users/ramyabygari/Desktop/CS598-FTS/abd-psr/etcd/node3 ${nodes[3]}:/users/Ramya/

for i in "${nodes[@]}"
do
  sudo scp -i ~/.ssh/id_ed25519 /Users/ramyabygari/Desktop/CS598-FTS/abd-psr/etcd/etcd.service $i:/users/Ramya/
done 



ssh -i ~/.ssh/id_ed25519 -t ${nodes[0]}  'cd /usr/local/src/ && sudo wget "https://github.com/coreos/etcd/releases/download/v3.3.9/etcd-v3.3.9-linux-amd64.tar.gz" && sudo tar -xvf etcd-v3.3.9-linux-amd64.tar.gz && sudo mv etcd-v3.3.9-linux-amd64/etcd* /usr/bin/ && sudo chmod +x /usr/bin/etcd* && sudo cp /users/Ramya/node0 /etc/etcd/ && sudo cp /users/Ramya/etcd.service /etc/systemd/system/'
ssh -i ~/.ssh/id_ed25519 -t ${nodes[1]}  'cd /usr/local/src/ && sudo wget "https://github.com/coreos/etcd/releases/download/v3.3.9/etcd-v3.3.9-linux-amd64.tar.gz" && sudo tar -xvf etcd-v3.3.9-linux-amd64.tar.gz && sudo mv etcd-v3.3.9-linux-amd64/etcd* /usr/bin/ && sudo chmod +x /usr/bin/etcd* && sudo cp /users/Ramya/node1 /etc/etcd/ && sudo cp /users/Ramya/etcd.service /etc/systemd/system/'
ssh -i ~/.ssh/id_ed25519 -t ${nodes[2]}  'cd /usr/local/src/ && sudo wget "https://github.com/coreos/etcd/releases/download/v3.3.9/etcd-v3.3.9-linux-amd64.tar.gz" && sudo tar -xvf etcd-v3.3.9-linux-amd64.tar.gz && sudo mv etcd-v3.3.9-linux-amd64/etcd* /usr/bin/ && sudo chmod +x /usr/bin/etcd* && sudo cp /users/Ramya/node2 /etc/etcd/ && sudo cp /users/Ramya/etcd.service /etc/systemd/system/'
ssh -i ~/.ssh/id_ed25519 -t ${nodes[3]}  'cd /usr/local/src/ && sudo wget "https://github.com/coreos/etcd/releases/download/v3.3.9/etcd-v3.3.9-linux-amd64.tar.gz" && sudo tar -xvf etcd-v3.3.9-linux-amd64.tar.gz && sudo mv etcd-v3.3.9-linux-amd64/etcd* /usr/bin/ && sudo chmod +x /usr/bin/etcd* && sudo cp /users/Ramya/node3 /etc/etcd/ && sudo cp /users/Ramya/etcd.service /etc/systemd/system/'


#                                     
# ssh -i ~/.ssh/id_ed25519 -t ${nodes[2]}  'wget -q --show-progress "https://github.com/etcd-io/etcd/releases/download/v3.5.0/etcd-v3.5.0-linux-amd64.tar.gz" 
#                                     && tar zxf etcd-v3.5.0-linux-amd64.tar.gz
#                                     && mv etcd-v3.5.0-linux-amd64/etcd* /usr/bin/
#                                     && sudo chmod +x /usr/bin/etcd*
#                                     && touch /etc/etcd
#                                     && mv /Users/ramyabygari/Desktop/CS598-FTS/abd-psr/etcd/node2 /etc/etcd
#                                     && touch /etc/systemd/system/etcd.service
#                                     && mv /Users/ramyabygari/Desktop/CS598-FTS/abd-psr/etcd/etcd.service /etc/systemd/system/etcd.service
#                                     '
# ssh -i ~/.ssh/id_ed25519 -t ${nodes[3]}  'wget -q --show-progress "https://github.com/etcd-io/etcd/releases/download/v3.5.0/etcd-v3.5.0-linux-amd64.tar.gz" 
#                                     && tar zxf etcd-v3.5.0-linux-amd64.tar.gz
#                                     && mv etcd-v3.5.0-linux-amd64/etcd* /usr/bin/
#                                     && sudo chmod +x /usr/bin/etcd*
#                                     && touch /etc/etcd
#                                     && mv /Users/ramyabygari/Desktop/CS598-FTS/abd-psr/etcd/node4 /etc/etcd
#                                     && touch /etc/systemd/system/etcd.service
#                                     && mv /Users/ramyabygari/Desktop/CS598-FTS/abd-psr/etcd/etcd.service /etc/systemd/system/etcd.service
#                                     '
