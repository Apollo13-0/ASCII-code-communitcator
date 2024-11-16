#include <ESP32Servo.h>

Servo myServo;  // Creación del objeto para controlar el servo

// Define el pin PWM al que está conectado el servo
const int servoPin = 13;  
const int giroCompleto = 1000; // Ajusta el tiempo (en ms) para alcanzar una posición simulada

void setup() {
  myServo.attach(servoPin, 500, 2400);
  Serial.begin(9600);
  Serial.println("Iniciando control de servo de rotación continua...");
}

void loop() {
  // Detener el servo (simulación de "posición" 0 grados)
  int i=11.25;
  while (i<180){
    Serial.print("Girando a: ");
    Serial.println(i);
    myServo.write(i);
    delay(1000);
    i=i+11.25;
  }

}