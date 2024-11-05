# Makefile for the AES server application

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Werror

# Source files
SRCS = main.c Server/server.c Server/aes_decryption.c

# Output binary
TARGET = main

# Libraries
LIBS = -lcrypto

# Default target
all: $(TARGET)

# Build the target
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# Clean target
clean:
	rm -f $(TARGET)
