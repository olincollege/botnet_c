#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>  // for NULL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../util/util.h"

void exitClient(int fd, fd_set* readfds, char fd_array[], int* num_clients) {
  int i;
  close(fd);
  FD_CLR(fd, readfds); /*clear the leaving client from the set*/
  for (i = 0; i < (*num_clients) - 1; i++)
    if (fd_array[i] == fd) break;
  for (; i < (*num_clients) - 1; i++) (fd_array[i]) = (fd_array[i + 1]);
  (*num_clients)--;
}

void error_and_close(int result, const char* error_msg, int server_sockfd) {
  if (result == -1) {
    perror(error_msg);
    close(server_sockfd);
  }
}

void close_socket(int num_clients, char fd_array[], const char* msg,
                  int server_sockfd) {
  for (int i = 0; i < num_clients; i++) {
    write(fd_array[i], msg, strlen(msg));
    close(fd_array[i]);
  }
  close_tcp_socket(server_sockfd);
}

void send_messages(int num_clients, char fd_array[], int server_sockfd) {
  char message[4096] = "";
  fgets(message, 4096, stdin);
  if (strcmp(message, "quit\n") == 0) {
    close_socket(num_clients, fd_array, "Server shutting down.", server_sockfd);
  } else {
    for (int i = 0; i < num_clients; i++)
      write(fd_array[i], message, strlen(message));
  }
}

void receive_messages(int result, char recieve_msg[], int num_clients, int fd,
                      char fd_array[]) {
  char message[4096] = "";
  if (result == -1)
    perror("read()");
  else if (result > 0) {
    recieve_msg[result] = '\0';

    /*concatinate the client id with the client's message*/

    printf("%s\n", strcat(message, recieve_msg));

    /*print to other clients*/

    for (int i = 0; i < num_clients; i++) {
      if (fd_array[i] != fd) /*dont write msg to same client*/
        write(fd_array[i], message, strlen(message));
    }
  }
}
