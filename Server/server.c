#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "server.h"
#include "aes_decryption.h"


// Function to set up the server, receive, and decrypt a message
int receive_and_decrypt_message(unsigned char *decrypted_text, int decrypted_text_size) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    unsigned char buffer[BUFFER_SIZE] = {0};

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return -1;
    }

    // Bind the socket to port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }
    if (listen(server_fd, 3) < 0) {
        perror("Listen");
        close(server_fd);
        return -1;
    }
    printf("Server is listening on port %d...\n", PORT);

    // Accept a single connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept");
        close(server_fd);
        return -1;
    }

    printf("Connection established with client.\n");

    // Receive encrypted data
    int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
    if (bytes_read <= AES_BLOCK_SIZE) {
        fprintf(stderr, "Invalid data received.\n");
        close(new_socket);
        close(server_fd);
        return -1;
    }

    // Separate IV and ciphertext
    unsigned char iv[AES_BLOCK_SIZE];
    unsigned char *ciphertext = buffer + AES_BLOCK_SIZE;
    int ciphertext_len = bytes_read - AES_BLOCK_SIZE;
    memcpy(iv, buffer, AES_BLOCK_SIZE);

    // Decrypt the message
    int decrypted_len = decrypt_message(ciphertext, ciphertext_len, iv, decrypted_text);
    if (decrypted_len <= 0) {
        fprintf(stderr, "Decryption failed.\n");
        close(new_socket);
        close(server_fd);
        return -1;
    }

    decrypted_text[decrypted_len] = '\0';  // Null-terminate the decrypted text

    // Close the connection and server socket
    close(new_socket);
    close(server_fd);

    return decrypted_len;  // Return the length of the decrypted message
}
