#pragma once
#include <netinet/in.h>  // port, struct sockaddr_in, in_addr_t, in_port_t
#include <stdnoreturn.h> //noreturn

/**
 * Print an error message and exit with a failure status code.
 *
 * Upon an error, print an error message with a desired prefix. The prefix
 * error_msg should describe the context in which the error occurred, followed
 * by a more specific message corresponding to errno set by whatever function or
 * system call that encountered the error. This function exits the program and
 * thus does not return.
 *
 * @param error_msg The error message to print.
 */

noreturn void error_and_exit(const char *error_msg);

/**
 * Attempt to open an IPv4 TCP socket.
 *
 * Try to create a TCP socket on IPv4, and return the socket descriptor if
 * successful. If creating the socket is not successful, print an error message
 * and exit the program without returning from the function.
 *
 * @return A socket descriptor corresponding to the new socket.
 */
int open_tcp_socket(void);

/**
 *
 * Closes TCP socket and ends program
 *
 * @param sockfd socket file descriptor number corresponding to the socket we
 * want to close
 */
void close_tcp_socket(int sockfd);

/**
 * Attempt to connect to a server. Generates and error message and ends program
 * if fails to connect
 *
 * @param client_socket socket descriptor of socket we want to connect to
 * @param server_addr A sockaddr_in structure to use with bind/connect, in
 * network order.
 *
 */
void try_connect(int client_socket, struct sockaddr_in server_addr);
