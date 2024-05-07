#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <criterion/redirect.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include "../server/server_functions.h"
#include "../util/util.h"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
// All tests were taken and modified from Assignment 8.

// Check that the leaving client is removed from the set and the client count is updated.
Test(exitClient, client_exit_correctly) {
    // Setup: Create a mock scenario with a set of clients and a leaving client
    fd_set readfds;
    int num_clients = 3;
    char fd_array[] = {1, 2, 3};
    int leaving_client_fd = 2;
    exitClient(leaving_client_fd, &readfds, fd_array, &num_clients);
    cr_expect(num_clients == 2, "Incorrect number of clients after client exit");
    for (int i = 0; i < num_clients; i++) {
        cr_expect(fd_array[i] != leaving_client_fd, "Leaving client still in the set");
    }
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
