#include "Arduino.h"
#include "lecturaRC.h"
#include "xeral.h"
#include "sinaisPWM.h"
#include <EnableInterrupt.h>


//<<<--- DECLARACION DE CONSTANTES--->>>

//Terminais
//const uint8_t pinThrottle = A11, pinRoll = A13, pinPitch = A12, pinYaw = A10;
//const uint8_t pinAux1 = A9, pinAux2 = A8;

#define pinThrottle (A11)    //Pin A11 -> Mega GPIO 86 
#define pinRoll (A13)        //Pin A13 -> Mega GPIO 84
#define pinYaw (A10)         //Pin A10 -> Mega GPIO 87
#define pinPitch (A12)       //Pin A12 -> Mega GPIO 85
#define pinAux1 (A9)        //Pin A9 -> Mega GPIO 88
#define pinAux2 (A8)        //Pin A8 -> Mega GPIO 89

//Valores para mapeo da sinal

const int anchoPulsoPotMax = 1830;
const int anchoPulsoPotMin = 930;
const int anchoPulsoMax = 2000; //Valor teórico
const int anchoPulsoMin = 1000;

const int consigna_w_Roll_max = 60;
const int consigna_w_Roll_min = -60;
const int consigna_w_Pitch_max = -75;
const int consigna_w_Pitch_min = 75;
const int consigna_w_Yaw_max = 60;
const int consigna_w_Yaw_min = -60;


//<<<--- DECLARACION DE VARIABLES --->>>

extern int modoVoar;

//Variables para subrutinas de atencion a interrupcions
volatile long microsUpThrottle;
volatile long anchoPulsoThrottle;

volatile long microsUpRoll;
volatile long anchoPulsoRoll;

volatile long microsUpPitch;
volatile long anchoPulsoPitch;

volatile long microsUpJaw;
volatile long anchoPulsoYaw;

volatile long microsUpAux1;
volatile long anchoPulsoAux1;

volatile long microsUpAux2;
volatile long anchoPulsoAux2;

//Obtencion de consignas RC
float consignas[3]; //Consignas de: PITCH , ROLL , YAW
int consignaAux1;
int consignaAux2;

//Control de tempo
long microsant, tempoms;


//<<<--- DECLARACION DE FUNCIONS --->>>

//--- FUNCIONS DE ATENCION A INTERRUPCION ---
//Estas funcións non son chamadas. Saltan ao ocorrer unha interrupción, actualizando os valores das variables.

void throttleInt() {
  //Evaluo o estado do terminal
  int estadoThrottle = digitalRead(pinThrottle);

  //Se hai cambio no terminal salta interrupcion. Se esta a valor alto comezo a contar, se esta a baixo remata o pulso.
  if (estadoThrottle) {
    microsUpThrottle = micros();
  }
  else {
    anchoPulsoThrottle = micros() - microsUpThrottle;
  }
}

void rollInt() {
  //Evaluo o estado do terminal
  int estadoRoll = digitalRead(pinRoll);

  //Se hai cambio no terminal salta interrupcion. Se esta a valor alto comezo a contar, se esta a baixo remata o pulso.
  if (estadoRoll) {
    microsUpRoll = micros();
  }
  else {
    anchoPulsoRoll = micros() - microsUpRoll;
  }
}

void pitchInt() {
  //Evaluo o estado do terminal
  int estadoPitch = digitalRead(pinPitch);

  //Se hai cambio no terminal salta interrupcion. Se esta a valor alto comezo a contar, se esta a baixo remata o pulso.
  if (estadoPitch) {
    microsUpPitch = micros();
  }
  else {
    anchoPulsoPitch = micros() - microsUpPitch;
  }
}

void yawInt() {
  //Evaluo o estado do terminal
  int estadoJaw = digitalRead(pinYaw);

  //Se hai cambio no terminal salta interrupcion. Se esta a valor alto comezo a contar, se esta a baixo remata o pulso.
  if (estadoJaw) {
    microsUpJaw = micros();
  }
  else {
    anchoPulsoYaw = micros() - microsUpJaw;
  }
}

void aux1Int() {
  //Evaluo o estado do terminal
  int estadoAux1 = digitalRead(pinAux1);

  //Se hai cambio no terminal salta interrupcion. Se esta a valor alto comezo a contar, se esta a baixo remata o pulso.
  if (estadoAux1) {
    microsUpAux1 = micros();
  }
  else {
    anchoPulsoAux1 = micros() - microsUpAux1;
  }
}

