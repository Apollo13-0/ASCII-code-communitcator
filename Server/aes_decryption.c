#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

#define AES_KEY_SIZE 32  // 256-bit AES key
#define AES_BLOCK_SIZE 16  // 128-bit block size for AES

//sudo apt-get install libssl-dev
//gcc -o aes_decryption aes_decryption.c -lcrypto

// Static AES key matching the Python code (32 bytes)
unsigned char STATIC_AES_KEY[AES_KEY_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
};

void handleErrors() {
    fprintf(stderr, "An error occurred.\n");
    exit(EXIT_FAILURE);
}

int aes_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    // Create and initialize the context
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    // Initialize the decryption operation.
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    // Provide the message to be decrypted, and obtain the plaintext output.
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    // Finalize the decryption.
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) 
        handleErrors();
    plaintext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int main() {
    FILE *encrypted_file = fopen("../Client/encrypted_content.bin", "rb");
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
    int decrypted_text_len = aes_decrypt(ciphertext, ciphertext_len, STATIC_AES_KEY, iv, decrypted_text);
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
