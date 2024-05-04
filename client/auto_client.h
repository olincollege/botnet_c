#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>

#pragma once

/**
 * Receive and process messages from the server.
 *
 * @param sockfd The socket file descriptor.
 * @param msg The buffer to store the received message.
 * @param msg_size The size of the buffer.
 * @return A FILE pointer to the output pipe for executed commands.
 */
FILE *recv_exec_msg(int sockfd, char msg[], int msg_size);

/**
 * Find network devices in the specified directory path.
 *
 * @param path The path of the directory to search for network devices.
 * @return A pointer to a string containing the name of the network device found, or NULL if not found.
 */
char *find_devices(char path[]);

/**
 * Retrieve the MAC address of the specified network interface.
 *
 * @param interface The name of the network interface.
 * @param addr The buffer to store the MAC address (in hexadecimal format).
 * @return 0 on success, -1 on failure.
 */
int mac_address(char interface[], char addr[]);
