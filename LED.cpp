#include "Arduino.h"
#include "xeral.h"
#include "LED.h"

// <<<--- DECLARACION DE CONSTANTES --->>>
//Terminais dos LED
const unsigned int ledAzul = 13, ledVerde = 26, ledAmarelo = 32, ledVermello = 38;
const unsigned int led1 = 44, led2 = 46, led3 = 48, led4 = 50;

// <<<--- DECLARACION DE VARIABLES --->>>
//Variables para funcion de parpadexo
int habLeds = 1, habZoador = 2; //Variable de habilitación //Para empregar na recepción de telemetría
long ton = 50000, toff = 1500000, contTon, contToff;
int escintileo;

//<<<--- FUNCIONS --->>>

void inicializaLeds()
{

  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledAzul, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermello, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
}

void escintilar()
{
  if (escintileo && (micros() - contTon > ton))
  {
    escintileo = 0;
    contToff = micros(); //Comeza a contar o tempo en off
  }
  if (!escintileo && (micros() - contToff > toff))
  {
    escintileo = 1;
    contTon = micros(); //Comeza a contar o tempo en on
  }
}

void ledsPosicionON()
{
  if(habLeds){
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
    }
}
  

void ledsPosicionOFF()
{
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
}

void procesaLeds(){
  
  switch (modoVoar){
    case 0:
      if (escintileo)
        ledsPosicionON();
      else
        ledsPosicionOFF();

      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledVermello, HIGH);
      break;

    case 1:
      if (escintileo)
        ledsPosicionON();
      else
        ledsPosicionOFF();

      digitalWrite(ledAmarelo, LOW);
      digitalWrite(ledVerde, HIGH);
      digitalWrite(ledVermello, LOW);
      break;

    case 2:
      if (escintileo)
        ledsPosicionON();
      else
        ledsPosicionOFF();

      digitalWrite(ledAmarelo, HIGH);
      digitalWrite(ledVerde, LOW);
      digitalWrite(ledVermello, LOW);
      break;

    default:
      break;
  }
  
 
}

//--- ZOADOR ---
void inicializaZoador(){
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH); //Buzzer conectado con lóxica negativa
}

void zoar(){
  if(habZoador == 3){
    if (escintileo)
      digitalWrite(10, LOW); //Loxica negativa
    
    else
      digitalWrite(10, HIGH);

  }
  if(habZoador == 2){
    digitalWrite(10, HIGH);
  }

}