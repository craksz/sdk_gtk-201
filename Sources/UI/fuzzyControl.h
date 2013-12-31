#ifndef __FUZZYCONTROL_H__
#define __FUZZYCONTROL_H__

#include "gamepad.h"
#include "UI/gui.h"

#define VCONTROL_NAME_VARX          "Posición X"
#define VCONTROL_NAME_VARY          "Posición Y"
#define VCONTROL_NAME_VARZ          "Altitud"
#define VCONTROL_NAME_VARW          "Posición Yaw"

#define VCONTROL_REFERENCE_VARX     0.0     
#define VCONTROL_REFERENCE_VARY     0.0
#define VCONTROL_REFERENCE_VARZ     heightTop
#define VCONTROL_REFERENCE_VARW     0.0

vControl* getVControl(vControlVars);

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
void Defuzz( vControl * theVar , double *u , double *h  );



void inGraph( fGraph *gX );
void fuzzyGraph( fGraph *, IplImage * );

#endif //__FUZZYCONTROL_H__
