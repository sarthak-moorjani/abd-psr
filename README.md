# ABD Shared Registers

The ABD shared registers algorithm for shared registers is a distributed algorithm used for reading and writing shared data in a shared register.

For handling reads, each node in the distributed system stores a value and a timestamp for that value. To read a value, the client sends a read request to all nodes and waits for quorum responses. Each node replies with its stored value and timestamp. The client then selects the most recent value and returns it as the result of the read operation.

For handling writes, the client sends a write request to all the nodes and waits for a quorum responses. Each node stores the new value with a timestamp that is greater than any previously seen timestamp for that node. Once the quorum has acknowledged the write, the write is considered complete.

The ABD algorithm can handle network partitions. When a network partition occurs, the nodes on either side of the partition can continue to operate independently, with each side having its own quorum. This ensures that the system can continue to function even in the presence of network failures.

The ABD quorum reads algorithm is commonly used in distributed systems to implement shared registers, which are a fundamental building block for many distributed algorithms.


Steps to run the ABD algorithm:
1. Set up your cloud lab cluster
2. Ensure grpc is setup correctly (https://grpc.io/docs/languages/cpp/quickstart/)
3. Clone the github repo
4. Change you current working directory to abd-psr/scripts
5. Run the following command:
```sh
$ ./multiple_clients.sh <CloudlabUsername> <List of Replica IPs>
```
```sh
Example
./multiple_clients.sh bob 10.10.1.1 10.10.1.2 10.10.1.3
```

The scripts takes as arguments  3 server IPs (exclude the IP of the server you are currently running the script on). The script starts the replicas (that are the shared registers storing the <key,value>) on the 3 servers passed as arguments. The client (serving the requests) is started on the replica the script is run on. The number of clients can be configured in the script!
