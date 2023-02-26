#!/bin/bash

cd /usr/local/src
sudo wget "https://github.com/coreos/etcd/releases/download/v3.3.9/etcd-v3.3.9-linux-amd64.tar.gz"
sudo tar -xvf etcd-v3.3.9-linux-amd64.tar.gz
sudo mv etcd-v3.3.9-linux-amd64/etcd* /usr/local/bin/
sudo mkdir -p /etc/etcd /var/lib/etcd
groupadd -f -g 1501 etcd
useradd -c "etcd user" -d /var/lib/etcd -s /bin/false -g etcd -u 1501 etcd
chown -R etcd:etcd /var/lib/etcd
ETCD_HOST_IP=$(ip addr show eth0 | grep "inet\b" | awk '{print $2}' | cut -d/ -f1)
echo $ETCD_HOST_IP
ETCD_NAME=$(hostname -s)
echo $ETCD_NAME