void aux2Int() {
  //Evaluo o estado do terminal
  int estadoAux2 = digitalRead(pinAux2);

  //Se hai cambio no terminal salta interrupcion. Se esta a valor alto comezo a contar, se esta a baixo remata o pulso.
  if (estadoAux2) {
    microsUpAux2 = micros();
  }
  else {
    anchoPulsoAux2 = micros() - microsUpAux2;
  }
}

//--- FUNCIONS DO MODULO---

void inicializaRC() {
  //ESTABLEZO I/O
  pinMode(13, OUTPUT);

  //Establezo terminais interrupci�ns.
  pinMode(pinThrottle, INPUT_PULLUP);
  pinMode(pinRoll, INPUT_PULLUP);
  pinMode(pinPitch, INPUT_PULLUP);
  pinMode(pinYaw, INPUT_PULLUP);
  pinMode(pinAux1, INPUT_PULLUP);
  pinMode(pinAux2, INPUT_PULLUP);

  enableInterrupt(pinThrottle, throttleInt, CHANGE);
  enableInterrupt(pinRoll, rollInt, CHANGE);
  enableInterrupt(pinPitch, pitchInt, CHANGE);
  enableInterrupt(pinYaw, yawInt, CHANGE);
  enableInterrupt(pinAux1, aux1Int, CHANGE);
  enableInterrupt(pinAux2, aux2Int , CHANGE);
}

void visualizaRC() {
  Serial.println("--- LECTURA RC ---");
  Serial.println("Ancho pulso:   Throttle: " + String(anchoPulsoThrottle));
  Serial.println("Ancho pulso:      PITCH: " + String(anchoPulsoPitch) + " ROLL: " + String(anchoPulsoRoll) + "  YAW: " + String(anchoPulsoYaw));
  Serial.println("Ancho pulso:       AUX1: " + String(anchoPulsoAux1) + "  AUX2: " + anchoPulsoAux2);
  Serial.println(" ");
  Serial.println("Consignas:        PITCH: " + String(consignas[0]) + "  ROLL:" + String(consignas[1]) + " YAW:" + String(consignas[2]));
  Serial.println("Consignas:         AUX1: " + String(consignaAux1) + "  AUX2: " + String(consignaAux2));
  Serial.println(" ");

}

void procesadoRC() {
  //-- MAPEO DAS SINAIS --
  //O mapeo empregase para pasar de milisegundos de sinal a consigna de velocidade angular w (º/seg)
  //NOTA: A potencia non ten consigna de w, empregase directamente no modulador, non se pasa a º/seg
  //NOTA: O modulador xerará sinais PWM e empregará microsegundos, a potencia pasase de mili a micros.
 
  //PEQUENO MARXE NA REXION CENTRAL DO STICK. Evita imprecisions da emisora e receptor radiocontrol
  if ((anchoPulsoPitch > 1468) or (anchoPulsoPitch < 1450))
    consignas[0] = map(anchoPulsoPitch, anchoPulsoMin, anchoPulsoMax, consigna_w_Pitch_min, consigna_w_Pitch_max);
  else
    consignas[0] = 0;

  if ((anchoPulsoRoll > 1468) or (anchoPulsoRoll < 1450))
    consignas[1] = map(anchoPulsoRoll, anchoPulsoMin, anchoPulsoMax, consigna_w_Roll_min, consigna_w_Roll_max);
  else
    consignas[1] = 0;

  if ((anchoPulsoYaw > 1475) or (anchoPulsoYaw < 1465))
    consignas[2] = map(anchoPulsoYaw, anchoPulsoMin, anchoPulsoMax, consigna_w_Yaw_min, consigna_w_Yaw_max);
  else
    consignas[2] = 0;

  if ((anchoPulsoAux2 > 1000))
    anguloCamara = map(anchoPulsoAux2, anchoPulsoMin, anchoPulsoMax, 0, 90);
  else
    anguloCamara = 0;

  //A evaluación para obter a consigna das canles auxiliares é discreta. Uso if
  if (anchoPulsoAux1 < 1250) {
    consignaAux1 = 0;     //Desarmado
    modoVoar = consignaAux1;
  }

  if (anchoPulsoAux1 > 1250 && anchoPulsoAux1 < 1750) {
    consignaAux1 = 1;     //Modo Angle
    modoVoar = consignaAux1;
  }

  if (anchoPulsoAux1 > 1750) {
    consignaAux1 = 2;     //Modo Acro
    modoVoar = consignaAux1;
  }

}


