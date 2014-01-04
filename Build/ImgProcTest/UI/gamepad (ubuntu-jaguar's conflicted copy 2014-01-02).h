#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_

#include "cv.h"
#define C_RESULT        int
#define C_FAIL      -1
#define C_OK        0
#define PI	3.141596
#define MAX_NAME_LENGTH 	255
#define bool_t  int32_t
#define FALSE   0
#define TRUE   0

#define VP_SUCCESS         (0)
#define VP_FAILURE            (!VP_SUCCESS)

#define VP_SUCCEEDED(a)	(((a) & 0xffff) == VP_SUCCESS)
#define VP_FAILED(a)	(((a) & 0xffff) != VP_SUCCESS)

	#define SUCCEED	VP_SUCCEEDED
	#define FAILED VP_FAILED	



typedef struct _input_device_t {
  char name[MAX_NAME_LENGTH];

  C_RESULT (*init)(void);
  C_RESULT (*update)(void);
  C_RESULT (*shutdown)(void);
} input_device_t;

#define switchTop 5
#define XGAIN1 1.8 
#define XGAIN2 0.5
#define XGAIN3 0.1
#define YGAIN1 1.8
#define YGAIN2 0.5
#define YGAIN3 0.1
#define ZGAIN1 0.8
#define ZGAIN2 3.0
#define ZGAIN3 1.0
#define WGAIN1 1.1
#define WGAIN2 3.3
#define WGAIN3 1.0
#define YAWTOLERANCE 10

#define VCONTROL_X          1
#define VCONTROL_Y          2
#define VCONTROL_Z          3
#define VCONTROL_W          4

typedef enum {
  PAD_X,
  PAD_Y
} PAD_AXIS;

typedef enum {
  PAD_AG = 0,
  PAD_AB,
  PAD_AD,
  PAD_AH,
  PAD_L1,
  PAD_R1,
  PAD_L2,
  PAD_R2,
  PAD_SELECT,
  PAD_START,
  PAD_NUM_BUTTONS
} PAD_BUTTONS;


typedef enum{
	varX=1,
	varY,
	varYaw,
	varZ
} vControlVars;

typedef enum{
	MANUAL_PHI=1,
	MANUAL_THETA,
	MANUAL_YAW,
    MANUAL_GAZ
} ManualVars;

typedef struct {

	double x;
	double y;
	double z;
	double w;

	double ref;

	int counter;
	int counter2;

	char *name;

}	fGraph;


typedef struct{
	float phi;
	float theta;
	float gaz;
	float yaw;
}Manual;

IplImage *getImage();
Manual *getManual();
void clearManual();
float getManualVariable(int index);

void vControlUpdate(vControlVars theVar,double vin);
double getVControlVout(int index);

extern input_device_t fpad;

C_RESULT open_fpad(void);
C_RESULT update_fpad(void);
C_RESULT close_fpad(void);

/*	Estructura para el control difuso	*/
typedef struct {

	double vin;
	double ref;
	double vout;

	double ke;
	double kr;
	double ku;

	double error;
	double error0;

	char *name;

}	vControl;

vControl* getVControl(int);

//	Proceso de Control
void fuzzyControl ( vControl *altitud, vControl *X, vControl *Y, vControl *Yaw );

//	Inicializa las structura de control a cero
void inControl ( vControl *control );

//	Establece las escalas de Error, Rate para el control de altitud
void setSkale ( vControl *control, double ke, double kr, double ku );

//	Proceso de fusificación
void fuzzification ( double *element , double *fuzzyMem );

//	Ecuación de la recta con pendiente positiva
void ecRecPos( double *x, double *y , int A, int C );

//	Ecuación de la recta con pendiente negativa
void ecRecNeg( double *x, double *y , int A, int C );

//	Inferencia Difusa para altitud
void fuzzyInferenceAlt( double *fuzzyError, double *fuzzyRate, double *vInference);

//	Método de inferencia difusa: Zadeh fuzzy logic AND
void minimum( double *var1 , double *var2 , double *var3 );

//	Método de inferencia difusa: Zadeh fuzzy logic OR
void maximum( double *var1, double *var2, double *var3);

//	Defusificación: Linear Defuzzyfier
void Defuzz( double *u , double *h , double *ku , double *vout );

void bringItOn(void);



void inGraph( fGraph *gX );
void fuzzyGraph( fGraph *, IplImage * );


#endif // _GAMEPAD_H_
