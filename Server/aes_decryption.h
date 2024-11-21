#ifndef AES_DECRYPTION_H
#define AES_DECRYPTION_H

#define AES_KEY_SIZE 32  // 256-bit AES key
#define AES_BLOCK_SIZE 16  // 128-bit block size for AES


void handleErrors();
int aes_decrypt(unsigned char *ciphertext, int ciphertext_len, 
                unsigned char *iv, unsigned char *plaintext);
int decrypt_message(unsigned char *ciphertext, int ciphertext_len, unsigned char *iv, 
                    unsigned char *output_plaintext);

#endif