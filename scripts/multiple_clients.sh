#!/bin/bash

# install gnu-parallel
sudo apt-get update
sudo apt-get install parallel

# change working directory from scripts
pushd ../
echo $(pwd)
# rm -rf inputs
# mkdir inputs
# rm -rf outputs
# mkdir outputs


echo $(pwd)

rm -rf src/abd-algo/cmake/build
mkdir -p src/abd-algo/cmake/build
pushd src/abd-algo/cmake/build
cmake -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl -DOPENSSL_LIBRARIES=/opt/homebrew/opt/openssl/lib -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
make -j 4

popd

# make one client initialise 
# echo $(pwd)

# echo $(pwd)
# python3 ./benchmark/random_gen.py W

pushd src/abd-algo/cmake/build
./abd_client true input.txt input.txt
popd

# pushd inputs/

# echo $(pwd)

# # generate x clients workload
# # generate read_workload
# clients=10
# for i in $(eval echo {1..$clients})
# do 
#     python3 ./../benchmark/read_workload.py $i
#     echo "Client $i read workload generated"
# done

# popd
# echo $(pwd)


export SHELL=$(type -p bash)

run_cpp_executable() {
    input_file=$1
    output_file="${input_file%.*}_output.txt"
    echo "Processing ${input_file} -> ${output_file}"
    ./abd_client false  ${input_file} ${output_file}
}


export -f run_cpp_executable


echo $(pwd)
# List of read_workload nput files to process
pushd inputs/read_workload
input_files=$(ls ./read_workload_input*.txt)
popd

#  Run gnu-parallel
pushd ./src/abd-algo/cmake/build

parallel run_cpp_executable {} ::: "${input_files[@]}"
