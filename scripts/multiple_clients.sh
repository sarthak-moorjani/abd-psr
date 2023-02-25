#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Error: Expects Cloudlab Username (Case-sensitive)"
  exit 1
fi

USER=$1

# install gnu-parallel
sudo apt-get update
sudo apt-get install parallel

clients=2

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
./abd_client true input.txt input.txt $USER
popd


export SHELL=$(type -p bash)

run_cpp_executable() {
    input_file=$1
    output_file="${input_file%.*}_output.txt"
    echo "Processing ${input_file} -> ${output_file}"
    ./abd_client false  ${input_file} ${output_file} $USER
}

export -f run_cpp_executable

echo $(pwd)
# List of read_workload nput files to process
pushd inputs/read_workload
input_files=($(ls -1t ./read_workload_input*.txt | head -n $clients))
echo "${input_files[@]}"
popd

#  Run gnu-parallel
pushd ./src/abd-algo/cmake/build

parallel run_cpp_executable {} ::: "${input_files[@]}"
