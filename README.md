# Botnet Implementation
A botnet is a system of a host server and multiple client servers, in which the host server is able to send commands to clients. Infect client computers connect with server and clients through desired communication protocol, and use hacked devices’ compute power for tasks unknown to device owners. In this project, we implemented botnet by writing the functionalities of a command script using transmission control protocol (TCP).

## Prerequisites 
* cmake: Used to compile and build c files and requires a CMakeLists.txt file
* make: compiles from source files and a Makefile
* gcc: Alternative to cmake
* Clang-tidy: Linting
* Git: Access GitHut repo
* Criterion: For testing

## Usage
```
git clone git@github.com:olincollege/botnet_c.git

# Run server
cd ./server
./build_server.sh
./server

# Run client
cd ./client
./build_client.sh
./auto_client

# Run tests
cd ./test
./test_server
./test_util
```

## Client-Server Connections
We adopted the client-server model for our implementation, and we set up auto-running server and client implementations in two separate directories. Detailed description and instructions to run the program are as follows.

### Server's Side
On the server's side, we first defined the port of communications to allow maximum message sizes as well as maximum number of clients being connected. We then created and binded a socket to the port, so that the server waits and listens on the socket. Whenever the client's side sends out a connection request through the socket, the server first accepts the request, and then automatically sends out a message to the client. The server can also read the client's MAC address.

In order to compile and run the server so that it can communicate with clients, first compile server from its source file, and run `./server`. When client and server are connnected, run CLI functions from the server on clients. Make sure that the ports specified in `server.c` and `start.sh` match (both are set at 7400 by default). `start.sh` also contains an IP address that should match the IP address of host server.

### Client's Side
First of all, we created a client socket, whose address information is defined using hostname (host server IP address). Then, the client sends out a request to make a connection with the corresponding server socket that it desires to communicate with. If the client received a message from the server, it first reads the given message from the socket file descriptor. The message is then executed using `popen`, which helps to create a pipe, fork the process, and run the child process on the shell. If the message is `"quit"`, the socket is then closed so that the client connection is ended.

* autorun.service: Used to call autorun.sh automatically client-side
* install.sh: Shell script using systemd to install and start autorun
* autorun.sh: Shell script containing call to auto_client binary
* auto_client: Script opening client socket, connecting to host server, and running sent commands



To compile and run the client's side, first compile the source file `auto_client.c` and run `./auto_client` in the same file directory. 

## MAC Address
To avoid using public IP addresses for testing purposes, we auto-generated Media Access Control (MAC) addresses in terms of 12-digit hexadecimal values that are assigned to each device connected to network. These MAC addresses are required when we try to locate each individual device, and they can be found on the device network interface card (NIC).
