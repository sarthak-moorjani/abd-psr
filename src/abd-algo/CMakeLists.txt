# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ helloworld example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building helloworld.

cmake_minimum_required(VERSION 3.5.1)

project(ABD-ALGO C CXX)

include(../cmake/common.cmake)

# Proto file
get_filename_component(abd_proto "../protos/abd_algo.proto" ABSOLUTE)
get_filename_component(abd_proto_path "${abd_proto}" PATH)

# Generated sources
set(abd_algo_srcs "${CMAKE_CURRENT_BINARY_DIR}/abd_algo.pb.cc")
set(abd_algo_hdrs "${CMAKE_CURRENT_BINARY_DIR}/abd_algo.pb.h")
set(abd_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/abd_algo.grpc.pb.cc")
set(abd_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/abd_algo.grpc.pb.h")

add_custom_command(
      OUTPUT "${abd_algo_srcs}" "${abd_algo_hdrs}" "${abd_grpc_srcs}" "${abd_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${abd_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${abd_proto}"
      DEPENDS "${abd_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# hw_grpc_proto
add_library(abd_algo_proto
  ${abd_algo_srcs}
  ${abd_algo_hdrs}
  ${abd_grpc_srcs}
  ${abd_grpc_hdrs})
target_link_libraries(abd_algo_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Targets greeter_[async_](client|server)
foreach(_target
  abd_client abd_replica)
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(${_target}
    abd_algo_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()
