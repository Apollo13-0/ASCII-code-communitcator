# Nombre del ejecutable
TARGET = findWord

# Archivos fuente
SRC = findWord.c server.c aes_decryption.c

# Compilador
CC = mpicc

# Flags de compilación
CFLAGS = -Wall -Werror

# Librerías a enlazar
LIBS = -lcrypto

# Regla principal (default)
all: $(TARGET)

# Regla para compilar el ejecutable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Limpiar archivos generados
clean:
	rm -f $(TARGET)

# Regla para recompilar desde cero
rebuild: clean all
