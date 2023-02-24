/*
 * Authors: sm106@illinois.edu
 *
 */

#include "abd_client.h"

#include "abd_algo.grpc.pb.h"

#include <assert.h>
#include <fstream>
#include <sstream>

using abd_algo::ABDImpl;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using namespace std;

//-----------------------------------------------------------------------------
ABDClient::ABDClient(vector<string> target_strs){
    for(int i =0; i< target_strs.size();i++){
        std::shared_ptr<Channel> channel = grpc::CreateChannel(target_strs[i], grpc::InsecureChannelCredentials());
        // cout << "Created" << endl;
        channels_[target_strs[i]] = channel;
    }
}   

//-----------------------------------------------------------------------------
string ABDClient::Read(string key) {
  // Call the two phases of the read protocol and return to the user.
  // Add code below!
  char *err = "";
  std::pair<time_t, string> p = ReadGetPhase(key, err);
//   cout << "Read Get phase done" << endl;
//   cout << p.first << " " << p.second << endl;
  if(strcmp(err,"")==0){
    ReadSetPhase(key, p.second, p.first, err);
    if(strcmp(err,"")==0){
        return p.second;
    }
  }
  return "";
}


//-----------------------------------------------------------------------------
std::pair<time_t, string> ABDClient::ReadGetPhase(std::string key, char* err) {
  // This is phase 1 of the read protocol.
  // Add code below!

  // cout << "In Read Get" << endl;
  
  ABDClient::AsyncCall<abd_algo::ReadGetArg,abd_algo::ReadGetRet>* get_call =
    new ABDClient::AsyncCall<abd_algo::ReadGetArg,abd_algo::ReadGetRet>();
  abd_algo::ReadGetArg request;
  request.set_key(key);
  std::map<std::string,std::shared_ptr<Channel>> :: iterator it = channels_.begin();
  int counter = 0;
  for(it = channels_.begin();it!=channels_.end();it++){
    string replica = it->first;
    // cout << "Read get from " << replica << endl;
    std::shared_ptr<Channel> channel = it->second;

    const auto start_time = chrono::system_clock::now();
    const chrono::system_clock::time_point deadline =
      start_time + chrono::milliseconds(5000);

    get_call->stubs.emplace_back(ABDImpl::NewStub(channel));
    ClientContext* context = new ClientContext();
    context->set_deadline(deadline);
    get_call->contexts.emplace_back(context);

    get_call->statuses.emplace_back(new Status());
    abd_algo::ReadGetRet* reply = new abd_algo::ReadGetRet();
    get_call->replies.emplace_back(reply);
    int i = counter;
    get_call->rpcs.emplace_back(
      (get_call->stubs[i]->AsyncReadGet)(context, request, &(get_call)->cq));
    get_call->rpcs[i]->Finish(reply, get_call->statuses[i], (void*)i);
    counter++;
  }

  std::atomic<int> num_rpcs_finished(0);
  std::atomic<int> num_rpcs_finished_ok(0);
  int majority = channels_.size()/2 + 1;
  bool timeOut = false;
  std::vector<std::pair<time_t,string>>timestamps;
  std::pair<time_t, string> max_ts;
  while (num_rpcs_finished < majority) {
    void* which_backend_ptr;
    bool ok = false;
    // Block until the next result is available in the completion queue "cq".
    get_call->cq.Next(&which_backend_ptr, &ok);
    num_rpcs_finished++;
    const size_t which_backend = size_t(which_backend_ptr);
    const Status& status = *(get_call->statuses[which_backend]);

    if (status.ok()) {
      // cout << "rpc ok" << endl;
      timestamps.emplace_back(make_pair(
        get_call->replies[num_rpcs_finished_ok]->timestamp(),
        get_call->replies[num_rpcs_finished_ok]->val()));
      // cout << "At read get ts = " << get_call->replies[num_rpcs_finished_ok]->timestamp() << "for key " << key << endl;
      num_rpcs_finished_ok++;
    } else {
        if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
          // cout << "rpc timed out" << endl;
        } else if(status.error_code() == grpc::StatusCode::NOT_FOUND) {
          cout << "read get phase - key not found. " << key << endl;
          err = "KeyNotFound";
          return max_ts;
        }
      }
    }
    if(timeOut){
        err = "Timeout Occurred";
        return max_ts;
    }
    // cout << get_call->stubs.size() << " rpcs attempted, " << num_rpcs_finished_ok << "/" << num_rpcs_finished << " rpcs finished ok" << endl;
    auto comparator = [&](pair<time_t,string> p1, pair<time_t,string> p2){
        return p1.first < p2.first;
    };
    max_ts = *max_element(timestamps.begin(),timestamps.end(), comparator);
    // cout << max_ts.first << endl;
    // cout << "Read Get Done." << endl;
    return max_ts;
}


