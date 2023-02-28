#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Error: Expects Cloudlab Username (Case-sensitive)"
  exit 1
fi

USER=$1

if [ $# -lt 4 ]; then
  echo "Error: Expects SERVER IP AND PORT"
  exit 1
fi

PORT=":50052"

SERVER1=$2
SERVER2=$3
SERVER3=$4


ssh $USER@$SERVER1 "pkill -f abd_replica" &
sleep 5
ssh $USER@$SERVER2 "pkill -f abd_replica" &
sleep 5
ssh $USER@$SERVER3 "pkill -f abd_replica" &
sleep 5

#Start Replicas
ssh  $USER@$SERVER2 " cd abd-psr/scripts &&  bash server.sh 2" &
ssh  $USER@$SERVER1 " cd abd-psr/scripts &&  bash server.sh 1" &
ssh  $USER@$SERVER3 " cd abd-psr/scripts &&  bash server.sh 3" &

SERVERPORT1="$SERVER1${PORT}"
SERVERPORT2="$SERVER2${PORT}"
SERVERPORT3="$SERVER3${PORT}"

# install gnu-parallel
sudo apt-get update
sudo apt-get install parallel

clients=8

# change working directory from scripts
pushd ../

echo $(pwd)

# Compile
rm -rf src/abd-algo/cmake/build
mkdir -p src/abd-algo/cmake/build
pushd src/abd-algo/cmake/build
cmake -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl -DOPENSSL_LIBRARIES=/opt/homebrew/opt/openssl/lib -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
make -j 4
popd

# make one client initialise 
pushd src/abd-algo/cmake/build
./abd_client true input100.txt input.txt $USER $SERVER1 $SERVER2 $SERVER3
popd
#exit

export SHELL=$(type -p bash)
export SERVERPORT1=$SERVERPORT1
export SERVERPORT2=$SERVERPORT2
export SERVERPORT3=$SERVERPORT3


run_cpp_executable() {
    input_file=$1
    output_file="${input_file%.*}_output.txt"
    echo "Processing ${input_file} -> ${output_file}"
    echo $SERVER1
    ./abd_client false ${input_file} ${output_file} $USER $SERVERPORT1 $SERVERPORT2 $SERVERPORT3
}

export -f run_cpp_executable

echo $(pwd)
# List of read_workload nput files to process
pushd inputs/read_workload
input_files=($(ls -1t ./read_workload_input*.txt | head -n $clients))
echo "${input_files[@]}"
popd

echo $input_files
# exit
#  Run gnu-parallel
pushd ./src/abd-algo/cmake/build

parallel run_cpp_executable {} ::: "${input_files[@]}"

ssh $USER@$SERVER1 "pkill -f abd_replica"
ssh $USER@$SERVER2 "pkill -f abd_replica"
ssh $USER@$SERVER3 "pkill -f abd_replica"


