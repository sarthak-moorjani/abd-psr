#!/bin/bash

# install gnu-parallel
# apt-get update
# apt-get install gnu-parallel

# change working directory from scripts
pushd ../
echo $(pwd)
rm -rf inputs
mkdir inputs
rm -rf outputs
mkdir outputs


# make one client initialise 
echo $(pwd)

echo $(pwd)
python3 benchmark/random_gen.py W
# ./abd_client true input.txt input.txt

pushd inputs/

# generate x clients workload
# generate read_workload
clients=5
for i in $(eval echo {1..$clients})
do 
    python3 ../benchmark/read_workload.py $i
    echo "Client $i read workload generated"
done

popd
popd
echo $(pwd)

rm -rf ../src/abd-algo/cmake/build
mkdir -p ../src/abd-algo/cmake/build
pushd ../src/abd-algo/cmake/build
cmake -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl -DOPENSSL_LIBRARIES=/opt/homebrew/opt/openssl/lib -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
make -j 4

export SHELL=$(type -p bash)

run_cpp_executable() {
    input_file=$1
    output_file="${input_file%.*}_output.txt"
    echo "Processing ${input_file} -> ${output_file}"
    ./abd_client false  ${input_file} ${output_file}
}


export -f run_cpp_executable



# List of read_workload nput files to process
input_files=$(ls read_workload_input*.txt)

#  Run gnu-parallel

parallel run_cpp_executable {} ::: "${input_files[@]}"