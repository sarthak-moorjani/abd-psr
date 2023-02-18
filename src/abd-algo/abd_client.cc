/*
 * Authors: sm106@illinois.edu
 *
 */

#include "abd_client.h"

#include "abd_algo.grpc.pb.h"

// temp
using abd_algo::ABDImpl;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using namespace std;

//-----------------------------------------------------------------------------
ABDClient::ABDClient(vector<string> target_strs){
    target_strs_ = target_strs;
}

//-----------------------------------------------------------------------------
string ABDClient::Read(string key) {
  // Call the two phases of the read protocol and return to the user.
  // Add code below!
  std::pair<time_t, string> p = ReadGetPhase(key);
  ReadSetPhase(key, p.second, p.first);
  return p.second;
}


//-----------------------------------------------------------------------------
std::pair<time_t, string> ABDClient::ReadGetPhase(std::string key) {
  // This is phase 1 of the read protocol.
  // Add code below!
  
  ABDClient::AsyncCall<abd_algo::ReadGetArg,abd_algo::ReadGetRet> * get_call = new ABDClient::AsyncCall<abd_algo::ReadGetArg,abd_algo::ReadGetRet>();
  abd_algo::ReadGetArg request;
  request.set_key(key);
  for(size_t i = 0; i < this->target_strs_.size(); i++){
    string replica = this->target_strs_[i];
    // cout << "Read get from " << replica << endl;
    std::shared_ptr<Channel> channel = grpc::CreateChannel(replica, grpc::InsecureChannelCredentials());

    const auto start_time = chrono::system_clock::now();
    const chrono::system_clock::time_point deadline = start_time + chrono::milliseconds(5000);

    get_call->stubs.emplace_back(ABDImpl::NewStub(channel));
    ClientContext* context = new ClientContext();
    context->set_deadline(deadline);
    get_call->contexts.emplace_back(context);

    get_call->statuses.emplace_back(new Status());                              
    abd_algo::ReadGetRet* reply = new abd_algo::ReadGetRet();                                        
    get_call->replies.emplace_back(reply);

    get_call->rpcs.emplace_back((get_call->stubs[i]->AsyncReadGet)(context, request, &(get_call)->cq));                                                          
    get_call->rpcs[i]->Finish(reply, get_call->statuses[i].get(), (void*)i);                                                         
  }

    int num_rpcs_finished = 0;
    int num_rpcs_finished_ok = 0;
    int majority = this->target_strs_.size()/2 + 1;
    std::vector<std::pair<time_t,string>>timestamps;
    while (num_rpcs_finished < majority) {
        void* which_backend_ptr;
        bool ok = false;
        // Block until the next result is available in the completion queue "cq".
        get_call->cq.Next(&which_backend_ptr, &ok);
        num_rpcs_finished++;
        const size_t which_backend = size_t(which_backend_ptr);
        const Status& status = *(get_call->statuses[which_backend].get());

        if (status.ok()) {
            // cout << "rpc ok" << endl;
            timestamps.emplace_back(make_pair(get_call->replies[num_rpcs_finished_ok]->timestamp(),get_call->replies[num_rpcs_finished_ok]->val()));
            num_rpcs_finished_ok++;
        } else {
            if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
                cout << "rpc timed out" << endl;
            } else if(status.error_code() == grpc::StatusCode::NOT_FOUND) {
                cout << "key not found. Inserting " << endl;
            }
        }
    }

    std::pair<time_t, string> max_ts;
    if(num_rpcs_finished_ok!=majority){
        max_ts = make_pair(time(0),0);
    }else{
        // cout << get_call->stubs.size() << " rpcs attempted, " << num_rpcs_finished_ok << "/" << num_rpcs_finished << " rpcs finished ok" << endl;
        auto comparator = [&](pair<time_t,string> p1, pair<time_t,string> p2){
            return p1.first < p2.first;
        };
        max_ts = *max_element(timestamps.begin(),timestamps.end(), comparator);
    }
    // cout << max_ts.first << endl;
    return max_ts;
}


