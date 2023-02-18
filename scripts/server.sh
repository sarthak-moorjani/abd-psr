rm -rf ../src/abd-algo/cmake/build
mkdir -p ../src/abd-algo/cmake/build
pushd ../src/abd-algo/cmake/build
cmake -DOPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl -DOPENSSL_LIBRARIES=/opt/homebrew/opt/openssl/lib -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
make -j 4
./abd_replica $1
