#include "find_mac.h"

#include <unistd.h>

void error_and_exit(const char *error_msg) {
  perror(error_msg);
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  exit(EXIT_FAILURE);
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

void mac_address(char interface[], char addr[]) {
  struct ifreq s;
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  char temp[3];

  // Error Checking
  if (fd == -1) {
    // error_and_exit("Can't open socket");
  }
  strcpy(s.ifr_name, interface);
  if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
    addr[0] = '\0'; // Initialize addr as an empty string
    for (int i = 0; i < 6; ++i) {
      sprintf(temp, "%02X", (unsigned char)s.ifr_hwaddr.sa_data[i]);
      strcat(addr, temp);
    }
  } else {
    perror("Error getting MAC address");
    exit(EXIT_FAILURE);
  }
  close(fd); // Close the socket
}

int main() {
  char *x = find_devices("/sys/class/net");
  // printf("STRING : %s\n", x);
  char y[12];
  mac_address(x, y);

  printf("%s\n", y);
}
