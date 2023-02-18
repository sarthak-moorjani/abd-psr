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

struct shared_register{
  std::string value;
  time_t ts;
};

class ABDReplica final : public abd_algo::ABDImpl::Service {
  public:
    // The RPC methods definition will come here.
    grpc::Status ReadGet(grpc::ServerContext* context,
                         const abd_algo::ReadGetArg* request,
                         abd_algo::ReadGetRet* reply) override;
    // The RPC methods definition will come here.
    grpc::Status ReadSet(grpc::ServerContext* context,
                         const abd_algo::ReadSetArg* request,
                         abd_algo::ReadSetRet* reply) override;
    // The RPC methods definition will come here.
    grpc::Status WriteGet(grpc::ServerContext* context,
                         const abd_algo::WriteGetArg* request,
                         abd_algo::WriteGetRet* reply) override;
    // The RPC methods definition will come here.
    grpc::Status WriteSet(grpc::ServerContext* context,
                         const abd_algo::WriteSetArg* request,
                         abd_algo::WriteSetRet* reply) override;
                  
};

#endif
