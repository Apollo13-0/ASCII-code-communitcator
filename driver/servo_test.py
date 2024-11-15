import ctypes
import time

# Cargar la biblioteca
servo_lib = ctypes.CDLL('./libservo.so')

# Definir prototipos de funciones
servo_lib.set_servo_position.argtypes = [ctypes.c_int]
servo_lib.rotacionHorario.argtypes = []
servo_lib.rotacionAntihorario.argtypes = []
servo_lib.get_servo_position.restype = ctypes.c_int

# Funciones en Python que llaman a la biblioteca C
def set_servo_position(angle):
    servo_lib.set_servo_position(angle)

def rotacionHorario():
    servo_lib.rotacionHorario()

def rotacionAntihorario():
    servo_lib.rotacionAntihorario()

def get_servo_position():
    return servo_lib.get_servo_position()

# Pruebas
print("Estableciendo el servo a 0 grados.")
set_servo_position(0)
time.sleep(1)

print("Estableciendo el servo a 90 grados.")
set_servo_position(90)
time.sleep(1)

print("Estableciendo el servo a 180 grados.")
set_servo_position(180)
time.sleep(1)

print("Obteniendo el ángulo actual del servo.")
angle = get_servo_position()
print(f"Ángulo actual: {angle} grados")
time.sleep(1)

print("Rotando en sentido horario.")
rotacionHorario()
time.sleep(1)

print("Rotando en sentido antihorario.")
rotacionAntihorario()
time.sleep(1)

