#include <dirent.h>
#include <linux/if.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h> // exit, EXIT_FAILURE
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

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

  while ((de = readdir(dr)) != NULL) {
    if (de->d_name[0] == 'w' && de->d_name[1] == 'l') {
      closedir(dr);
      return strcpy(network_interface, de->d_name);
    }
  }

  closedir(dr);
  return NULL;
}

int mac_address(char interface[]) {
  struct ifreq s;
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

  // Error Checking
  if (fd == -1) {
    // error_and_exit("Can't open socket");
  }
  strcpy(s.ifr_name, interface);
  if (0 == ioctl(fd, SIOCGIFHWADDR, &s)) {
    int i;
    for (i = 0; i < 6; ++i)
      printf(" %02x", (unsigned char)s.ifr_addr.sa_data[i]);
    puts("\n");
    return 0;
  }

  return 1;
}

int main() {
  char *x = find_devices("/sys/class/net");
  printf("STRING : %s\n", x);
  mac_address(x);
}
