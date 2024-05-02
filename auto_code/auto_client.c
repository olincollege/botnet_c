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

char *getSystemCommandOutput(const char *command) {
  int MAX_BUFFER_SIZE = 100;
  char buffer[MAX_BUFFER_SIZE];
  char *result = NULL;
  FILE *pipe = popen(command, "r");

  if (!pipe) {
    printf("Error: Failed to execute command.\n");
    return NULL;
  }

  while (fgets(buffer, MAX_BUFFER_SIZE, pipe) != NULL) {
    // Append the output to the result string
    if (result == NULL) {
      result = strdup(buffer);
    } else {
      char *temp = realloc(result, strlen(result) + strlen(buffer) + 1);
      if (temp == NULL) {
        printf("Error: Memory allocation failed.\n");
        free(result);
        pclose(pipe);
        return NULL;
      }
      result = temp;
      strcat(result, buffer);
    }
  }

  pclose(pipe);
  return result;
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

  free(recv_line);

  return 0;
}

int main(int argc, char *argv[]) {
  int i = 0, client_sockfd;
  struct sockaddr_in server_address;
  int addresslen = sizeof(struct sockaddr_in), fd;
  char fd_array[MAX_CLIENTS];
  fd_set readfds, testfds, clientfds;
  char msg[MSG_SIZE + 1];
  char kb_msg[MSG_SIZE + 10];

  /*Client variables*/

  int sockfd;
  int result;
  char hostname[MSG_SIZE] = "127.0.0.1";
  int port = 7400;

  struct hostent *hostinfo;
  char alias[MSG_SIZE];
  int clientid;

  /*Client*/

  printf("\n*** Client program starting (enter \"quit\" to stop): \n");
  fflush(stdout);

  /* Create a socket for the client */
  sockfd = open_tcp_socket();
  /* Name the socket, as agreed with the server */
  hostinfo = gethostbyname(hostname); /* look for host's name */
  // Make a sockaddr_in for binding/connecting
  struct sockaddr_in address = {
      .sin_family = AF_INET,
      .sin_port = (in_port_t)htons(port),
      .sin_addr = *(struct in_addr *)*hostinfo->h_addr_list,
  };

  /* Connect the socket to the server's socket */
  try_connect(sockfd, address);
  FILE *socket_file = get_socket_file(sockfd);
  int socket_file_status = 0;
  while (socket_file_status != -1) {
    socket_file_status = command_recv(socket_file);

    // socket_file_status = command_recv(socket_file);
  }

  if (!feof(stdin) && !feof(socket_file)) {
    error_and_exit("Error reading or writing line:");
  }

  // Clean up and exit.
  printf("Closing TCP");
  close_tcp_socket(sockfd);
  return 0;
}
