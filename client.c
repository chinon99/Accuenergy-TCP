#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 3) {
        printf("Usage: %s <server IP> <server port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};
    int message_count = 0;

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address and port
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    // Convert IP address string to binary format
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        perror("invalid address");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    // Main loop to send messages and receive responses
    while (true) {
        // Send a message with the current count
        snprintf(buffer, BUFFER_SIZE, "Message count: %d", message_count++);
        send(sock, buffer, strlen(buffer), 0);

        // Receive the server's response
        int bytes_received = read(sock, buffer, BUFFER_SIZE);
        buffer[bytes_received] = '\0';
        printf("Server response: %s\n", buffer);

        // Wait for 1 second before sending the next message
        sleep(1);
    }

    return 0;
}
