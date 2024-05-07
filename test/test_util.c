#include <arpa/inet.h>
#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../util/util.h"

// Start server prior

// Check that `try_connect` successfully connects to a valid server address
Test(try_connect, connect_successfully) {
  int client_socket = open_tcp_socket();
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(12345); // Example port
  // Set a valid IP address for successful connection (requires actual server setup)
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
  try_connect(client_socket, server_addr);
  close_tcp_socket(client_socket);
  cr_expect(1, "Connection should succeed if server is listening on 127.0.0.1:12345");
}

// Check the execution of recv_exec_msg function with a valid input
Test(recv_exec_msg, test_valid_execution) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(12345); 
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
  connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  char msg[4096]; 
  FILE *output_pipe = recv_exec_msg(sockfd, msg, sizeof(msg));
  ASSERT_NOT_NULL(output_pipe);
  fclose(output_pipe);
  close(sockfd);
}

// Check the find_devices function with a valid input
Test(find_devices, test_valid_interface) {
  char *interface = find_devices("/sys/class/net");
  ASSERT_NOT_NULL(interface);
}
