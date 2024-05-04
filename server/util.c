#include "util.h"

#include <stdint.h> // uint16_t
#include <stdio.h>  // perror
#include <stdlib.h> // exit, EXIT_FAILURE
#include <unistd.h>

int open_tcp_socket(void) {
  // IPV4 + TCP Socket
  int listener_d = socket(AF_INET, SOCK_STREAM, 0);

  if (listener_d == -1) {
    error_and_exit("Can't open socket \n");
  }
  return listener_d;
}

void close_tcp_socket(int sockfd) {
  close(sockfd);
  exit(0);
}
