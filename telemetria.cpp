#include "Arduino.h"
#include "telemetria.h"
#include "LED.h"
#include "lecturaMPU6050.h"
#include "xeral.h"

//<<<--- DECLARACION DE VARIABLES --->>>
long millisAnt;
int vez;
int habTelemetria = 1;

//<<<--- DECLARACION DE FUNCIONS --->>>

void enviaTelemetria(){
  
    //TRAMA
    String mensaxe = ("/" + String(modoVoar) + "/" + String(angXiro[0]) + "/" + String(angXiro[1]) + "/" + String(vBat) + "/");
    Serial3.print(mensaxe); //Utiliza porto serie 3 de Arduino Mega, conectado a modulo HC12
    Serial3.write('\n');    //Separa mensaxes
    //NOTA: Se inmediatamente despois do envio se fai unha lectura, hai problemas para leer
  
}

void recibeTelemetria(){
  
    char valor; 
    if(Serial3.available()) {
      valor = Serial3.read();
      int valorInt = int(valor - 48); //Resta valor ASCII
      if (valorInt == 0 || valorInt == 1)
        habLeds = valorInt;
      if (valorInt == 2 || valorInt == 3)
        habZoador = valorInt;
    }
  
}

void xestionaTelemetria(){
    if(habTelemetria){

        if(millis() - millisAnt > 100){ //Valor de tempo que evita interferencias na app Python (un valor menor da fallos)
            millisAnt = millis();
            if (vez == 0){
                enviaTelemetria();
                vez = 1;
            }

            if (vez == 1){
                recibeTelemetria();
                vez = 0;
            }
        }
    }

}

void inicializaTelemetria(){

  //Inicio porto serie 3
  if(habTelemetria)
    Serial3.begin(9600);
}

