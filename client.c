#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Convert hex string to byte array
int hex_to_bytes(const char *hex, unsigned char *bytes) {
    int len = strlen(hex);
    if (len % 2 != 0) return -1;

    for (int i = 0; i < len / 2; i++) {
        sscanf(hex + 2*i, "%2hhx", &bytes[i]);
    }
    return len / 2;
}

void send_hex_command(int sock, const char *hex_command) {
    unsigned char buffer[BUFFER_SIZE];
    int byte_len = hex_to_bytes(hex_command, buffer);
    if (byte_len < 0) {
        fprintf(stderr, "Invalid hex string\n");
        return;
    }

    send(sock, buffer, byte_len, 0);

    char response[BUFFER_SIZE] = {0};
    int bytes_received = recv(sock, response, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        response[bytes_received] = '\0';
        printf("Sent hex: %s\n", hex_command);
        printf("Received: %s\n", response);
    } else {
        perror("recv");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        return 1;
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Replace these with the actual hex payloads
    send_hex_command(sock, "3996123f13a9c1a37321016566b85cb3"); // connect
    send_hex_command(sock, "e19b15d6cdb17ecaaacab66501cbdb26"); // open
    send_hex_command(sock, "393c7eb9b0ba57eb5c385304f9722f62"); // disconnect

    close(sock);
    return 0;
}