//-----------------------------------------------------------------------------

void ABDClient::ReadSetPhase(std::string key, std::string value, int max_ts, char* err) {
  // This is phase 2 of the read protocol.
  // Add code below!
  cout << "In Read Set" << endl;
  ABDClient::AsyncCall<abd_algo::ReadSetArg,abd_algo::ReadSetRet> * set_call =
    new ABDClient::AsyncCall<abd_algo::ReadSetArg,abd_algo::ReadSetRet>();
  abd_algo::ReadSetArg request;

  request.set_key(key);
  request.set_val(value);
  request.set_timestamp(max_ts);
  std::map<std::string,std::shared_ptr<Channel>> :: iterator it = channels_.begin();
  int counter = 0;
  for (it = channels_.begin();it!=channels_.end();it++){
    // cout << "Writing get from " << replica << endl;
    std::shared_ptr<Channel> channel = it->second;
    const auto start_time = chrono::system_clock::now();
    const chrono::system_clock::time_point deadline = start_time + chrono::milliseconds(5000);

    set_call->stubs.emplace_back(ABDImpl::NewStub(channel));
    ClientContext* context = new ClientContext();
    context->set_deadline(deadline);
    set_call->contexts.emplace_back(context);

    set_call->statuses.emplace_back(new Status());
    abd_algo::ReadSetRet* reply = new abd_algo::ReadSetRet();
    set_call->replies.emplace_back(reply);

    int i = counter;
    set_call->rpcs.emplace_back((set_call->stubs[i]->AsyncReadSet)(context, request, &(set_call)->cq));
    set_call->rpcs[i]->Finish(reply, set_call->statuses[i], (void*)i);
    counter++;
  }

  std::atomic<int> num_rpcs_finished(0);
  std::atomic<int> num_rpcs_finished_ok(0);
  int majority = channels_.size()/2 + 1;
  bool newKey = false;
  bool timeOut = false;
  while (num_rpcs_finished < majority) {
    void* which_backend_ptr;
    bool ok = false;
    // Block until the next result is available in the completion queue "cq".
    set_call->cq.Next(&which_backend_ptr, &ok);
    num_rpcs_finished++;
    const size_t which_backend = size_t(which_backend_ptr);
    const Status& status = *(set_call->statuses[which_backend]);
    if (status.ok()) {
      num_rpcs_finished_ok++;
    } else {
      if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
      // cout << "rpc timed out" << endl;
      timeOut = true;
    } else if(status.error_code() == grpc::StatusCode::NOT_FOUND) {
        // cout << "key not found read set phase" << key << endl;
      }
    }
  }
  // cout << "Read Set Done." << endl;
}

//-----------------------------------------------------------------------------

void ABDClient::Write(std::string key, std::string value) {
  // Call the two phases of the write protocol here.
  // Add code below!
  pair<time_t,int> p = WriteGetPhase(key, value);
  WriteSetPhase(key, value, p.first);
}

