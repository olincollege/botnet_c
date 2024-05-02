#include "util.h"

#include <stdint.h> // uint16_t
#include <stdio.h>  // perror
#include <stdlib.h> // exit, EXIT_FAILURE

void error_and_exit(const char *error_msg) {
  perror(error_msg);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  exit(EXIT_FAILURE);
}

int open_tcp_socket(void) {
  // IPV4 + TCP Socket
  int listener_d = socket(AF_INET, SOCK_STREAM, 0);

  if (listener_d == -1) {
    error_and_exit("Can't open socket");
  }
  return listener_d;
}

void close_tcp_socket(int sockfd){
  close(sockfd);
  exit(0);
}

struct sockaddr_in socket_address(in_addr_t addr, in_port_t port) {
  struct sockaddr_in name = {
      .sin_family = AF_INET,
      .sin_port = (in_port_t)htons(port),
      .sin_addr.s_addr = htonl(addr),
  };
  return name;

}
