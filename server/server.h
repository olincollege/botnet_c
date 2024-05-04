#pragma once
#include "util.h"
#include <netinet/in.h>  // port, struct sockaddr_in, in_addr_t, in_port_t
#include <stdnoreturn.h> //noreturn

/**
 * Exit client after message read from open socket is not readable.
 *
 * Upon an invalid message from an open socket, the function will close the socket connected
 * to the faulty client. The socket is closed, the socket is removed from the total set of client
 * sockets, and the array containing information for the file descriptors of all the file
 * descriptors is adjusted for the deleted fd.
 *
 * @param fd An integer representing the file descriptor of the client socket to be removed.
 * @param readfds A fd_set type defined as the pointer of readfds, which is FD_ZERO to clear the 
 * value.
 * @param fdarray An array of integers representing the array of file descriptors for the sockets
 * connected to all clients.
 * @param num_clients A pointer to an integer num_clients that represents the total number of clients
 * connected.
 */

void exitClient(int fd, fd_set *readfds, int fd_array[], int *num_clients);

/**
 * Closes all client sockets, sends a final message, and shuts down the server.
 *
 * This function takes a list of client file descriptors and a message,
 * sends the message to each client, closes their sockets, and then
 * shuts down the server by closing the server socket and exiting the program.
 *
 * @param num_clients The number of currently connected clients.
 * @param fd_array An array of file descriptors representing connected clients.
 * @param msg The message to send to each client before closing their connection.
 * @param server_sockfd The file descriptor for the server's listening socket.
 */
void close_socket(int num_clients, int fd_array[], const char *msg, int server_sockfd);

/**
 * Reads a message from a specific client and broadcasts it to all other clients.
 *
 * This function reads a message from the specified client socket, adds it to the given 
 * message buffer, and then broadcasts it to all other connected clients. If there's an 
 * error during reading, the client is removed from the set and the connection is closed.
 *
 * @param fd The file descriptor for the client from which to read the message.
 * @param msg A buffer to store the message read from the client.
 * @param readfds The set of file descriptors used to track connected clients.
 * @param kb_msg A buffer to store additional text, often used to concatenate with the client's message.
 * @param num_clients The number of currently connected clients.
 * @param fd_array An array of file descriptors representing connected clients.
 */
void write_msg(int fd, char msg[], fd_set readfds, char kb_msg[], int num_clients, int fd_array[]);