//-----------------------------------------------------------------------------
std::pair<time_t, int> ABDClient::WriteGetPhase(std::string key,
                                                std::string value) {
  // This is the get phase of the write protocol.
  // cout << "In Write Get " << endl;
  ABDClient::AsyncCall<abd_algo::WriteGetArg,abd_algo::WriteGetRet>* get_call =
    new ABDClient::AsyncCall<abd_algo::WriteGetArg,abd_algo::WriteGetRet>();
  abd_algo::WriteGetArg request;
  request.set_key(key);
  request.set_val(value);
  std::map<std::string,std::shared_ptr<Channel>> :: iterator it =
    channels_.begin();

  int counter = 0;
  for (it = channels_.begin(); it!=channels_.end();it++){
    // cout << "Writing get from " << replica << endl;
    std::shared_ptr<Channel> channel = it->second;

    const auto start_time = chrono::system_clock::now();
    const chrono::system_clock::time_point deadline =
      start_time + chrono::milliseconds(5000);

    get_call->stubs.emplace_back(ABDImpl::NewStub(channel));
    ClientContext* context = new ClientContext();
    context->set_deadline(deadline);
    get_call->contexts.emplace_back(context);
    get_call->statuses.emplace_back(new Status());

    abd_algo::WriteGetRet* reply = new abd_algo::WriteGetRet();
    get_call->replies.emplace_back(reply);

    int i = counter;
    get_call->rpcs.emplace_back(
      (get_call->stubs[i]->AsyncWriteGet)(context, request, &(get_call)->cq));
    get_call->rpcs[i]->Finish(reply, get_call->statuses[i], (void*)i);
    counter++;
  }

    int num_rpcs_finished = 0;
    int num_rpcs_finished_ok = 0;
    int majority = channels_.size()/2 + 1;
    std::vector<std::pair<time_t,int>>timestamps;
    while (num_rpcs_finished < majority) {
        // cout << "RPC Finished:Majority" << num_rpcs_finished << " " << majority << endl;
        void* which_backend_ptr;
        bool ok = false;
        // Block until the next result is available in the completion queue "cq".
        get_call->cq.Next(&which_backend_ptr, &ok);
        num_rpcs_finished++;
        const size_t which_backend = size_t(which_backend_ptr);
        const Status& status = *(get_call->statuses[which_backend]);

        if (status.ok()) {
            // cout << "rpc ok" << endl;
            timestamps.emplace_back(make_pair(get_call->replies[num_rpcs_finished_ok]->timestamp(),get_call->replies[num_rpcs_finished_ok]->client_id()));
            // cout << "Timestamp: " << get_call->replies[num_rpcs_finished_ok]->timestamp() << endl;
            num_rpcs_finished_ok++;
        } else {
            if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
                // cout << "write get rpc timed out" << endl;
            } else if(status.error_code() == grpc::StatusCode::NOT_FOUND) {
                // cout << "write get phase key not found. Inserting " <<  key << endl;
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
        max_ts.first += 1;
        // cout << "New timestamp value for key: " << key << " value: " << value << " Timestamp: " << max_ts.first <<  endl;
    }
    // cout << max_ts.first << endl;
    // cout << "Write Get Done." << endl;
    return max_ts;
}

//-----------------------------------------------------------------------------

void ABDClient::WriteSetPhase(std::string key, std::string value, int max_ts) {
  // This is the set phase of the write protocol.
  // Add code below!
  // cout << "In Write Set" << endl;
  ABDClient::AsyncCall<abd_algo::WriteSetArg,abd_algo::WriteSetRet>* set_call =
    new ABDClient::AsyncCall<abd_algo::WriteSetArg,abd_algo::WriteSetRet>();
  abd_algo::WriteSetArg request;

  request.set_key(key);
  request.set_val(value);
  // cout << "max ts in write set " << max_ts << "key " << key << "value " << value << endl;
  request.set_timestamp(max_ts);
  std::map<std::string,std::shared_ptr<Channel>> :: iterator it = channels_.begin();

  int counter = 0;
  for(it = channels_.begin();it!=channels_.end();it++){
    // cout << "Writing get from " << replica << endl;
    std::shared_ptr<Channel> channel = it->second;

    const auto start_time = chrono::system_clock::now();
    const chrono::system_clock::time_point deadline = start_time + chrono::milliseconds(5000);

    set_call->stubs.emplace_back(ABDImpl::NewStub(channel));
    ClientContext* context = new ClientContext();
    context->set_deadline(deadline);
    set_call->contexts.emplace_back(context);

    set_call->statuses.emplace_back(new Status());
    abd_algo::WriteSetRet* reply = new abd_algo::WriteSetRet(); 
    set_call->replies.emplace_back(reply);
    int i = counter;
    set_call->rpcs.emplace_back((
      set_call->stubs[i]->AsyncWriteSet)(context, request, &(set_call)->cq));
    set_call->rpcs[i]->Finish(reply, set_call->statuses[i], (void*)i);
    counter++;
  }

    int num_rpcs_finished = 0;
    int num_rpcs_finished_ok = 0;
    bool newKey = false;
    int majority = this->channels_.size()/2 + 1;

    while (num_rpcs_finished < majority) {
        void* which_backend_ptr;
        bool ok = false;
        // Block until the next result is available in the completion queue "cq".
        set_call->cq.Next(&which_backend_ptr, &ok);
        num_rpcs_finished++;
        const size_t which_backend = size_t(which_backend_ptr);
        const Status& status = *(set_call->statuses[which_backend]);
        // cout << "rpc #" << which_backend << " done after " << elapsed_ms(start_time) << "ms" << endl;
        if (status.ok()) {
            // cout << "rpc ok" << endl;
            num_rpcs_finished_ok++;
        } else {
            if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
                cout << "write set rpc timed out" << endl;
            } else if(status.error_code() == grpc::StatusCode::NOT_FOUND) {
                cout << "write get phase key not found. Inserting " << key <<  endl;
            }
        }
    }
    // cout << "Write Set Done." << endl;
    // cout << set_call->stubs.size() << " rpcs attempted, " << num_rpcs_finished_ok << "/" << num_rpcs_finished << " rpcs finished ok" << endl;
}

