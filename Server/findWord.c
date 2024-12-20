#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include <unistd.h>
#include "server.h"
#include "../driver/servo_lib.h"


#define MAX_TEXT 500000
#define MAX_WORDS 50000
#define OUTPUT_FILE "decrypted_message.txt"


typedef struct {
    char word[50];
    int count;
} WordFreq;

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

int isMonosilaba(const char *palabra) {
    int len = strlen(palabra);
    return (len <= 3);
}

void minimizeText(char *text) {
    for (int i = 0; text[i]; i++) {
        text[i] = tolower(text[i]);
    }
}

int tokenizeText(char *text, WordFreq wordFreq[], int *maxFreq) {
    char *token = strtok(text, " ,.-\n");
    int wordCount = 0;

    while (token != NULL) {
        if (isMonosilaba(token)) {
            token = strtok(NULL, " ,.-\n");
            continue;
        }

        int found = 0;
        for (int i = 0; i < wordCount; i++) {
            if (strcmp(wordFreq[i].word, token) == 0) {
                wordFreq[i].count++;
                if (wordFreq[i].count > *maxFreq) {
                    *maxFreq = wordFreq[i].count;
                }
                found = 1;
                break;
            }
        }

        if (!found && wordCount < MAX_WORDS) {
            strcpy(wordFreq[wordCount].word, token);
            wordFreq[wordCount].count = 1;
            if (*maxFreq == 0) {
                *maxFreq = 1;
            }
            wordCount++;
        }

        token = strtok(NULL, " ,.-\n");
    }
    return wordCount;
}

void mergeWordCounts(WordFreq globalWordFreq[], int *globalWordCount, WordFreq localWordFreq[], int localWordCount) {
    for (int i = 0; i < localWordCount; i++) {
        int found = 0;
        for (int j = 0; j < *globalWordCount; j++) {
            if (strcmp(globalWordFreq[j].word, localWordFreq[i].word) == 0) {
                globalWordFreq[j].count += localWordFreq[i].count;
                found = 1;
                break;
            }
        }
        if (!found && *globalWordCount < MAX_WORDS) {
            strcpy(globalWordFreq[*globalWordCount].word, localWordFreq[i].word);
            globalWordFreq[*globalWordCount].count = localWordFreq[i].count;
            (*globalWordCount)++;
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char text[MAX_TEXT];
    WordFreq wordFreq[MAX_WORDS];
    int maxFreq = 0;
    int wordCount = 0;

    // Master lee el archivo completo
    if (rank == 0) {
        int decrypted_len = receive_and_decrypt_message((unsigned char *)text, MAX_TEXT);
        if (decrypted_len < 0) {
            fprintf(stderr, "Failed to receive or decrypt the message.\n");
        }
        save_to_file(OUTPUT_FILE, (const char *) text);
    }

    // Broadcast del texto completo a todos los nodos
    MPI_Bcast(text, MAX_TEXT, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Dividir el texto en partes iguales
    int chunkSize = strlen(text) / size;
    char localText[MAX_TEXT];
    strncpy(localText, text + rank * chunkSize, chunkSize);
    localText[chunkSize] = '\0';

    // Minimize texto
    minimizeText(localText);

    // Tokenizar y contar palabras en cada nodo
    wordCount = tokenizeText(localText, wordFreq, &maxFreq);

    // Recolectar datos al master
    if (rank == 0) {
        WordFreq globalWordFreq[MAX_WORDS];
        int globalWordCount = 0;
        mergeWordCounts(globalWordFreq, &globalWordCount, wordFreq, wordCount);

        for (int i = 1; i < size; i++) {
            MPI_Recv(wordFreq, MAX_WORDS * sizeof(WordFreq), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&wordCount, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            mergeWordCounts(globalWordFreq, &globalWordCount, wordFreq, wordCount);
        }

        // Imprimir la palabra más frecuente
        printf("Palabras más frecuentes:\n");
        int maxFreqGlobal = 0;
        for (int i = 0; i < globalWordCount; i++) {
            if (globalWordFreq[i].count > maxFreqGlobal) {
                maxFreqGlobal = globalWordFreq[i].count;
            }
        }

        int index;
        for (index = 0; index < globalWordCount; index++) {
            if (globalWordFreq[index].count == maxFreqGlobal) {
                printf("%s (Frecuencia: %d)\n", globalWordFreq[index].word, globalWordFreq[index].count);
                break;
            }
        }

        printf("Mostrando la palabra en el servo:\n");
        for (int i = 0; i < strlen(globalWordFreq[index].word); i++) {
            // Convertir cada carácter a su representación hexadecimal
            unsigned char hex_char = globalWordFreq[index].word[i];

            // Extraer el nibble alto y bajo (4 bits cada uno)
            char high_nibble = (hex_char >> 4) & 0xF; // Obtener los 4 bits más significativos
            char low_nibble = hex_char & 0xF;         // Obtener los 4 bits menos significativos

            // Convertir a caracteres ASCII ('0'-'F')
            char high_char = (high_nibble < 10) ? ('0' + high_nibble) : ('A' + (high_nibble - 10));
            char low_char = (low_nibble < 10) ? ('0' + low_nibble) : ('A' + (low_nibble - 10));

            // Enviar el nibble alto al servo
            buscarLetra(high_char);
            sleep(1);

            // Enviar el nibble bajo al servo
            buscarLetra(low_char);
            sleep(1);
        }


    } else {
        // Enviar resultados al master
        MPI_Send(wordFreq, MAX_WORDS * sizeof(WordFreq), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&wordCount, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}