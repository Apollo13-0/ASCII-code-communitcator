#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "aes_decryption.h"  // Include the header for aes_decryption functions

#define PORT 65432
#define BUFFER_SIZE 1024*1000

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    unsigned char buffer[BUFFER_SIZE] = {0};

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        printf("Waiting for a connection...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept");
            exit(EXIT_FAILURE);
        }
        printf("Connection established with client.\n");

        // Receive encrypted data
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes_read <= AES_BLOCK_SIZE) {
            fprintf(stderr, "Invalid data received.\n");
            close(new_socket);
            continue;
        }

        // Separate IV and ciphertext
        unsigned char iv[AES_BLOCK_SIZE];
        unsigned char *ciphertext = buffer + AES_BLOCK_SIZE;
        int ciphertext_len = bytes_read - AES_BLOCK_SIZE;
        memcpy(iv, buffer, AES_BLOCK_SIZE);

        // Buffer to store the decrypted text
        unsigned char decrypted_text[BUFFER_SIZE] = {0};

        // Decrypt the message
        int decrypted_len = decrypt_message(ciphertext, ciphertext_len, iv, decrypted_text);
        if (decrypted_len > 0) {
            printf("Decrypted message: %s\n", decrypted_text);
        } else {
            fprintf(stderr, "Decryption failed.\n");
        }

        // Close the connection
        close(new_socket);
    }

    return 0;
}
