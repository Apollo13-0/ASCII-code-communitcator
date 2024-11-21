#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include "aes_decryption.h"

//sudo apt-get install libssl-dev
//gcc -o aes_decryption aes_decryption.c -lcrypto

// Static AES key (same as Python)
unsigned char STATIC_AES_KEY[AES_KEY_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
};


void handleErrors(int code) {
    printf("Error code %d: ", code);
    fprintf(stderr, "An error occurred during decryption.\n");
    exit(EXIT_FAILURE);
}

int aes_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    // Create and initialize the context
    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors(-1);

    // Initialize the decryption operation.
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, STATIC_AES_KEY, iv))
        handleErrors(-2);

    // Provide the message to be decrypted, and obtain the plaintext output.
    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors(-3);
    plaintext_len = len;

    // Finalize the decryption.
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) 
        handleErrors(-4);
    plaintext_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int decrypt_message(unsigned char *ciphertext, int ciphertext_len, unsigned char *iv, unsigned char *output_plaintext) {
    int decrypted_len = aes_decrypt(ciphertext, ciphertext_len, iv, output_plaintext);
    output_plaintext[decrypted_len] = '\0';  // Null-terminate the decrypted string
    return decrypted_len;
}