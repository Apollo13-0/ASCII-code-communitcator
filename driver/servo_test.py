import ctypes
import time

# Cargar la biblioteca
servo_lib = ctypes.CDLL('./libservo.so')

# Definir prototipos
servo_lib.set_servo_position.argtypes = [ctypes.c_char]
servo_lib.buscarLetra.argtypes = [ctypes.c_char]
servo_lib.get_servo_position.restype = ctypes.c_char

# Pruebas
print("Haciendo barrido de 0x0 a 0xF:")
for hex_char in "0123456789ABCDEF":
    servo_lib.buscarLetra(hex_char.encode('utf-8'))
    time.sleep(1)

print("Obteniendo posición actual del servo:")
position = servo_lib.get_servo_position().decode('utf-8')
print(f"Posición actual: 0x{position}")

