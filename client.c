#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

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
// This is the client requested in v1 and essentialy acts as garage_attacker (run ./client host_ip server_port
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

    // Replace these with the actual hex payloads for v1-6 (excluding 5 as it is changing) Now on 4
    send_hex_command(sock, "f8e9df6b3d8ea7e6c95e440834b862eca06b318a7303a5"); 
    send_hex_command(sock, "08e4ee968bd45fe918671dd0c909898d91895159"); 
    send_hex_command(sock, "123d7d1a219704857916620b30238ce0e9eb324af3e046cd6ab9"); 


    close(sock);
    return 0;
} 

// v1
// 636F6E6E656374 (connect)
// 6F70656E (open)
// 646973636F6E6E656374 (disconnect)
// SECURITY BREACH: DOOR OPEN!  code: 832C25BC

// v2
// 3996123f13a9c1a37321016566b85cb3 
// e19b15d6cdb17ecaaacab66501cbdb26 
// 393c7eb9b0ba57eb5c385304f9722f62
// SECURITY BREACH: DOOR OPEN!  code: 1313B098

// v3
// d5a776c48d9ab3681770c66cd769ba1a3c2faf6d814a66a768390143b7ef489d
// 6e98fd1d9cc44e744367368dc772449c21c6aad856de995fd128587565a6f839 
// 73c9c537173a7b7e700845e34ce1fc06a3b3e22b9a25d2a2920e9648c547908c
// SECURITY BREACH: DOOR OPEN!  code: B74B2245

// v4
// f8e9df6b3d8ea7e6c95e440834b862eca06b318a7303a5
// 08e4ee968bd45fe918671dd0c909898d91895159
// 123d7d1a219704857916620b30238ce0e9eb324af3e046cd6ab9
// SECURITY BREACH: DOOR OPEN!  code: 9F42CDB5

// v5
// SECURITY BREACH: DOOR OPEN!  code: 34527C84

// v6
// 505a942cdf64c3
// 5c459f2c
// 575c8921d569d9aaef2b
// SECURITY BREACH: DOOR OPEN!  code: 5EF40527
