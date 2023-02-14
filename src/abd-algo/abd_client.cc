/*
 * Authors: sm106@illinois.edu
 *
 */

#include "abd_client.h"

#include "helloworld.grpc.pb.h"

// temp
using helloworld::Greeter;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using namespace std;

//-----------------------------------------------------------------------------

ABDClient::ABDClient(std::shared_ptr<Channel> channel,
                         vector<string> target_strs)
                         : stub_(Greeter::NewStub(channel)) {
}

//-----------------------------------------------------------------------------

string ABDClient::Read(string key) {
  // Call the two phases of the read protocol and return to the user.
  // Add code below!
}

//-----------------------------------------------------------------------------

std::pair<std::string, int> ABDClient::ReadGetPhase(std::string key) {
  // This is phase 1 of the read protocol.
  // Add code below!
}

//-----------------------------------------------------------------------------

void ABDClient::ReadSetPhase(std::string key, int max_ts) {
  // This is phase 2 of the read protocol.
  // Add code below!
}

//-----------------------------------------------------------------------------

void ABDClient::Write(std::string key, std::string value) {
  // Call the two phases of the write protocol here.
  // Add code below!
}

//-----------------------------------------------------------------------------

std::pair<std::string, int> ABDClient::WriteGetPhase(std::string key,
                                                     std::string value) {
  // This is the get phase of the write protocol.
  // Add code below!
}

//-----------------------------------------------------------------------------

void ABDClient::WriteSetPhase(std::string key, std::string value, int max_ts) {
  // This is the set phase of the write protocol.
  // Add code below!
}

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  std::string target_str;
  std::string arg_str("--target");
  if (argc > 1) {
    std::string arg_val = argv[1];
    size_t start_pos = arg_val.find(arg_str);
    if (start_pos != std::string::npos) {
      start_pos += arg_str.size();
      if (arg_val[start_pos] == '=') {
        target_str = arg_val.substr(start_pos + 1);
      } else {
        std::cout << "The only correct argument syntax is --target="
                  << std::endl;
        return 0;
      }
    } else {
      std::cout << "The only acceptable argument is --target=" << std::endl;
      return 0;
    }
  } else {
    target_str = "localhost:50051";
  }
  ABDClient greeter(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  return 0;
}