//-----------------------------------------------------------------------------
void ABDClient::ReadSetPhase(std::string key, std::string value, int max_ts) {
  // This is phase 2 of the read protocol.
  // Add code below!
//   cout << "Read Set" << endl;
    ABDClient::AsyncCall<abd_algo::ReadSetArg,abd_algo::ReadSetRet> * set_call = new ABDClient::AsyncCall<abd_algo::ReadSetArg,abd_algo::ReadSetRet>();
    abd_algo::ReadSetArg request;
    
    request.set_key(key);
    request.set_val(value);
    request.set_timestamp(max_ts);
    for(size_t i = 0; i < this->target_strs_.size(); i++){
        string replica = this->target_strs_[i];
        // cout << "Writing get from " << replica << endl;
        std::shared_ptr<Channel> channel = grpc::CreateChannel(replica, grpc::InsecureChannelCredentials());

        const auto start_time = chrono::system_clock::now();
        const chrono::system_clock::time_point deadline = start_time + chrono::milliseconds(5000);

        set_call->stubs.emplace_back(ABDImpl::NewStub(channel));
        ClientContext* context = new ClientContext();
        context->set_deadline(deadline);
        set_call->contexts.emplace_back(context);

        set_call->statuses.emplace_back(new Status());                              
        abd_algo::ReadSetRet* reply = new abd_algo::ReadSetRet();                                        
        set_call->replies.emplace_back(reply);

        set_call->rpcs.emplace_back((set_call->stubs[i]->AsyncReadSet)(context, request, &(set_call)->cq));                                                          
        set_call->rpcs[i]->Finish(reply, set_call->statuses[i].get(), (void*)i);                                                         
    }

    int num_rpcs_finished = 0;
    int num_rpcs_finished_ok = 0;
    int majority = this->target_strs_.size()/2 + 1;
    bool newKey = false;
    while (num_rpcs_finished < majority) {
        void* which_backend_ptr;
        bool ok = false;
        // Block until the next result is available in the completion queue "cq".
        set_call->cq.Next(&which_backend_ptr, &ok);
        num_rpcs_finished++;
        const size_t which_backend = size_t(which_backend_ptr);
        const Status& status = *(set_call->statuses[which_backend].get());
        // cout << "rpc #" << which_backend << " done after " << elapsed_ms(start_time) << "ms" << endl;
        if (status.ok()) {
            // cout << "rpc ok" << endl;
            num_rpcs_finished_ok++;
        } else {
            if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
                cout << "rpc timed out" << endl;
            } else if(status.error_code() == grpc::StatusCode::NOT_FOUND) {
                cout << "key not found." << endl;
            }
        }
    }
    // cout << set_call->stubs.size() << " rpcs attempted, " << num_rpcs_finished_ok << "/" << num_rpcs_finished << " rpcs finished ok" << endl;
}

//-----------------------------------------------------------------------------
void ABDClient::Write(std::string key, std::string value) {
  // Call the two phases of the write protocol here.
  // Add code below!
  pair<time_t,int> p = WriteGetPhase(key, value);
  WriteSetPhase(key, value, p.first);
}   
//-----------------------------------------------------------------------------
std::pair<time_t, int> ABDClient::WriteGetPhase(std::string key,std::string value) {
  // This is the get phase of the write protocol.
  // Add code below!
  ABDClient::AsyncCall<abd_algo::WriteGetArg,abd_algo::WriteGetRet> * get_call = new ABDClient::AsyncCall<abd_algo::WriteGetArg,abd_algo::WriteGetRet>();
  abd_algo::WriteGetArg request;
  request.set_key(key);
  request.set_val(value);
  for(size_t i = 0; i < this->target_strs_.size(); i++){
    string replica = this->target_strs_[i];
    // cout << "Writing get from " << replica << endl;
    std::shared_ptr<Channel> channel = grpc::CreateChannel(replica, grpc::InsecureChannelCredentials());

    const auto start_time = chrono::system_clock::now();
    const chrono::system_clock::time_point deadline = start_time + chrono::milliseconds(5000);

    get_call->stubs.emplace_back(ABDImpl::NewStub(channel));
    ClientContext* context = new ClientContext();
    context->set_deadline(deadline);
    get_call->contexts.emplace_back(context);

    get_call->statuses.emplace_back(new Status());                              
    abd_algo::WriteGetRet* reply = new abd_algo::WriteGetRet();                                        
    get_call->replies.emplace_back(reply);

    get_call->rpcs.emplace_back((get_call->stubs[i]->AsyncWriteGet)(context, request, &(get_call)->cq));                                                          
    get_call->rpcs[i]->Finish(reply, get_call->statuses[i].get(), (void*)i);                                                         
  }

    int num_rpcs_finished = 0;
    int num_rpcs_finished_ok = 0;
    int majority = this->target_strs_.size()/2 + 1;
    std::vector<std::pair<time_t,int>>timestamps;
    while (num_rpcs_finished < majority) {
        // cout << "RPC Finished:Majority" << num_rpcs_finished << " " << majority << endl;
        void* which_backend_ptr;
        bool ok = false;
        // Block until the next result is available in the completion queue "cq".
        get_call->cq.Next(&which_backend_ptr, &ok);
        num_rpcs_finished++;
        const size_t which_backend = size_t(which_backend_ptr);
        const Status& status = *(get_call->statuses[which_backend].get());

        if (status.ok()) {
            // cout << "rpc ok" << endl;
            timestamps.emplace_back(make_pair(get_call->replies[num_rpcs_finished_ok]->timestamp(),get_call->replies[num_rpcs_finished_ok]->client_id()));
            num_rpcs_finished_ok++;
        } else {
            if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
                cout << "rpc timed out" << endl;
            } else if(status.error_code() == grpc::StatusCode::NOT_FOUND) {
                cout << "key not found. Inserting " << endl;
            }
        }
    }

    std::pair<time_t, int> max_ts;
    if(num_rpcs_finished_ok!=majority){
        max_ts = make_pair(time(0),0);
    }else{
        // cout << get_call->stubs.size() << " rpcs attempted, " << num_rpcs_finished_ok << "/" << num_rpcs_finished << " rpcs finished ok" << endl;
        auto comparator = [&](pair<time_t,int> p1, pair<time_t,int> p2){
            return p1.first < p2.first or (p1.first==p2.first and p1.second < p2.second);
        };
        max_ts = *max_element(timestamps.begin(),timestamps.end(), comparator);
    }
    // cout << max_ts.first << endl;
    return max_ts;
}

