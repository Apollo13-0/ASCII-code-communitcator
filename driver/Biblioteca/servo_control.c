// servo_control.c
#include "servo_control.h"
#include <stdio.h>
#include <stdlib.h>

// Supongamos que el driver permite controlar el servo a través de un archivo especial en /dev/gpio_servo
#define GPIO_SERVO_DRIVER "../gpio-driver.c"

// Variable de archivo para manejar el driver
static FILE *servo_driver = NULL;

// Inicializa el servo, abriendo el driver y configurando el pin GPIO
int init_servo(int gpio_pin) {
    servo_driver = fopen(GPIO_SERVO_DRIVER, "w");
    if (servo_driver == NULL) {
        perror("Error al abrir el driver del servo");
        return -1;
    }
    fprintf(servo_driver, "INIT %d\n", gpio_pin); // Suponiendo que el driver soporta inicialización por pin
    fflush(servo_driver);
    return 0;
}

// Convierte un carácter hexadecimal (0-F) a un ángulo entre 0 y 180 grados
int hex_to_angle(char hex_char) {
    if (hex_char >= '0' && hex_char <= '9') {
        return (hex_char - '0') * 11.25;
    } else if (hex_char >= 'A' && hex_char <= 'F') {
        return (hex_char - 'A' + 10) * 11.25;
    } else if (hex_char >= 'a' && hex_char <= 'f') {
        return (hex_char - 'a' + 10) * 11.25;
    } else {
        return -1; // Caracter inválido
    }
}

// Mueve el servo al ángulo especificado
int move_servo(int angle) {
    if (servo_driver == NULL) {
        fprintf(stderr, "El driver no está inicializado.\n");
        return -1;
    }
    fprintf(servo_driver, "MOVE %d\n", angle); // Enviamos el comando al driver
    fflush(servo_driver);
    return 0;
}

// Cierra el driver y libera los recursos
void close_servo() {
    if (servo_driver != NULL) {
        fprintf(servo_driver, "CLOSE\n"); // Comando para liberar el driver si es necesario
        fclose(servo_driver);
        servo_driver = NULL;
    }
}
