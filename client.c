#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>

#define SERVER_IP "10.59.174.169"  // Replace with actual host IP
#define SERVER_PORT 21493          // Replace with actual port
#define BUFFER_SIZE 1024
#define TIMEOUT_SEC 5              // Timeout in seconds

int connect_with_timeout(int sock, struct sockaddr_in *server_addr) {
    // Set socket to non-blocking
    fcntl(sock, F_SETFL, O_NONBLOCK);

    int result = connect(sock, (struct sockaddr *)server_addr, sizeof(*server_addr));
    if (result < 0 && errno != EINPROGRESS) {
        perror("Immediate connect failed");
        return -1;
    }

    fd_set wait_set;
    FD_ZERO(&wait_set);
    FD_SET(sock, &wait_set);

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;

    result = select(sock + 1, NULL, &wait_set, NULL, &timeout);
    if (result <= 0) {
        fprintf(stderr, "Connection timed out or failed\n");
        return -1;
    }

    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
    if (so_error != 0) {
        fprintf(stderr, "Socket error: %s\n", strerror(so_error));
        return -1;
    }

    // Set back to blocking
    fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) & ~O_NONBLOCK);
    return 0;
}

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

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        return 1;
    }

    if (connect_with_timeout(sock, &server_addr) != 0) {
        close(sock);
        return 1;
    }

    send_command(sock, "connect");
    send_command(sock, "open door");
    send_command(sock, "disconnect");

    close(sock);
    return 0;
}
