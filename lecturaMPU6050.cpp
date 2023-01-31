#include "lecturaMPU6050.h"
#include "xeral.h"
#include <Wire.h>
#include "Arduino.h"

//NOTA ACLARATORIA: EIXO X -> PITCH ; EIXO Y -> ROLL ; EIXO Z -> YAW

//<<<--- DECLARACION DE VARIABLES --->>>
//Calibracion
int varCal;
float xiroCal[3];   //xiroCal x, xiroCal y, xiroCal z
float aclCal[3];    //aclCal x, aclCal y, aclCal z

//Medicions MPU
float acl[3];       //Valor bruto acelerometro en eixos x, y, z
float xiroRaw[3];   //Valor bruto xiroscopio en eixos x, y, z
float temperatura;

//Externas
float xiro[3];      //Valor procesado xiroscopio para pitch, roll, yaw
//float xiroPitch, xiroYaw, xiroRoll;
float angXiro[2];   //Valor de angulo de xiro para pitch, roll
//float angXiroPitch, angXiroRoll, angXiroYaw;
float angAclPitch, angAclRoll, angAclYaw, angAclTotal;

//Programa
long tempoExecucion, tempoMs, contadorBucle;
int i; //Para bucles

//<<<--- FUNCIONS --->>>

void inicializaMPU() {
    
    //ESTABLECE AS PAUTAS DE FUNCIONAMENTO ESCRIBINDO NOS REXISTROS
    Wire.beginTransmission(direccionMPU);
    Wire.write(0x6B);                           // Rexistro 107 (6B hex). Xesti�n de potencia
    Wire.write(0x00);                           // 00000000 Modo funcionamento est�ndar, reloxo interno 8MHz
    Wire.endTransmission();

    Wire.beginTransmission(direccionMPU);
    Wire.write(0x1B);                           // Rexistro 27 (1B hex). Configuraci�n de xiroscopio
    Wire.write(0x08);                           // 00001000 Rango de 500 g/s
    Wire.endTransmission();

    Wire.beginTransmission(direccionMPU);
    Wire.write(0x1C);                          // Rexistro 28 (1C hex). Configuraci�n de aceler�metro
    Wire.write(0x10);                          // 00010000 Rango de +/- 8g
    Wire.endTransmission();

    Wire.beginTransmission(direccionMPU);
    Wire.write(0x1A);                        // Rexistro 26 (1A hex). Configuraci�n
    Wire.write(0x03);                        // Filtro aceler�metro 44Hz; Filtro xiroscopio a 42Hz
    Wire.endTransmission();

}

void calibraMPU() {
    //CALIBRACI�N INICIAL PARA MINIMIZAR O ERRO DE OFFSET
    
    //CALIBRADO XIROSCOPIO
    for (varCal = 0; varCal < 3000; varCal++) {
        mideMPU();

        //Calculo sumatorios
        for (i = 0; i < 3; i++){
            xiroCal[i] += xiroRaw[i];
        }
        /*xiroXCal += xiroX;
        xiroYCal += xiroY;
        xiroZCal += xiroZ;*/

        delayMicroseconds(1000); //Espera para tomar medidas separadas brevemente
    }
    
    //Calculo promedio
    for (i = 0; i < 3; i++){
        xiroCal[i] = xiroCal[i]/3000;
    }

   
    /*
    xiroCal[0] = xiroCal[0] / 3000;
    xiroCal[1] = xiroCal[1] / 3000;
    xiroCal[3] = xiroCal[2] / 3000;*/

    //CALIBRADO ACELEROMETRO
    for (varCal = 0; varCal < 3000; varCal++) {
        mideMPU();

        //Calculo sumatorios
        for (i = 0; i < 3; i++){
            aclCal[i] += acl[i];
        }
        /*
        aclCal[0] += acl[0];
        aclCal[1] += acl[1];
        aclCal[2] += acl[2];*/
    }
    //Calculo promedio
    for (i = 0; i < 3; i++){
        aclCal[i] = aclCal[i]/3000;
    }/*
    aclCal[0] = aclCal[0] / 3000;
    aclCal[1] = aclCal[1] / 3000;
    aclCal[2] = aclCal[2] / 3000;*/

    //NOTA: Podo apgar unha luz de calibraci�n ao rematar
}

