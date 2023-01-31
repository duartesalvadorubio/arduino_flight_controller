#include "Arduino.h"
#include "lecturaMPU6050.h"
#include "lecturaRC.h"
#include "PID.h"
#include "xeral.h"

//<<<--- DECLARACION DE CONSTANTES --->>>

//Limitacion de actuacion
const int maxPIDw[3] = {250, 250, 250};      //Pitch, Roll, Yaw
const int maxPIDang[3] = {250, 250, 250};

//Anti-WindUp
const int awPIDw[3] = {0, 0, 0};       //Pitch, Roll, Yaw
const int awPIDang[3] = {0, 0, 0};


//<<<--- DECLARACION DE VARIABLES --->>>

//Variables externas
extern int modoVoar;

//Variables PID
float errorw[3];    //Pitch, Roll, Yaw
float errorwAnt[3];

float errorAng[2];  //Pitch, Roll
float errorAngAnt[2];

//--
float errorwAntAnt[3];
float errorAngAntAnt[2];
float actuacionPIDwAnt[3];             
float actuacionPIDAngAnt[2];   

float vIntegralw[3];
float vIntegralAng[2];
//--

float Kpw[3] = {2.5, 2.5, 2.5};  
float Kdw[3] = {0.2, 0.2, 0.2};         //Kp de PID w Pitch, Roll, Yawfloat Kdw[3] = {0.5, 0.5, 0.5};       //Kd de PID w Pitch, Roll, Yaw
float Kiw[3] = {0.007, 0.007, 0.005};  //Ki de PID w Pitch, Roll, Yaw

float Kpang[2] = {2.5, 2.5};            //Kp de PID ang Pitch, Roll
float Kdang[2] = {0.2, 0.2};          //Kd de PID ang Pitch, Roll
float Kiang[2] = {0.006, 0.006};      //Ki de PID ang Pitch, Roll

float Iw[3];                        //actuacion I en PID w Pitch, Roll, Yaw
float Dw[3];                        //actuacion D en PID w Pitch, Roll, Yaw

float Iang[2];                      //actuacion I en PID ang Pitch, Roll
float Dang[2];                      //actuacion D en PID ang Pitch, Roll

float actuacionPIDw[3];             //Actuacion PID w (pitch, roll, yaw)
float actuacionPIDang[2];           //Actuacion PID angle (pitch, roll)

//Marcas anteriores
float xiroAnt[3];                   //Valor anterior de grad/s (Pitch, Roll, Yaw)
float angXiroAnt[2];                //Valor anterior de grad (Pitch, Roll)

//Consignas
float consignasPID[3] = {0, 0, 0};  //Consignas de Pitch, Roll, Yaw segundo modo de voo

//--- VARIABLES PID NOVO ---
unsigned long tempoActualw[3], tempoAnteriorw[3], tempoActualAng[2], tempoAnteriorAng[2];
double deltaTempow[3], deltaTempoAng[2];
float sumaErrorw[3], difErrorw[3], sumaErrorAng[2], difErrorAng[2];

//<<<--- DECLARACION DE FUNCIONS --->>>

