#ifndef SERVO_LIB_H
#define SERVO_LIB_H

// Establece un ángulo específico para el servo
void set_servo_position(int angle);

// Rota el servo en sentido horario (aumenta el ángulo)
void rotacionHorario();

// Rota el servo en sentido antihorario (disminuye el ángulo)
void rotacionAntihorario();

// Obtiene el ángulo actual del servo
int get_servo_position();

#endif

