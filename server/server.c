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
#include "server_functions.h"

int main() {
  const uint16_t MYPORT = 7400;
  int MAX_CLIENTS = 150;
  int MSG_SIZE = 4096;

  ssize_t result;
  int num_clients = 0;
  int server_sockfd, client_sockfd;
  int fd;
  char fd_array[MAX_CLIENTS];
  fd_set readfds, testfds;
  char msg[MSG_SIZE + 1];

  /*Server*/

  printf("\n*** Server waiting (enter \"quit\" to stop): \n");
  fflush(stdout);

  /* Create and name a socket for the server */

  server_sockfd = open_tcp_socket();

  struct sockaddr_in server_address = {
      .sin_family = AF_INET,
      .sin_port = (uint16_t)htons(MYPORT),
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
            fd_array[num_clients] = (char)client_sockfd;
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
          result =
              read(fd, msg, (size_t)MSG_SIZE); /*read data from open socket*/
          receive_messages((int)result, msg, num_clients, fd, fd_array);
        } else {
          exitClient(fd, &readfds, fd_array,
                     &num_clients); /* A client is leaving */
        }
      }
    }
  }
}
