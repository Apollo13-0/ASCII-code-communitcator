#include <stdio.h>       // Para perror, printf, fprintf, stderr
#include <stdlib.h>      // Para exit
#include <string.h>      // Para memcpy
#include <arpa/inet.h>   // Para sockaddr_in, htons, etc.
#include <unistd.h>      // Para close y read
#include "server.h"
#include "aes_decryption.h"


int receive_and_decrypt_message(unsigned char *decrypted_text, int decrypted_text_size) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    unsigned char buffer[BUFFER_SIZE] = {0};

    // Crear socket del servidor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return -1;
    }

    // Configurar socket
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

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept");
        close(server_fd);
        return -1;
    }

    printf("Connection established with client.\n");

    // Leer los primeros 4 bytes para obtener la longitud del mensaje
    unsigned char length_buffer[4];
    if (read(new_socket, length_buffer, 4) != 4) {
        perror("Failed to read message length");
        close(new_socket);
        close(server_fd);
        return -1;
    }
    int message_length = ntohl(*(int*)length_buffer);
    printf("Message length: %d bytes\n", message_length);

    if (message_length > BUFFER_SIZE) {
        fprintf(stderr, "Message too large for buffer\n");
        close(new_socket);
        close(server_fd);
        return -1;
    }

    // Leer el resto del mensaje
    int total_read = 0;
    while (total_read < message_length) {
        int bytes_read = read(new_socket, buffer + total_read, message_length - total_read);
        if (bytes_read <= 0) {
            perror("Read failed");
            close(new_socket);
            close(server_fd);
            return -1;
        }
        total_read += bytes_read;
    }
    printf("Received %d bytes of data\n", total_read);

    // Separar IV y datos cifrados
    unsigned char iv[AES_BLOCK_SIZE];
    unsigned char *ciphertext = buffer + AES_BLOCK_SIZE;
    int ciphertext_len = total_read - AES_BLOCK_SIZE;
    memcpy(iv, buffer, AES_BLOCK_SIZE);

    // Desencriptar el mensaje
    int decrypted_len = decrypt_message(ciphertext, ciphertext_len, iv, decrypted_text);
    if (decrypted_len <= 0) {
        fprintf(stderr, "Decryption failed\n");
        close(new_socket);
        close(server_fd);
        return -1;
    }

    decrypted_text[decrypted_len] = '\0';  // Null-terminate el texto desencriptado
    printf("Decrypted message: %s\n", decrypted_text);

    close(new_socket);
    close(server_fd);
    return decrypted_len;
}
