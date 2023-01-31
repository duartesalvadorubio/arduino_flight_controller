#include "Arduino.h"
#include "sinaisPWM.h"
#include "PID.h"
#include "lecturaRC.h"
#include "lecturaMPU6050.h"
#include "led.h"


//DECLARACIONS GLOBAIS

int modoVoar;       //0 -> DESARMADO ; 1 -> ANGLE ; 2 -> ACRO
long tempoBucle;     //Contador tempo ciclo
int visualizar[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //0-> Xeral; 1-> Lectura RC ; 2-> Lectura MPU ; 3-> PIDs ; 4-> Xerador PWM ; 
                    //0, 1, 2, 3, 4, 5

float vBat;

//--- TENSION BATERÍA ---

void inicializaCAD(){
  pinMode(A0, INPUT);
}

void mideBateria(){
   
  int valorCAD = analogRead(A0);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  vBat = valorCAD * (12.6 / 940.0); //1023 para 5V, 945 para 4.62 V, acorde ao divisor instalado  
  
  if(vBat < 11){
    toff = 100000; //Se o nivel de batería é moi baixo, o escintileo é máis rápido
  }
  else{
    toff = 1500000;
  }
  
}


//--- FUNCIONS DE VISUALIZACION ---

void inicializaComunicacion(){
  //Inicio visualizacion serial
  for (int i = 0; i < 10 ; i++) {
    if (visualizar[i]) {
      Serial.begin(250000);

      delay(200);
      break;
    }
  }
}


void visualizacionXeral() {
  
  //Visualizacion xeral
  if (visualizar[0]) {
    Serial.println("V. xeral: MODO VOAR: " + String(modoVoar));
    Serial.println("V. xeral: V BATERIA: " + String(vBat));
    Serial.println(" ");
  }

  //Visualizo Modulos
  if (visualizar[1]) visualizaRC();
  if (visualizar[2]) visualizaMPU();
  if (visualizar[3]) visualizaPID();
  if (visualizar[4]) visualizaESC();
  
}


