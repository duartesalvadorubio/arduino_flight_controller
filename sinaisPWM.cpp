#include "sinaisPWM.h"
#include "lecturaMPU6050.h"
#include "lecturaRC.h"
#include "PID.h"
#include "xeral.h"
#include "Arduino.h"

#define vMin (920)
//<<<--- DECLARACION DE CONSTANTES --->>>

const int pinESC[4] = {3, 4, 5, 6};             //Terminais de conexion cos ESC
const int pinServo[2] = {9, 8};

//<<<--- DECLARACION DE VARIABLES --->>>

bool estado[4] = {false, false, false, false};   //Para o xerador de PWM. Indica o estado da saida de cada canle (alto ou baixo)
float accion[4];                                //Para o procesado da accion
float esc[4];                                   //Tempo a manter en nivel alto cada ESC por ciclo de execucion

float consignasServo[2], anguloCamara;
float angulos[2], anchoPulsoServo[2]; //Angulos a xirar en Pitch e Roll para manter a camara nivelada
bool estadoServo[2];
float accionServo[2];

//Contadores temporais
//long tempo1, tempo2;
long tOn, loopTimerESC;

//<<<--- DECLARACION DE FUNCIONES --->>>

void armarMotores() {

  const int tdelay1 = 920;
  const int tfrec = 6000;
  int count = 0;

  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  delay(200);

  // --- PROGRAMA ---
  while ( count < 500) {

    //Inicializa os motores cun PWM baixo para que os ESC non protesten
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);

    delayMicroseconds(tdelay1);

    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);

    delayMicroseconds(tfrec - tdelay1);
    count++;

  }
}


void visualizaESC() {
  Serial.println("--- MODULADOR PWM ---");

  Serial.println("Throttle:     " + String(anchoPulsoThrottle));
  Serial.println("Atuacion ang: " + String(actuacionPIDang[0]) + " ; " + String(actuacionPIDang[1]));
  Serial.println("Atuacion w:   " + String(actuacionPIDw[0]) + " ; " + String(actuacionPIDw[1]) + " ; " + String(actuacionPIDw[2]));
  Serial.println("Tempos. ESC1: " + String(esc[0]) + "  ESC2: " + String(esc[1]) + "  ESC3: " + String(esc[2]) + "  ESC4: " + String(esc[3]));
  Serial.println(" ");
}

void inicializaPWM() {
  for (int i = 0; i < 4; i++) {
    pinMode(pinESC[i], OUTPUT);
  }
  pinMode(pinServo[0], OUTPUT);
  pinMode(pinServo[1], OUTPUT);

}

void procesadoSinaisPWM() {
  //SERVOMOTORES
  //Obteño angulo a xirar en cada eixo
    angulos[0] = anguloCamara - angXiro[0];      //Ao colocar o servo no lado dereito debo invertir a consigna
    angulos[0] = constrain(angulos[0], -90, 90); //Evito sairme de rango se falla algo
    angulos[1] = -angXiro[1] + 3;                //A cifra extra corrixe a pequena desviación que ten a patiña ao encaixar no servomotor.
    angulos[1] = constrain(angulos[1], -90, 90); 
    //Paso angulos a ancho de pulso
    anchoPulsoServo[0] = map(angulos[0], -90, 90, 600, 2400); //Situar o servo en posicion de repouso en 1500us
    anchoPulsoServo[1] = map(angulos[1], -90, 90, 600, 2400);

  //VARIADORES
  if (modoVoar != 0) {

    //En caso de aceleración mínima desactivado control de estabilidade, o drone non despega e non hai marxe de control
    if (anchoPulsoThrottle <= 1150 ) {
      //Parte integral dos PID a 0 para non acumular erro
      for (int i = 0; i < 2 ; i++) {
        Iw[i] = 0;
        Iang[i] = 0;
      }
      Iw[2] = 0; //O vector para PID w ten 3 posicions

      //Consigna para os motores únicamente é a potencia
      for (int i = 0; i < 4; i++) {
        esc[i] = anchoPulsoThrottle;
      }

      //Parar motores se o stick está ao mínimo
      for (int i = 0; i < 4; i++)
        if (esc[i] < 1000) esc[i] = vMin; //Ver se con 950 se paran

    }

    if (anchoPulsoThrottle > 1150) {


      //Modulacion para cada ESC. Modos ANGLE e ACRO
      esc[0] = anchoPulsoThrottle + actuacionPIDw[0] - actuacionPIDw[1] - actuacionPIDw[2];
      esc[1] = anchoPulsoThrottle + actuacionPIDw[0] + actuacionPIDw[1] + actuacionPIDw[2];
      esc[2] = anchoPulsoThrottle - actuacionPIDw[0] + actuacionPIDw[1] - actuacionPIDw[2];
      esc[3] = anchoPulsoThrottle - actuacionPIDw[0] - actuacionPIDw[1] + actuacionPIDw[2];


      //Filtrado consignas excesivas para os motores
      //O filtro baixo evita que algún motor poida deterse totalmente co dron voando
      for (int i = 0; i < 4; i++) {
        if ( esc[i] < 1100) esc[i] = 1100;
      }
    }
  }
  //MODO STOP. MOTORES PARADOS
  else {
    for (int i = 0; i < 4; i++)
      esc[i] = 920;
  }

  //O filtro alto evita calquera consigna erronea superior ao máximo de 2000 us
  for (int i = 0; i < 4; i++) {
    if ( esc[i] > 2000) esc[i] = 2000;
  }


}

void xeradorPWM() {

  //Inicializacions con terminais a nivel alto
  for (int i = 0; i < 4; i++)
    digitalWrite(pinESC[i], HIGH);
  
  for (int i = 0; i < 2; i++)
    digitalWrite(pinServo[i], HIGH);


  //Estado actual de cada saida: Nivel Alto
  for (int i = 0; i < 4; i++)
    estado[i] = true;
  
  for (int i = 0; i < 2; i++)
    estadoServo[i] = true;

  for (int i = 0; i < 4; i++)
    accion[i] = esc[i] + tempoBucle; //Tempo que se debe manter en nivel alto cada ESC
  
  for (int i = 0; i < 2; i++)
    accionServo[i] = anchoPulsoServo[i] + tempoBucle;

  //CONMUTA PWM
  while ( estado[0] || estado[1] || estado[2] || estado[3] || estadoServo[0] || estadoServo[1] == true) {

    for (int i = 0; i < 4; i++) {
      loopTimerESC = micros();
      if (accion[i] <= loopTimerESC) {
        estado[i] = false;
        digitalWrite(pinESC[i], LOW);
      }
    }

    for (int i = 0; i < 2; i++) {
      loopTimerESC = micros();
      if (accionServo[i] <= loopTimerESC) {
        estadoServo[i] = false;
        digitalWrite(pinServo[i], LOW);
      }

  }
}
}
