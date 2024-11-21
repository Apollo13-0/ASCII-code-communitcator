#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <stdint.h>

// Inicializa el servo en el pin GPIO especificado
int init_servo(int gpio_pin);

// Convierte un carácter hexadecimal (0-F) a un ángulo entre 0 y 180 grados
int hex_to_angle(char hex_char);

// Mueve el servo al ángulo especificado
int move_servo(int angle);

// Libera los recursos del servo
void close_servo();

#endif // SERVO_CONTROL_H