void visualizaPID() {
Serial.println("--- VISUALIZACION PIDS ---");
Serial.println(" ");

Serial.println("PID ANGLE. PITCH");
Serial.println("Consigna:   " + String(consignas[0]));
Serial.println("Angulo MPU: " + String(angXiro[0]));
Serial.println("Error:      " + String(errorAng[0]));
Serial.println("Valor I:    " + String(Iang[0]));
Serial.println("ACTUACION:  " + String(actuacionPIDang[0]));
//Serial.println("Delta T.  : " + String(deltaTempoAng[0]));
Serial.println(" ");

Serial.println("PID ANGLE. ROLL");
Serial.println("Consigna:   " + String(consignas[1]));
Serial.println("Angulo MPU: " + String(angXiro[1]));
Serial.println("Error:      " + String(errorAng[1]));
Serial.println("Valor I:    " + String(Iang[1]));
Serial.println("ACTUACION:  " + String(actuacionPIDang[1]));
//Serial.println("Delta T.  : " + String(deltaTempoAng[1]));
Serial.println(" ");

Serial.println("--- --- --- --- ---");
Serial.println(" ");

Serial.println("PID W. PITCH");
Serial.println("Consigna:   " + String(consignasPID[0]));
Serial.println("V. ang. MPU:" + String(xiro[0]));
Serial.println("Error:      " + String(errorw[0]));
Serial.println("Valor I:    " + String(Iw[0]));
Serial.println("ACTUACION:  " + String(actuacionPIDw[0]));
//Serial.println("Delta T.  : " + String(deltaTempow[0]));
Serial.println(" ");

Serial.println("PID W. ROLL");
Serial.println("Consigna:   " + String(consignasPID[1]));
Serial.println("V. ang. MPU:" + String(xiro[1]));
Serial.println("Error:      " + String(errorw[1]));
Serial.println("Valor I:    " + String(Iw[1]));
Serial.println("ACTUACION:  " + String(actuacionPIDw[1]));
//Serial.println("Delta T.  : " + String(deltaTempow[1]));
Serial.println(" ");

Serial.println("PID W. YAW");
Serial.println("Consigna:   " + String(consignasPID[2]));
Serial.println("V. ang. MPU:" + String(xiro[2]));
Serial.println("Error:      " + String(errorw[2]));
Serial.println("Valor I:    " + String(Iw[2]));
Serial.println("ACTUACION:  " + String(actuacionPIDw[2]));
//Serial.println("Delta T.  : " + String(deltaTempow[1]));
Serial.println(" ");


}

void acroPID() {
  //Selecion de consigna segundo modo de voo
  if (modoVoar == 1) { //MODO ANGLE
    consignasPID[0] = actuacionPIDang[0]; //Consignas do PID previo para Pitch, ROll
    consignasPID[1] = actuacionPIDang[1];
    consignasPID[2] = consignas[2];       //Consignas de grad/seg para Yaw
  }

  else if (modoVoar == 2) { //MODO ACRO
    for (int i = 0; i < 3; i++) {
      consignasPID[i] = consignas[i]; //Consignas de grad/seg para todos os eixos
    }
  }

  //--- CALCULO PID W (ROLL, PITCH, YAW) ---
  
  for (int i = 0; i < 3; i++) {
    
    //Calculo erro
    errorw[i] = consignasPID[i] - xiro[i];
    
    //Calculo parte integral
    Iw[i] = Kiw[i]*errorw[i];
    //Anti Windup
    Iw[i] = constrain(Iw[i], -awPIDw[i], awPIDw[i]);

    //Obteño actuacion
    actuacionPIDw[i] = actuacionPIDwAnt[i] + (Kpw[i] * ( errorw[i] - errorwAnt[i])) + Iw[i] + (Kdw[i]*(errorw[i] + errorwAntAnt[i] - 2*errorwAnt[i]));
    //Limito actuacion
    actuacionPIDw[i] = constrain(actuacionPIDw[i], -maxPIDw[i], maxPIDw[i]);

    //Actualizo marcas
    actuacionPIDwAnt[i] = actuacionPIDw[i];
    errorwAntAnt[i] = errorwAnt[i];
    xiroAnt[i] = xiro[i];
    errorwAnt[i] = errorw[i];
    }
}

void anglePID() {

  //--- CALCULO PID ANG (ROLL, PITCH) ---
  for (int i = 0; i < 2; i++) {
    
    //Calculo erro
    errorAng[i] = consignas[i] - angXiro[i];

     //Calculo parte integral
    Iang[i] = Kiang[i]*errorAng[i];
    //Anti WindUp
    Iang[i] = constrain(Iang[i], -awPIDw[i], awPIDw[i]);

    //Obteño actuacion
    actuacionPIDang[i] = actuacionPIDAngAnt[i] + (Kpang[i] * ( errorAng[i] - errorAngAnt[i])) + Iang[i] + (Kdang[i]*(errorAng[i] + errorAngAntAnt[i] - 2*errorAngAnt[i]));
    //Limito actuacion
    actuacionPIDang[i] = constrain(actuacionPIDang[i], -maxPIDang[i], maxPIDang[i]);

    //Actualizo marcas
    actuacionPIDAngAnt[i] = actuacionPIDang[i];
    errorAngAntAnt[i] = errorAngAnt[i];
    angXiroAnt[i] = angXiro[i];
    errorAngAnt[i] = errorAng[i];
    
  }
}
