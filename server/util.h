#pragma once
#include <netinet/in.h>  // port, struct sockaddr_in, in_addr_t, in_port_t
#include <stdnoreturn.h> //noreturn

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
 * @param sockfd Socket file descriptor number corresponding to the socket we
 * want to close.
 */
void close_tcp_socket(int sockfd);

