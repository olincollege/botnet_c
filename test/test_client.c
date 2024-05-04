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

/* Include the header file for the functions you want to test */
#include "../client/auto_client.h"
#include "../client/util.h"

Test(recv_exec_msg, test_valid_execution) {
  // Test case to check the execution of recv_exec_msg function with a valid
  // input
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(12345); 
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

  char msg[4096]; 
  FILE *output_pipe = recv_exec_msg(sockfd, msg, sizeof(msg));

  // Assert statements to check the output or behavior of the function
  ASSERT_NOT_NULL(output_pipe);
  // Add more assertions if needed

  // Clean up
  fclose(output_pipe);
  close(sockfd);
}

Test(find_devices, test_valid_interface) {
  // Test case to check the find_devices function with a valid input
  char *interface = find_devices("/sys/class/net");

  // Assert statements to check the output or behavior of the function
  ASSERT_NOT_NULL(interface);
  // Add more assertions if needed
}
