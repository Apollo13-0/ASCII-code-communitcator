# Directorios
SERVER_DIR = Server
DRIVER_DIR = driver

# Ejecutables
TARGET_FINDWORD = $(SERVER_DIR)/findWord
TARGET_SERVO_TEST = $(DRIVER_DIR)/servo_test

# Archivos fuente
SRC_FINDWORD = $(SERVER_DIR)/findWord.c $(SERVER_DIR)/server.c $(SERVER_DIR)/aes_decryption.c $(DRIVER_DIR)/servo_lib.c
SRC_SERVO_TEST = $(DRIVER_DIR)/servo_test.c $(DRIVER_DIR)/servo_lib.c

# Compilador
CC = mpicc
CC_GCC = gcc

# Flags de compilación
CFLAGS = -Wall -Werror
LIBS = -lcrypto

# Archivos relacionados con el driver
DRIVER_OBJ = $(DRIVER_DIR)/gpio_driver.o
DRIVER_SO = $(DRIVER_DIR)/libservo.so
DRIVER_STATIC = $(DRIVER_DIR)/libservo.a

# Regla principal (default)
all: driver library_shared library_static $(TARGET_FINDWORD) $(TARGET_SERVO_TEST)

# Compilar el driver
driver:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/$(DRIVER_DIR) modules

# Compilar la biblioteca compartida
library_shared: $(DRIVER_DIR)/servo_lib.o
	$(CC_GCC) -shared -o $(DRIVER_SO) -fPIC $(DRIVER_DIR)/servo_lib.c

# Compilar la biblioteca estática
library_static: $(DRIVER_DIR)/servo_lib.o
	ar rcs $(DRIVER_STATIC) $(DRIVER_DIR)/servo_lib.o

# Compilar el programa principal
$(TARGET_FINDWORD): $(SRC_FINDWORD)
	$(CC) $(CFLAGS) -o $(TARGET_FINDWORD) $(SRC_FINDWORD) $(LIBS)

# Compilar el programa de prueba del servo
$(TARGET_SERVO_TEST): $(DRIVER_DIR)/servo_test.o $(DRIVER_STATIC)
	$(CC_GCC) -o $(TARGET_SERVO_TEST) $(DRIVER_DIR)/servo_test.o -L$(DRIVER_DIR) -lservo

# Compilar el archivo objeto de la biblioteca
$(DRIVER_DIR)/servo_lib.o: $(DRIVER_DIR)/servo_lib.c
	$(CC_GCC) -c -fPIC $(DRIVER_DIR)/servo_lib.c -o $(DRIVER_DIR)/servo_lib.o

# Compilar el archivo objeto del programa de prueba del servo
$(DRIVER_DIR)/servo_test.o: $(DRIVER_DIR)/servo_test.c
	$(CC_GCC) -c $(DRIVER_DIR)/servo_test.c -o $(DRIVER_DIR)/servo_test.o

# Instalar la biblioteca compartida en el sistema
install_lib:
	sudo cp $(DRIVER_SO) /usr/local/lib/
	sudo ldconfig

# Limpiar archivos generados
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD)/$(DRIVER_DIR) clean
	rm -f $(SERVER_DIR)/*.o $(DRIVER_DIR)/*.o $(DRIVER_STATIC) $(DRIVER_SO) $(TARGET_FINDWORD) $(TARGET_SERVO_TEST)
