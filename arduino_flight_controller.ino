#include "xeral.h"
#include "LED.h"
#include "lecturaRC.h"
#include "lecturaMPU6050.h"
#include "PID.h"
#include "sinaisPWM.h"
#include "telemetria.h"

#include "Arduino.h" //Permite utilizar delay(), Serial..., etc.

#define microsCiclo (6000)
#define pinLed (13)

//<<<--- PROGRAMA PRINCIPAL --->>>

void setup(){

  //Inicio elementos
  inicializaRC();
  inicializaMPU();
  calibraMPU();
  inicializaPWM();
  armarMotores();
  inicializaLeds();
  inicializaComunicacion();
  inicializaTelemetria();
  inicializaCAD();
  inicializaZoador();

  //Encendo LED Arduino. Inicializacions listas.
  digitalWrite(pinLed, HIGH);

  tempoBucle = micros();
}

void loop(){

  //Chamo a todos os elementos do programa
  //Lectura receptor e sensores
  procesadoRC();
  lecturaMPU();

  //PIDS
  anglePID();
  acroPID();

  //Procesado sinais ESC
  procesadoSinaisPWM();
  //procesaServo();
  while (micros() - tempoBucle < microsCiclo); //Espera a terminar este ciclo

  //Novo ciclo
  tempoBucle = micros();

  xeradorPWM();

  //Outras funcions
  mideBateria();

  xestionaTelemetria();
  visualizacionXeral();
  
  escintilar();
  procesaLeds();
  //zoar();
}
