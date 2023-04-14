#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    struct sockaddr_in address;
} client_data_t;

void *handle_client(void *arg) {
    client_data_t *client_data = (client_data_t *)arg;
    int client_socket = client_data->socket;
    char buffer[BUFFER_SIZE] = {0};

    while (true) {
        // Read data from the client
        int bytes_read = read(client_socket, buffer, BUFFER_SIZE);

        // If read() returns 0 or a negative value, it means the client has disconnected
        if (bytes_read <= 0) {
            break;
        }

        // Echo the message back to the client
        send(client_socket, buffer, bytes_read, 0);
    }

    // Close the connection and free the memory allocated for the client_socket
    close(client_socket);
    free(arg);
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Display a message indicating the server is running and listening for connections
    printf("Server is running and listening on port %d...\n", PORT);

    // Main loop to accept incoming connections
    while (true) {
        // Accept an incoming connection
        int *new_socket_ptr = (int *)malloc(sizeof(int));
        if ((*new_socket_ptr = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // Print a message when a new client connects
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(address.sin_port);
        printf("New client connected: IP: %s, Port: %d\n", client_ip, client_port);

        // Create a new thread to handle the client
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void *)new_socket_ptr) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }

        // Detach the thread to avoid memory leaks
        pthread_detach(client_thread);
    }

    return 0;
}
