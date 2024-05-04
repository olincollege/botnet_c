#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MSG_SIZE 4096
#define MAX_CLIENTS 150
#define MYPORT 7400

void exitClient(int fd, fd_set *readfds, int fd_array[], int *num_clients) {
    int i;
    close(fd); /* close the socket */
    FD_CLR(fd, readfds); /* remove the client from the set */
    
    for (i = 0; i < *num_clients; i++) {
        if (fd_array[i] == fd) { /* find the client in the array */
            break;
        }
    }

    /* shift elements to remove the closed client */
    for (int j = i; j < (*num_clients) - 1; j++) {
        fd_array[j] = fd_array[j + 1];
    }
    
    (*num_clients)--; /* decrement client count */
}

void close_socket(int num_clients, int fd_array[], const char *msg, int server_sockfd) {
    for (int i = 0; i < num_clients; i++) {
        write(fd_array[i], msg, strlen(msg));
        close(fd_array[i]);
    }
    close(server_sockfd);
    exit(0);
}

void write_msg(int fd, char msg[], fd_set readfds, char kb_msg[], int num_clients, int fd_array[]) {
    int result = read(fd, msg, MSG_SIZE); /* read data from open socket */
    
    if (result == -1) {
        perror("Error reading from client");
        exitClient(fd, &readfds, fd_array, &num_clients); /* remove client on error */
    } else if (result > 0) {
        msg[result] = '\0'; /* ensure null-termination */

        strcat(kb_msg, " "); /* concat space */
        strcat(kb_msg, msg); /* append the message */
        
        /* broadcast message to other clients */
        for (int i = 0; i < num_clients; i++) {
            if (fd_array[i] != fd) { /* don't send to the same client */
                write(fd_array[i], kb_msg, strlen(kb_msg));
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int num_clients = 0;
    int server_sockfd, client_sockfd;
    int addresslen = sizeof(struct sockaddr_in);
    int fd_array[MAX_CLIENTS];
    fd_set readfds, testfds;
    char msg[MSG_SIZE + 1];
    char kb_msg[MSG_SIZE + 10];

    /* Server Setup */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        perror("Error creating server socket");
        return 1;
    }

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(MYPORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };

    if (bind(server_sockfd, (struct sockaddr *)&server_address, addresslen) == -1) {
        perror("Error binding server socket");
        close(server_sockfd);
        return 1;
    }

    if (listen(server_sockfd, 5) == -1) {
        perror("Error listening on server socket");
        close(server_sockfd);
        return 1;
    }

    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);
    FD_SET(0, &readfds); /* keyboard input for quitting */

    printf("\n\t******************** iBOT Server ********************\n");
    printf("\n*** Server waiting (enter \"quit\" to stop): \n");

    /* Main Server Loop */
    while (1) {
        testfds = readfds; /* create a copy of readfds */
        if (select(FD_SETSIZE, &testfds, NULL, NULL, NULL) == -1) {
            perror("Error with select");
            close(server_sockfd);
            return 1;
        }

        for (int fd = 0; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &testfds)) {
                if (fd == server_sockfd) { /* new connection */
                    client_sockfd = accept(server_sockfd, NULL, NULL);
                    if (client_sockfd == -1) {
                        perror("Error accepting new client");
                        continue; /* skip this iteration */
                    }

                    if (num_clients < MAX_CLIENTS) {
                        FD_SET(client_sockfd, &readfds);
                        fd_array[num_clients] = client_sockfd;
                        num_clients++;
                        printf("\n -> Bot No. %d connected\n", num_clients);
                    } else {
                        const char *full_msg = "Too many clients. Try again later.\n";
                        write(client_sockfd, full_msg, strlen(full_msg));
                        close(client_sockfd);
                    }
                } else if (fd == 0) { /* keyboard input */
                    fgets(kb_msg, MSG_SIZE, stdin);
                    if (strcmp(kb_msg, "quit\n") == 0) {
                        close_socket(num_clients, fd_array, "Server shutting down.", server_sockfd);
                    } else {
                        /* send message to all clients */
                        for (int i = 0; i < num_clients; i++) {
                            write(fd_array[i], kb_msg, strlen(kb_msg));
                        }
                    }
                } else { /* client data */
                    write_msg(fd, msg, readfds, kb_msg, num_clients, fd_array);
                }
            }
        }
    }
    return 0;
}
