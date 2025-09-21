#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "10.59.174.169"  // Replace with your host IP
#define SERVER_PORT 21493          // Replace with your UCID-based port
#define BUFFER_SIZE 1024

void send_command(int sock, const char *command) {
    send(sock, command, strlen(command), 0);
    char buffer[BUFFER_SIZE] = {0};
    int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Sent: %s\n", command);
        printf("Received: %s\n", buffer);
    } else {
        perror("recv");
    }
}

int main() {
    int sock;
    struct sockaddr_in server_addr;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return 1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Send commands
    send_command(sock, "connect");
    send_command(sock, "open");
    send_command(sock, "disconnect");

    // Close socket
    close(sock);
    return 0;
}
