#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../util/util.h"
#define MSG_SIZE 4096
#define MAX_CLIENTS 150
#define MYPORT 7400

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
  fgets(message, MSG_SIZE, stdin);
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

int main(int argc, char* argv[]) {
  int i = 0;
  int count = 0;
  char pass[1];
  int port, result;
  int num_clients = 0;
  int server_sockfd, client_sockfd;
  int fd;
  char fd_array[MAX_CLIENTS];
  fd_set readfds, testfds, clientfds;
  char msg[MSG_SIZE + 1];
  char kb_msg[MSG_SIZE + 10];

  /*Server*/

  port = MYPORT;

  printf("\n*** Server waiting (enter \"quit\" to stop): \n");
  fflush(stdout);

  /* Create and name a socket for the server */

  server_sockfd = open_tcp_socket();

  struct sockaddr_in server_address = {
      .sin_family = AF_INET,
      .sin_port = htons(MYPORT),
      .sin_addr.s_addr = htonl(INADDR_ANY),
  };

  int bind_result = bind(server_sockfd, (struct sockaddr*)&server_address,
                         sizeof(server_address));
  error_and_close(bind_result, "Error binding server socket\n", server_sockfd);

  /* Create a connection queue and initialize a file descriptor set */
  int listen_result = listen(server_sockfd, 5);

  error_and_close(listen_result, "Error binding server socket\n",
                  server_sockfd);

  FD_ZERO(&readfds);
  FD_SET(server_sockfd, &readfds);
  FD_SET(0, &readfds); /* Add keyboard to file descriptor set */

  /* Now wait for clients and requests */

  while (1) {
    testfds = readfds;
    int select_result = select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
    error_and_close(select_result, "Select Error", server_sockfd);
    /* If there is activity, find which descriptor it's on using FD_ISSET */

    for (fd = 0; fd < FD_SETSIZE; fd++) {
      if (FD_ISSET(fd, &testfds)) {
        if (fd == server_sockfd) {
          /* Accept a new connection request */
          client_sockfd = accept(server_sockfd, NULL, NULL);

          if (num_clients < MAX_CLIENTS) {
            FD_SET(client_sockfd, &readfds);
            fd_array[num_clients] = client_sockfd;
            /*Client ID*/
            printf("\n -> Server No. %d standby for orders\n", ++num_clients);
            fflush(stdout);
          } else {
            sprintf(msg, "Too many clients.  Try again later.\n");
            write(client_sockfd, msg, strlen(msg));
            close(client_sockfd);
          }
        } else if (fd == 0) {
          // KEYBOARD ACTIVITY
          send_messages(num_clients, fd_array, server_sockfd);
        } else if (fd) {
          // CLIENT RECEIVE
          result = read(fd, msg, MSG_SIZE); /*read data from open socket*/
          receive_messages(result, msg, num_clients, fd, fd_array);
        } else {
          exitClient(fd, &readfds, fd_array,
                     &num_clients); /* A client is leaving */
        }
      }
    }
  }
}
