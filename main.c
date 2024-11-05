#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Server/aes_decryption.h"

int main() {
    // Open the encrypted file and create the output file
    FILE *encrypted_file = fopen("Client/encrypted_content.bin", "rb");
    FILE *decrypted_file = fopen("input_decrypted.txt", "w");

    if (!encrypted_file || !decrypted_file) {
        fprintf(stderr, "Failed to open file(s).\n");
        return EXIT_FAILURE;
    }

    // Load IV from the encrypted file
    unsigned char iv[AES_BLOCK_SIZE];
    if (fread(iv, 1, AES_BLOCK_SIZE, encrypted_file) != AES_BLOCK_SIZE) {
        fprintf(stderr, "Error reading IV.\n");
        return EXIT_FAILURE;
    }

    // Determine ciphertext length
    fseek(encrypted_file, 0, SEEK_END);
    long file_size = ftell(encrypted_file);
    fseek(encrypted_file, AES_BLOCK_SIZE, SEEK_SET);  // Skip the IV

    int ciphertext_len = file_size - AES_BLOCK_SIZE;
    unsigned char *ciphertext = malloc(ciphertext_len);
    if (!ciphertext) {
        fprintf(stderr, "Memory allocation error.\n");
        return EXIT_FAILURE;
    }

    if (fread(ciphertext, 1, ciphertext_len, encrypted_file) != ciphertext_len) {
        fprintf(stderr, "Error reading ciphertext.\n");
        return EXIT_FAILURE;
    }
    fclose(encrypted_file);

    // Buffer for the decrypted text
    unsigned char *decrypted_text = malloc(ciphertext_len);  // Allocate enough space
    if (!decrypted_text) {
        fprintf(stderr, "Memory allocation error.\n");
        return EXIT_FAILURE;
    }

    // Decrypt the ciphertext
    int decrypted_text_len = aes_decrypt(ciphertext, ciphertext_len, iv, decrypted_text);
    decrypted_text[decrypted_text_len] = '\0';  // Null-terminate the plaintext

    // Write the decrypted content to the output file
    fprintf(decrypted_file, "%s", decrypted_text);
    fclose(decrypted_file);

    // Clean up
    free(ciphertext);
    free(decrypted_text);

    printf("Decryption complete. Output saved to input_decrypted.txt\n");
    return EXIT_SUCCESS;
}