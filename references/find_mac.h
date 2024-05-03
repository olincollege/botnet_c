#pragma once

#include <dirent.h>
#include <linux/if.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>      // exit, EXIT_FAILURE
#include <stdnoreturn.h> // noreturn
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

/**
 * Print an error message and exit with a failure status code.
 *
 * Upon an error, print an error message with a desired prefix. The prefix
 * error_msg should describe the context in which the error occurred, followed
 * by a more specific message corresponding to errno set by whatever function or
 * system call that encountered the error. This function exits the program and
 * thus does not return.
 *
 * @param error_msg The error message to print.
 */
noreturn void error_and_exit(const char *error_msg);

/**
 * Finds a list of network interfaces connected to the device
 *
 *
 * @param path is a string which is the path to the device's system network
 * folder. Usually it is sys/class/net for most Linux OSs
 *
 * @return a String which is the WLAN network interface name
 */
char *find_devices(char path[]);

