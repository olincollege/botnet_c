# Botnet Implementation
A botnet is a system of a host server and multiple client servers, in which the host server is able to send commands to clients. Infect client computers connect with server and clients through desired communication protocol, and use hacked devices’ compute power for tasks unknown to device owners. In this project, we implemented botnet by writing the functionalities of a command script using transmission control protocol (TCP).

## Client-Server Connections
We adopted the client-server model for our implementation, and we set up auto-running server and client implementations in two separate directories. Detailed description and instructions to run the program are as follows.

### Server's Side
On the server's side, we first defined the port of communications to allow maximum message sizes as well as maximum number of clients being connected. We then created and binded a socket to the port, so that the server waits and listens on the socket. Whenever the client's side sends out a connection request through the socket, the server first accepts the request, and then automatically sends out a message to the client. The server can also read the client's MAC address.

In order to compile and run the server so that it can communicate with clients, first compile server from its source file, and run `./server` and `./start.sh` respectively in the same directory. When client and server are connnected, run CLI function from server on client. Make sure that the ports specified in `server.c` and `start.sh` must match (both are set at 7400 by default). `start.sh` also contains IP address that should match address of host server.

### Client's Side
First of all, we created a client socket, whose address information is defined using hostname (host server IP address). Then, the client sends out a request to make a connection with the corresponding server socket that it desires to communicate with. If the client received a message from the server, it first reads the given message from the socket file descriptor. The message is then executed using `popen`, which helps to create a pipe, fork the process, and run the child process on the shell. If the message is `"quit"`, the socket is then closed so that the client connection is ended.

To compile and run the client's side, first compile the source file `auto_client.c` and run `./auto_client` in the same file directory. 

## MAC Address
To avoid using public IP addresses for testing purposes, we auto-generated Media Access Control (MAC) addresses in terms of 12-digit hexadecimal values that are assigned to each device connected to network. These MAC addresses are required when we try to locate each individual device, and they can be found on the device network interface card (NIC).










The README describes in sufficient detail what the project is or does.
The README describes in sufficient detail the packages needed to compile and run the project.
The README describes in sufficient detail instructions to compile and run the project.