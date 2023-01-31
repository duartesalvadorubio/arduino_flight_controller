//--- VARIABLES EXTERNAS ---

extern float consignas[3];
extern int consignaAux1;
extern int consignaAux2;

extern volatile long anchoPulsoThrottle;
extern volatile long anchoPulsoRoll;
extern volatile long anchoPulsoPitch;
extern volatile long anchoPulsoYaw;
extern volatile long anchoPulsoAux1;
extern volatile long anchoPulsoAux2;
//--- FUNCIONS ---

void procesadoRC();
void inicializaRC();
void visualizaRC();
