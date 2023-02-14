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

#include "abd_algo.grpc.pb.h"

class ABDReplica final : public abd_algo::ABDImpl::Service {
  public:
    // The RPC methods definition will come here.
    grpc::Status ReadGet(grpc::ServerContext* context,
                         const abd_algo::ReadGetArg* request,
                         abd_algo::ReadGetRet* reply) override;
};

#endif
