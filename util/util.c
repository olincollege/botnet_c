#include "util.h"

#include <stdint.h> // uint16_t
#include <stdio.h>  // perror
#include <stdlib.h> // exit, EXIT_FAILURE
#include <unistd.h>

void error_and_exit(const char *error_msg) {
  perror(error_msg);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  exit(EXIT_FAILURE);
}

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

void try_connect(int client_socket, struct sockaddr_in server_addr) {
  // Try connecting
  if (connect(client_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    error_and_exit("Connection failed\n");
  }
}
