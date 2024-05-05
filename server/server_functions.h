#pragma once
#include <netinet/in.h>   // port, struct sockaddr_in, in_addr_t, in_port_t
#include <stdnoreturn.h>  //noreturn

#include "../util/util.h"


/**
 * Exit client after message read from open socket is not readable.
 *
 * Upon an invalid message from an open socket, the function will close the
 * socket connected to the faulty client. The socket is closed, the socket is
 * removed from the total set of client sockets, and the array containing
 * information for the file descriptors of all the file descriptors is adjusted
 * for the deleted fd.
 *
 * @param fd An integer representing the file descriptor of the client socket to
 * be removed.
 * @param readfds A fd_set type defined as the pointer of readfds, which is
 * FD_ZERO to clear the value.
 * @param fdarray An array of integers representing the array of file
 * descriptors for the sockets connected to all clients.
 * @param num_clients A pointer to an integer num_clients that represents the
 * total number of clients connected.
 */

void exitClient(int fd, fd_set* readfds, char fd_array[], int* num_clients);

/**
 * Closes all client sockets, sends a final message, and shuts down the server.
 *
 * This function takes a list of client file descriptors and a message,
 * sends the message to each client, closes their sockets, and then
 * shuts down the server by closing the server socket and exiting the program.
 *
 * @param num_clients The number of currently connected clients.
 * @param fd_array An array of file descriptors representing connected clients.
 * @param msg The message to send to each client before closing their
 * connection.
 * @param server_sockfd The file descriptor for the server's listening socket.
 */
void close_socket(int num_clients, int fd_array[], const char* msg,
                  int server_sockfd);

/**
 * Checks for errors and closes the server socket if an error occurs.
 *
 * @param result The result of a system call.
 * @param error_msg Error message to print.
 * @param server_sockfd The server socket file descriptor.
 */
void error_and_close(int result, const char* error_msg, int server_sockfd);


/**
 * @brief Sends messages to all connected clients.
 *
 * @param num_clients Number of connected clients.
 * @param fd_array Array containing file descriptors of connected clients.
 * @param server_sockfd The server socket file descriptor.
 */
void send_messages(int num_clients, char fd_array[], int server_sockfd);

/**
 * @brief Receives messages from a client and broadcasts them to all other clients.
 *
 * @param result The result of a system call.
 * @param recieve_msg Buffer to store received message.
 * @param num_clients Number of connected clients.
 * @param fd File descriptor of the client.
 * @param fd_array Array containing file descriptors of connected clients.
 */
void receive_messages(int result, char recieve_msg[], int num_clients, int fd, char fd_array[]);
