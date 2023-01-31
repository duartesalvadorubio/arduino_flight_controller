//--- DEFINE ---
#define direccionMPU (0x68)

//--- VARIABLES EXTERNAS ---
extern float angXiro[2];    //Indica o ángulo xirado do dron en eixos Pitch, Roll
extern float xiro[3];       //Indica a velocidade angular de rotacion do dron (Pitch, Roll, Yaw)

//--- FUNCIONS ---
void inicializaMPU();
void calibraMPU();
void mideMPU();
void procesaMPU();
void lecturaMPU();
void visualizaMPU();
