#ifndef SERVER_H
#define SERVER_H

#define PORT 65432
#define BUFFER_SIZE 1024*2000 

int receive_and_decrypt_message(unsigned char *decrypted_text, int decrypted_text_size);

#endif