//-----------------------------------------------------------------------------

void ABDClient::WriteSetPhase(std::string key, std::string value, int max_ts) {
  // This is the set phase of the write protocol.
  // Add code below!
//   cout << "set" << endl;
    ABDClient::AsyncCall<abd_algo::WriteSetArg,abd_algo::WriteSetRet> * set_call = new ABDClient::AsyncCall<abd_algo::WriteSetArg,abd_algo::WriteSetRet>();
    abd_algo::WriteSetArg request;
    
    request.set_key(key);
    request.set_val(value);
    request.set_timestamp(max_ts);
    for(size_t i = 0; i < this->target_strs_.size(); i++){
        string replica = this->target_strs_[i];
        // cout << "Writing get from " << replica << endl;
        std::shared_ptr<Channel> channel = grpc::CreateChannel(replica, grpc::InsecureChannelCredentials());

        const auto start_time = chrono::system_clock::now();
        const chrono::system_clock::time_point deadline = start_time + chrono::milliseconds(5000);

        set_call->stubs.emplace_back(ABDImpl::NewStub(channel));
        ClientContext* context = new ClientContext();
        context->set_deadline(deadline);
        set_call->contexts.emplace_back(context);

        set_call->statuses.emplace_back(new Status());                              
        abd_algo::WriteSetRet* reply = new abd_algo::WriteSetRet();                                        
        set_call->replies.emplace_back(reply);

        set_call->rpcs.emplace_back((set_call->stubs[i]->AsyncWriteSet)(context, request, &(set_call)->cq));                                                          
        set_call->rpcs[i]->Finish(reply, set_call->statuses[i].get(), (void*)i);                                                         
    }

    int num_rpcs_finished = 0;
    int num_rpcs_finished_ok = 0;
    bool newKey = false;
    while (num_rpcs_finished < set_call->stubs.size()) {
        void* which_backend_ptr;
        bool ok = false;
        // Block until the next result is available in the completion queue "cq".
        set_call->cq.Next(&which_backend_ptr, &ok);
        num_rpcs_finished++;
        const size_t which_backend = size_t(which_backend_ptr);
        const Status& status = *(set_call->statuses[which_backend].get());
        // cout << "rpc #" << which_backend << " done after " << elapsed_ms(start_time) << "ms" << endl;
        if (status.ok()) {
            // cout << "rpc ok" << endl;
            num_rpcs_finished_ok++;
        } else {
            if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
                cout << "rpc timed out" << endl;
            } else if(status.error_code() == grpc::StatusCode::NOT_FOUND) {
                cout << "key not found. Inserting" << endl;
            }
        }
    }
    // cout << set_call->stubs.size() << " rpcs attempted, " << num_rpcs_finished_ok << "/" << num_rpcs_finished << " rpcs finished ok" << endl;
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
  ABDClient abd_client({"10.10.1.1:50052","10.10.1.2:50052","10.10.1.3:50052"});
  abd_client.Write("2","2");
  cout << "Read " << abd_client.Read("2") << endl;
  abd_client.Write("2","8");
  abd_client.Write("1","8");
  cout<< "Read " << abd_client.Read("1") << endl;
  cout << "Read " << abd_client.Read("2") << endl;
  cout << "Read " << abd_client.Read("6") << endl;
  return 0;
}
