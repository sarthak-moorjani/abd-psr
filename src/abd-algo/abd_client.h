/*
 * Authors: sm106@illinois.edu
 * This file defines the client for the ABD algorithm.
 *
 */

#ifndef ABD_CLIENT_H
#define ABD_CLIENT_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "abd_algo.grpc.pb.h"

class ABDClient {
  private:
    std::unique_ptr<abd_algo::ABDImpl::Stub> stub_;

    // Since clients are closed loop, there would be only one outstanding
    // request at every point in time. Therefore, we can have a status
    // variable.
    grpc::Status status_;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context_;

    // Target IPs for the servers/replicas.
    std::vector<std::string> target_strs_;

  public:
    // Constructor.
    ABDClient(std::shared_ptr<grpc::Channel> channel,
              std::vector<std::string> target_strs =
                std::vector<std::string> ());

    // Read Interface for the algorithm.
    std::string Read(std::string key);

    // Phase 1 for the read protocol. This method returns a
    // pair of the chosen value and the largest timestamp corresponding to it.
    std::pair<std::string, int> ReadGetPhase(std::string key);

    // Phase 2 for the read protocol. This method just talks
    // to the replicas to set the current value. The value returned to the
    // client will always be 'v' as set by the GetPhase of the algorithm.
    void ReadSetPhase(std::string key, int max_ts);

    // Write Interface for the algorithm.
    void Write(std::string key, std::string value);

    // Phase 1 for the write protocol. This method returns a pair of the max
    // unique timestamp and the client id of the client writing the value.
    std::pair<std::string, int> WriteGetPhase(std::string key, std::string value);

    // Phase 2 for the write protocol. This method talks to the replicas to
    // write the most recent value.
    void WriteSetPhase(std::string key, std::string value, int max_ts);
};

#endif
