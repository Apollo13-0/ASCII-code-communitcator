#include <stdio.h>
#include <stdlib.h>
#include "Server/server.h"  // For decrypt_message function declaration

#define DECRYPTED_TEXT_SIZE 1024*1000

void test_connection() {
    // Buffer to store decrypted text
    unsigned char decrypted_text[DECRYPTED_TEXT_SIZE];

    // Receive and decrypt the message
    int decrypted_len = receive_and_decrypt_message(decrypted_text, DECRYPTED_TEXT_SIZE);
    if (decrypted_len > 0) {
        printf("Decrypted message: %s\n", decrypted_text);
    } else {
        fprintf(stderr, "Failed to receive or decrypt the message.\n");
    }
}

int main() {
    test_connection();
    return 0;
}