bool initialise(ABDClient abd_client) {

  vector<string> operations, keys, values;
  string line;
  cout << "Initialising the store with 1M values.." <<endl;
  ifstream myfile("./../../../../inputs/input.txt");
  if (myfile.is_open())
  {
    while (getline(myfile, line)) {
      istringstream ss(line);
      string word;
      while (ss >> word) {
        if (word.size() == 3) {
          operations.push_back(word);
        } else if (word.size() == 24) {
          keys.push_back(word);
        } else {
          values.push_back(word);
        }
      }
    }
    myfile.close();
  } else {
    cout << "cannot find file, run the random_gen file in benchmark directory!" << endl;
    return false;
  }

  int iter = 0;
  for (auto operation: operations) {
    if(strcmp(operation.c_str(),"get")==0){
        std::string val = abd_client.Read(keys[iter]);
        cout << "Value " <<  val << endl;
    }else if(strcmp(operation.c_str(),"put")==0){
        abd_client.Write(keys[iter], values[iter]);
    }
    iter++;
  }

  cout << "Checking what we wrote is written" << endl;
  for (int i = 0; i < keys.size(); i++) {
    std::string val = abd_client.Read(keys[i]);
    cout << "Value " <<  val << endl;
    assert(val == values[i]);
  }
  cout << "Initialisation done!" <<endl;
  return true;
}

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  bool is_initialise = true;

  if (strcmp(argv[1], "false") == 0) {
    cout << "initialise set to false" << endl;
    is_initialise = false;
  }
  std::string workload_input_file = argv[2];
  std::string workload_output_file = argv[3];
  ABDClient abd_client({"10.10.1.1:50052", "10.10.1.2:50052", "10.10.1.3:50052"});
  // std::string arg_str("--target");
  
  // if (argc > 1) {
  //   std::string arg_val = argv[1];
  //   size_t start_pos = arg_val.find(arg_str);
  //   if (start_pos != std::string::npos) {
  //     start_pos += arg_str.size();
  //     if (arg_val[start_pos] == '=') {
  //       target_str = arg_val.substr(start_pos + 1);
  //     } else {
  //       std::cout << "The only correct argument syntax is --target="
  //                 << std::endl;
  //       return 0;
  //     }
  //   } else {
  //     std::cout << "The only acceptable argument is --target=" << std::endl;
  //     return 0;
  //   }
  // } else {
  //   target_str = "localhost:50051";
  // }

  if(is_initialise){
    if(!initialise(abd_client))
      cout << "Initialization failed" << endl;
    return 0;
  }
  string workload_input_filename  = "/users/sarthakm/abd-psr/inputs/" +  workload_input_file;
   string workload_output_filename  = "/users/sarthakm/abd-psr/outputs/" +  workload_output_file;
  ifstream myfile(workload_input_filename);
  vector<string> operations, keys, values;
  string line;
  if (myfile.is_open())
  {
    while (getline(myfile, line)) {
      istringstream ss(line);
      string word;
      while (ss >> word) {
        if (word.size() == 3) {
          operations.push_back(word);
        } else if (word.size() == 24) {
          keys.push_back(word);
        } else {
          values.push_back(word);
        }
      }
    }
    myfile.close();
  } else {
    cout << "cannot find " + workload_input_filename + " file, run the random_gen file in benchmark directory!" << endl;
  }

    int iter = 0;
  for (auto operation: operations) {
    if(strcmp(operation.c_str(),"get")==0){
        std::string val = abd_client.Read(keys[iter]);
        if (val.empty()) {
          cout << "Key not found" << endl;
          continue;
        }
        cout << argv[1] << " Value " <<  val << endl;
    }else if(strcmp(operation.c_str(),"put")==0){
        abd_client.Write(keys[iter], values[iter]);
    }
    iter++;
  }
  // for read-workload

  // Check values
  return 0;
}