void mideMPU() {
    //TOMARA OS DATOS RAW 

    //Establezo comunicaci�n I2C
    Wire.beginTransmission(direccionMPU);
    Wire.write(0x3B); //Indico onde comezar a leer os rexistros
    Wire.endTransmission();

    //Leo 14 bytes dos rexistros (acelerometro 6 rexistros, temperatura 2 rexistros, xiroscopio 6 rexistros)
    Wire.requestFrom(direccionMPU, 14);

    while (Wire.available() < 14); //Agardo a recibir 14 bytes no buffer (datos desde o rexistro 59 ao 72 do MPU)

    //Tomo medici�ns. Parte alta + parte baixa (desprazamento de bits)
    //NOTA: PAXINA 30 NO DATASHEET
    acl[0] = Wire.read() << 8 | Wire.read();
    acl[1] = Wire.read() << 8 | Wire.read();
    acl[2] = Wire.read() << 8 | Wire.read();
    //Cambiar por un bucle

    temperatura = Wire.read() << 8 | Wire.read();

    xiroRaw[0] = Wire.read() << 8 | Wire.read();
    xiroRaw[1] = Wire.read() << 8 | Wire.read();
    xiroRaw[2] = Wire.read() << 8 | Wire.read();
}

void procesaMPU() {
    //Calibro aceleracion
    for(i = 0; i < 3; i++){
        acl[i] -= aclCal[i];
    }
    /*
    aclX -= aclXCal;
    aclY -= aclYCal;
    aclZ -= aclZCal;
    */
    acl[2] = acl[2] + 4096; //O acelerometro mide a aceleracion terrestre, hai que compensalo

    //Calculo vel. angular
    for(i = 0; i < 3; i++){
        xiro[i] = (xiroRaw[i] - xiroCal[i]) / 65.5 ;//Un valor bruto obtido de 65.5 equivale a un xiro de 1�/s
    }
    /*xiroPitch = (xiroX - xiroXCal) / 65.5; //Un valor bruto obtido de 65.5 equivale a un xiro de 1�/s
    xiroRoll = (xiroY - xiroYCal) / 65.5;
    xiroYaw = (xiroZ - xiroZCal) / 65.5;*/

    //C�lculo �ngulos

    angXiro[0] += xiro[0] * tempoExecucion / 1000; //Angulo xirado = vel. angular * tempo de xiro
    angXiro[1] += xiro[1] * tempoExecucion / 1000;

    angXiro[0] += angXiro[1] * sin((xiroRaw[2] - xiroCal[2]) * tempoExecucion * 0.000000266); // tempoExecucion/1000 /65.5 * PI/180
    angXiro[1] -= angXiro[0] * sin((xiroRaw[2] - xiroCal[2]) * tempoExecucion * 0.000000266);

    angAclTotal = sqrt(pow(acl[0], 2) + pow(acl[1], 2) + pow(acl[2], 2));
    angAclPitch = asin((float)acl[1] / angAclTotal) * 57.2958;
    angAclRoll = asin((float)acl[0] / angAclTotal) * -57.2958;

    angXiro[0] = angXiro[0] * 0.99 + angAclPitch * 0.01; //Filtro para correci�n de erro de drift
    angXiro[1] = angXiro[1] * 0.99 + angAclRoll * 0.01;
}

void visualizaMPU(){
  
    for(int i = 0; i < 3; i++){
        Serial.println("Vel. ang. xiro (eixo " + String(i)+ " ): " + String(xiro[i]) );
    }
    Serial.println(" ");
    Serial.println("Ang. xirado Pitch: " + String(angXiro[0]));
    Serial.println("Ang. xirado ROll : " + String(angXiro[1]));
    Serial.println(" ");
   
}

void lecturaMPU() {
    while (micros() - contadorBucle < 5000); //Tempo de ciclo 5 ms
    tempoExecucion = (micros() - contadorBucle) / 1000;
    contadorBucle = micros();
 
    //Chamadas a funci�ns
    mideMPU();
    procesaMPU();

    //if (visualizar[2]) visualizaMPU();
}
