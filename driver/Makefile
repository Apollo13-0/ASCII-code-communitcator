# Compilación del driver
obj-m += gpio_driver.o

all: driver library_shared library_static servo_test install_lib

# Compilar el driver
driver:
	sudo rmmod gpio_driver
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod gpio_driver.ko
	

# Compilar la biblioteca compartida
library_shared: servo_lib.o
	gcc -shared -o libservo.so -fPIC servo_lib.c

# Compilar la biblioteca estática
library_static: servo_lib.o
	ar rcs libservo.a servo_lib.o

# Compilar el programa de prueba en C que usa la biblioteca estática
servo_test: servo_test.o libservo.a
	gcc -o servo_test servo_test.o -L. -lservo

# Compilar el archivo objeto de la biblioteca
servo_lib.o: servo_lib.c
	gcc -c -fPIC servo_lib.c -o servo_lib.o

# Compilar el archivo objeto del programa de prueba en C
servo_test.o: servo_test.c
	gcc -c servo_test.c -o servo_test.o

# Copiar la biblioteca compartida al directorio del sistema y actualizar las bibliotecas
install_lib:
	sudo cp libservo.so /usr/local/lib/
	sudo ldconfig

# Limpiar archivos generados
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f *.o *.a *.so servo_test

