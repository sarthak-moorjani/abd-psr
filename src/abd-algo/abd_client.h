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

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::CompletionQueue;
using grpc::Service;
using grpc::ClientAsyncResponseReader;

using namespace std;

class ABDClient {
  private:
    
    // std::unique_ptr<abd_algo::ABDImpl::Stub> stub_;

    // Since clients are closed loop, there would be only one outstanding
    // request at every point in time. Therefore, we can have a status
    // variable.
    // grpc::Status status_;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    // grpc::ClientContext context_;

    // Target IPs for the servers/replicas.
    std::map<std::string,std::shared_ptr<Channel>> channels_;

  public:
    template<typename InType, typename OutType>
    class AsyncCall {
        public:
          vector<std::unique_ptr<abd_algo::ABDImpl::Stub>> stubs;
          CompletionQueue cq;
          vector<std::unique_ptr<ClientContext>> contexts;
          vector<std::unique_ptr<Status>> statuses;
          vector<std::unique_ptr<OutType>> replies;
          vector<std::unique_ptr<ClientAsyncResponseReader<OutType>>> rpcs;
    };
  
    // Constructor.
    ABDClient(std::vector<std::string> target_strs =
                std::vector<std::string> ());
    
    // Read Interface for the algorithm.
    std::string Read(std::string key);

    // Phase 1 for the read protocol. This method returns a
    // pair of the chosen value and the largest timestamp corresponding to it.
    std::pair<time_t, string> ReadGetPhase(std::string key, char*err);

    // Phase 2 for the read protocol. This method just talks
    // to the replicas to set the current value. The value returned to the
    // client will always be 'v' as set by the GetPhase of the algorithm.
    void ReadSetPhase(std::string key, std::string value, int max_ts, char *err);

    // Write Interface for the algorithm.
    void Write(std::string key, std::string value);

    // Phase 1 for the write protocol. This method returns a pair of the max
    // unique timestamp and the client id of the client writing the value.
    std::pair<time_t, int> WriteGetPhase(std::string key, std::string value);

    // Phase 2 for the write protocol. This method talks to the replicas to
    // write the most recent value.
    void WriteSetPhase(std::string key, std::string value, int max_ts);
};

#endif
