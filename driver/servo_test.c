#include <stdio.h>
#include <unistd.h>
#include "servo_lib.h"

int main() {
    printf("Estableciendo el servo a 0 grados.\n");
    set_servo_position(0);
    sleep(1);

    printf("Estableciendo el servo a 90 grados.\n");
    set_servo_position(90);
    sleep(1);

    printf("Estableciendo el servo a 180 grados.\n");
    set_servo_position(180);
    sleep(1);

    printf("Obteniendo el ángulo actual del servo.\n");
    int angle = get_servo_position();
    printf("Ángulo actual: %d grados\n", angle);
    sleep(1);

    printf("Rotando en sentido horario.\n");
    rotacionHorario();
    sleep(1);

    printf("Rotando en sentido antihorario.\n");
    rotacionAntihorario();
    sleep(1);

    return 0;
}

