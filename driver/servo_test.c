#include <stdio.h>
#include <unistd.h>
#include "servo_lib.h"

int main() {
    printf("Haciendo barrido de 0x0 a 0xF:\n");
    for (char hex_char = '0'; hex_char <= '9'; hex_char++) {
        buscarLetra(hex_char);
        sleep(1);
    }
    for (char hex_char = 'A'; hex_char <= 'F'; hex_char++) {
        buscarLetra(hex_char);
        sleep(1);
    }

    printf("Obteniendo posición actual del servo:\n");
    char position = get_servo_position();
    printf("Posición actual: 0x%c\n", position);

    return 0;
}

