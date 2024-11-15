#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "servo_lib.h"

#define DRIVER_PATH "/dev/gpio_driver"
#define ANGLE_STEP 10   // Paso de ángulo para rotaciones

// Función para establecer un ángulo específico para el servo
void set_servo_position(int angle) {
    int fd = open(DRIVER_PATH, O_WRONLY);
    if (fd == -1) {
        perror("Error al abrir el driver");
        exit(EXIT_FAILURE);
    }

    // Escribir el ángulo en el driver
    if (write(fd, &angle, sizeof(angle)) == -1) {
        perror("Error al enviar el ángulo al driver");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
}

// Función para rotar el servo en sentido horario (aumenta el ángulo)
void rotacionHorario() {
    int current_angle = get_servo_position();
    int new_angle = current_angle + ANGLE_STEP;

    if (new_angle > 180) new_angle = 180; // Limitar a 180 grados
    set_servo_position(new_angle);
}

// Función para rotar el servo en sentido antihorario (disminuye el ángulo)
void rotacionAntihorario() {
    int current_angle = get_servo_position();
    int new_angle = current_angle - ANGLE_STEP;

    if (new_angle < 0) new_angle = 0; // Limitar a 0 grados
    set_servo_position(new_angle);
}

// Función para obtener el ángulo actual del servo
int get_servo_position() {
    int fd = open(DRIVER_PATH, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el driver");
        exit(EXIT_FAILURE);
    }

    int angle;
    if (read(fd, &angle, sizeof(angle)) == -1) {
        perror("Error al leer el ángulo del driver");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
    return angle;
}

