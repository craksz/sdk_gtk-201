#ifndef __FUZZYCONTROL_H__
#define __FUZZYCONTROL_H__

#include "gamepad.h"
#include "UI/gui.h"

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



void inGraph( fGraph *gX );
void fuzzyGraph( fGraph *, IplImage * );

#endif //__FUZZYCONTROL_H__
