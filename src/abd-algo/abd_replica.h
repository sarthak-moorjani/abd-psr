/*
 * Authors: sm106@illinois.edu
 * This file defines the replica for the ABD algorithm.
 *
 */

#ifndef ABD_REPLICA_H
#define ABD_REPLICA_H

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "helloworld.grpc.pb.h"

class ABDReplica final : public helloworld::Greeter::Service {
  public:
    // The RPC methods definition will come here.
    grpc::Status SayHello(grpc::ServerContext* context,
                          const helloworld::HelloRequest* request,
                          helloworld::HelloReply* reply) override;
};

#endif
