#include <stdio.h>
#include <stdlib.h>
#include "Server/server.h"  // For decrypt_message function declaration

#define DECRYPTED_TEXT_SIZE 1024*2000

#define OUTPUT_FILE "decrypted_message.txt"

// Function to save the decrypted message to a file
void save_to_file(const char *filename, const char *data) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    fprintf(file, "%s", data);
    fclose(file);
    printf("Decrypted message saved to %s\n", filename);
}

void test_connection(unsigned char* decrypted_text) {
    // Receive and decrypt the message
    int decrypted_len = receive_and_decrypt_message(decrypted_text, DECRYPTED_TEXT_SIZE);
    if (decrypted_len > 0) {
        printf("Decrypted message: %s\n", decrypted_text);
    } else {
        fprintf(stderr, "Failed to receive or decrypt the message.\n");
    }
}

int main() {
    // Buffer to store decrypted text
    unsigned char decrypted_text[DECRYPTED_TEXT_SIZE];
    test_connection(decrypted_text);
    // Save the decrypted message to a file
    save_to_file(OUTPUT_FILE, (const char *)decrypted_text);
    return 0;
}


