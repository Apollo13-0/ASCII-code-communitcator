#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "servo_lib.h"

#define DRIVER_PATH "/dev/gpio_driver"

// Función para establecer la posición del servo
void set_servo_position(char hex_char) {
    int fd = open(DRIVER_PATH, O_WRONLY);
    if (fd == -1) {
        perror("Error al abrir el driver");
        exit(EXIT_FAILURE);
    }

    if (write(fd, &hex_char, sizeof(hex_char)) == -1) {
        perror("Error al enviar el valor al driver");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

// Función para obtener la posición actual del servo
char get_servo_position() {
    int fd = open(DRIVER_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el driver");
        exit(EXIT_FAILURE);
    }

    char hex_char;
    if (read(fd, &hex_char, sizeof(hex_char)) == -1) {
        perror("Error al leer la posición del driver");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
    return hex_char;
}

// Función para buscar una letra en el rango 0x0 a 0xF
void buscarLetra(char hex_char) {
    printf("Buscando la letra %c\n", hex_char);
    set_servo_position(hex_char);
}

