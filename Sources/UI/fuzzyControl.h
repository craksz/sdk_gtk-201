#ifndef __FUZZYCONTROL_H__
#define __FUZZYCONTROL_H__

//#include "gamepad.h"
//#include "UI/gui.h"
#include <stdio.h>
#include <stdlib.h>


#define NegativeTolerance -10
#define PositiveTolerance 10

#define XGAIN1  0.15
#define XGAIN2  0.8
#define XGAIN3  1.0
#define YGAIN1  0.15
#define YGAIN2  0.8
#define YGAIN3  1.0
#define ZGAIN1  1.0
#define ZGAIN2  1.0
#define ZGAIN3  1.0
#define WGAIN1  1.0
#define WGAIN2  1.0
#define WGAIN3  1.0
#define WCE     0.55
#define YCE     1.38
#define XCE     1.136
#define ZCE     0.142

#define VCONTROL_NAME_VARX          "Posición X"
#define VCONTROL_NAME_VARY          "Posición Y"
#define VCONTROL_NAME_VARZ          "Altitud"
#define VCONTROL_NAME_VARW          "Posición Yaw"

#ifndef heightTop
#define heightTop   700
#endif

#ifndef VIDEO_VERT_WIDTH
#define VIDEO_VERT_WIDTH 176
#endif
#ifndef VIDEO_VERT_HEIGHT
#define VIDEO_VERT_HEIGHT 144
#endif

#define VCONTROL_REFERENCE_VARX     0.0     
#define VCONTROL_REFERENCE_VARY     0.0
#define VCONTROL_REFERENCE_VARZ     heightTop
#define VCONTROL_REFERENCE_VARW     0.0


typedef enum{
	varX=1,
	varY,
	varYaw,
	varZ
} vControlVars;

/*	Estructura para el control difuso	*/
typedef struct {
    
    double ce;
    
	double vin;
	double ref;
	double vout;

	double ke;
	double kr;
	double ku;

	double error;
	double error0;

	char *name;
    
    double * h;
    
}	vControl;

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


vControl* getVControl(vControlVars);

double vControlGetRef(vControlVars theVar);
double vControlGetVin(vControlVars theVar);
double vControlGetVout(vControlVars theVar);


void vControlSetVin(vControlVars theVar,double vin);
void vControlSetRef(vControlVars theVar,double ref);


void vControlInit(void);

fGraph * getFGraph();

void fGraphUpdateRef(double ref);

void fGraphUpdate();

//	Proceso de Control
void fuzzyControl (void);

//	Inicializa las structura de control a cero
void inControl ( vControlVars theVarIndex );

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
void Defuzz( vControl * theVar , double *u );



void inGraph( fGraph *gX );
#endif //__FUZZYCONTROL_H__
