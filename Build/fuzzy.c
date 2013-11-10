#include <stdio.h>
#include <highgui.h>
#include <math.h>

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


//	Programa Principal
int main(	){

	double altura;
	double altRef;

	double posX;
	double posY;

	vControl altitud;
	vControl X;
	vControl Y;
	vControl Yaw;

	inControl( &altitud );
	inControl( &X );
	inControl( &Y );
	inControl( &Yaw );

	altitud.vin 	= altura;
	altitud.ref 	= altRef;

	X.vin = posX;
	X.ref = 72.0;

	Y.vin = posY;
	Y.ref = 88.0;

	Yaw.vin = atan2( posY, posX );
	Yaw.ref = 0.0;

	fuzzyControl( &altitud, &X, &Y, &Yaw );

	return 0;
}

//	Funciones
//	Proceso de Control
void fuzzyControl ( vControl *altitud, vControl *X, vControl *Y, vControl *Yaw ){

//	Variables auxiliares
	double fuzzyMem1[5];	// Auxiliar para la fuzzyficación 
	double fuzzyMem2[5];	// Auxiliar para la fuzzyficación
	double vInference[7];			// Auxiliar para la inferencia difusa
	double h[] = {-1,-0.8,-0.5,0,0.5,0.8,1};		// Conjuntos Singletons de salida
	double prevError;
	double Error;
	double Rate;
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Altitud
	char name[] = "altitud";
	altitud->name = name;
	altitud->error 	= altitud->ref - altitud->vin;
	setSkale( altitud, 0.05, 0.1, 1.0 );

	Error = altitud->ke * altitud->error;
	Rate = altitud->kr * ( altitud->error - altitud->error0 );

	altitud->error0 = altitud->error;

	fuzzification ( &Error , fuzzyMem1 );
	fuzzification ( &Rate , fuzzyMem2 );
	fuzzyInferenceAlt( fuzzyMem1, fuzzyMem2, vInference );
	Defuzz( vInference , h , &altitud->ku , &altitud->vout );
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	X
	char nameX[] = "Posicion X";
	X->name 	= nameX;
	X->error 	= X->ref - X->vin;
	setSkale( X, 1.4, 2.0, 1.0 );

	Error = X->ke * X->error;
	Rate = X->kr * ( X->error - X->error0 );

	X->error0 = X->error;

	fuzzification ( &Error , fuzzyMem1 );
	fuzzification ( &Rate , fuzzyMem2 );
	fuzzyInferenceAlt( fuzzyMem1, fuzzyMem2, vInference );
	Defuzz( vInference , h , &X->ku , &X->vout );
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Y
	char nameY[] = "Posicion Y";
	Y->name 	= nameY;
	Y->error 	= Y->ref - Y->vin;
	setSkale( Y, 1.15, 2.0, 1.0 );

	Error = Y->ke * Y->error;
	Rate = Y->kr * ( Y->error - Y->error0 );

	Y->error0 = Y->error;

	fuzzification ( &Error , fuzzyMem1 );
	fuzzification ( &Rate , fuzzyMem2 );
	fuzzyInferenceAlt( fuzzyMem1, fuzzyMem2, vInference );
	Defuzz( vInference , h , &Y->ku , &Y->vout );
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Yaw
	char nameYaw[] = "Posicion Yaw";
	Yaw->name 	= nameYaw;
	Yaw->error 	= Yaw->ref - Yaw->vin;
	setSkale( Yaw, 0.55, 1, 1.0 );

	Error = Yaw->ke * Yaw->error;
	Rate = Yaw->kr * ( Yaw->error - Yaw->error0 );

	Yaw->error0 = Yaw->error;

	fuzzification ( &Error , fuzzyMem1 );
	fuzzification ( &Rate , fuzzyMem2 );
	fuzzyInferenceAlt( fuzzyMem1, fuzzyMem2, vInference );
	Defuzz( vInference , h , &Yaw->ku , &Yaw->vout );
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
}

//	Inicializa las structura de control a cero
void inControl ( vControl *control ){

	control->vin 	= 0.0;
	control->ref 	= 0.0;
	control->vout 	= 0.0;

	control->ke 	= 0.0;
	control->kr 	= 0.0;
	control->ku	= 0.0;

	control->error	= 0.0;
	control->error0 = 0.0;

	//control->name	= name;

}

//	Establece las escalas de Error, Rate para el control de altitud
void setSkale ( vControl *control, double ke, double kr, double ku ){

	control->ke = ke;
	control->kr = kr;
	control->ku = ku;

}

//	Proceso de fusificación
void fuzzification ( double *element , double *fuzzyMem ){

//	Conjvoutto A-2: 	Muy Negativo
	if ( *element < -100 )
		fuzzyMem[0] = 1;
	else if ( *element >= -100 && *element <= -70 )
		ecRecNeg( element, &fuzzyMem[0], -100, -70);
	else
		fuzzyMem[0] = 0;

//	Conjvoutto A-1: 	Negativo
	if ( *element < -80 )
		fuzzyMem[1] = 0;
	else if ( *element >= -80 && *element <= -50 )
		ecRecPos( element, &fuzzyMem[1], -80, -50);
	else if ( *element >= -50 && *element <= -10 )
		ecRecNeg( element, &fuzzyMem[1], -50, -10);
	else
		fuzzyMem[1] = 0;

//	Conjvoutto A0: 	Cero
	if ( *element < -20 )
		fuzzyMem[2] = 0;
	else if ( *element >= -20 && *element <= 0 )
		ecRecPos( element, &fuzzyMem[2], -20, 0);
	else if ( *element >= 0 && *element <= 20 )
		ecRecNeg( element, &fuzzyMem[2], 0, 20);
	else
 		fuzzyMem[2] = 0;

//	Conjvoutto A1: 	Positivo
	if ( *element < 10 )
		fuzzyMem[3] = 0;
	else if ( *element >= 10 && *element <= 50 )
		ecRecPos( element, &fuzzyMem[3], 10, 50);
	else if ( *element >= 50 && *element <= 80 )
		ecRecNeg( element, &fuzzyMem[3], 50, 80);
	else
 		fuzzyMem[3] = 0;

//	Conjvoutto A2:	Muy Positivo
	if ( *element < 70 )
		fuzzyMem[4] = 0;
	else if ( *element >= 70 && *element <= 100 )
		ecRecPos( element, &fuzzyMem[4], 70, 100);
	else
 		fuzzyMem[4] = 1;
	
	printf("\nFuzzy: {%3.1f,%3.1f,%3.1f,%3.1f,%3.1f}",fuzzyMem[0],fuzzyMem[1],fuzzyMem[2],fuzzyMem[3],fuzzyMem[4] );
}

//	Ecuación de la recta con pendiente positiva
void ecRecPos( double *x, double *y , int A, int C ){

	*y = -(*x-A)/(A-C);

}

//	Ecuación de la recta con pendiente negativa
void ecRecNeg( double *x, double *y , int A, int C ){

	*y = (*x-A)/(A-C) + 1;

}

//	Inferencia Difusa para altitud
void fuzzyInferenceAlt( double *fuzzyError, double *fuzzyRate, double *vInference){
//	Reglas Difusas

//	1.-	Si Error es Muy Positivo y Rate es Muy positivo entonces la salida es 	h+2
//	2.-	Si Error es Muy Positivo y Rate es Positivo 	entonces la salida es 	h+3
//	3.-	Si Error es Muy Positivo y Rate es Cero 	entonces la salida es	h+2
//	4.- 	Si Error es Muy Positivo y Rate es Negativo 	entonces la salida es	h+2
//	5.-	Si Error es Muy Positivo y Rate es Muy Negativo entonces la salida es	h+3

//	6.-	Si Error es Positivo y Rate es Muy Positivo 	entonces la salida es	h+1
//	7.-	Si Error es Positivo y Rate es Positivo 	entonces la salida es	h+2
//	8.-	Si Error es Positivo y Rate es Cero 		entonces la salida es	h+1
//	9.-	Si Error es Positivo y Rate es Negativo 	entonces la salida es	h+2
//	10.-	Si Error es Positivo y Rate es Muy Negativo 	entonces la salida es	h+3

//	11.-	Si Error es Cero y Rate es Muy Positivo 	entonces la salida es 	h+1
//	12.-	Si Error es Cero y Rate es Positivo 		entonces la salida es	h0
//	13.-	Si Error es Cero y Rate es Cero 		entonces la salida es	h0
//	14.-	Si Error es Cero y Rate es Negativo 		entonces la salida es	h0
//	15.-	Si Error es Cero y Rate es Muy Negativo 	entonces la salida es	h-1

//	16.-	Si Error es Negativo y Rate es Muy Positivo 	entonces la Salida es	h-3
//	17.-	Si Error es Negativo y Rate es Positivo 	entonces la Salida es	h-2
//	18.-	Si Error es Negativo y Rate es Cero 		entonces la Salida es	h-1
//	19.-	Si Error es Negativo y Rate es Negativo 	entonces la Salida es	h-2
//	20.-	Si Error es Negativo y Rate es Muy Negativo 	entonces la Salida es	h-1

//	21.-	Si Error es Muy Negativo y Rate es Muy Positivo entonces la salida es	h-3
//	22.-	Si Error es Muy Negativo y Rate es Positivo 	entonces la salida es	h-2
//	23.-	Si Error es Muy Negativo y Rate es Cero 	entonces la salida es	h-3
//	24.-	Si Error es Muy Negativo y Rate es Negativo 	entonces la salida es	h-3
//	25.-	Si Error es Muy Negativo y Rate es Muy Negativo entonces la salida es	h-2

	double uz[25];
	
	minimum( &fuzzyError[4], &fuzzyRate[4], &uz[0] );
	minimum( &fuzzyError[4], &fuzzyRate[3], &uz[1] );
	minimum( &fuzzyError[4], &fuzzyRate[2], &uz[2] );
	minimum( &fuzzyError[4], &fuzzyRate[1], &uz[3] );
	minimum( &fuzzyError[4], &fuzzyRate[0], &uz[4] );

	minimum( &fuzzyError[3], &fuzzyRate[4], &uz[5] );
	minimum( &fuzzyError[3], &fuzzyRate[3], &uz[6] );
	minimum( &fuzzyError[3], &fuzzyRate[2], &uz[7] );
	minimum( &fuzzyError[3], &fuzzyRate[1], &uz[8] );
	minimum( &fuzzyError[3], &fuzzyRate[0], &uz[9] );

	minimum( &fuzzyError[2], &fuzzyRate[4], &uz[10] );
	minimum( &fuzzyError[2], &fuzzyRate[3], &uz[11] );
	minimum( &fuzzyError[2], &fuzzyRate[2], &uz[12] );
	minimum( &fuzzyError[2], &fuzzyRate[1], &uz[13] );
	minimum( &fuzzyError[2], &fuzzyRate[0], &uz[14] );

	minimum( &fuzzyError[1], &fuzzyRate[4], &uz[15] );
	minimum( &fuzzyError[1], &fuzzyRate[3], &uz[16] );
	minimum( &fuzzyError[1], &fuzzyRate[2], &uz[17] );
	minimum( &fuzzyError[1], &fuzzyRate[1], &uz[18] );
	minimum( &fuzzyError[1], &fuzzyRate[0], &uz[19] );

	minimum( &fuzzyError[0], &fuzzyRate[4], &uz[20] );
	minimum( &fuzzyError[0], &fuzzyRate[3], &uz[21] );
	minimum( &fuzzyError[0], &fuzzyRate[2], &uz[22] );
	minimum( &fuzzyError[0], &fuzzyRate[1], &uz[23] );
	minimum( &fuzzyError[0], &fuzzyRate[0], &uz[24] );

	vInference[0] = 0.0;
	vInference[1] = 0.0;
	vInference[2] = 0.0;
	vInference[3] = 0.0;
	vInference[4] = 0.0;
	vInference[5] = 0.0;
	vInference[6] = 0.0;

//	Para Salida h-3 	( reglas 16, 21, 23, 24 )
	maximum( &uz[15], &uz[20], &vInference[0] );
	maximum( &uz[22], &uz[23], &vInference[0] );
//	Para Salida h-2 	( reglas 17, 19, 22, 25 )
	maximum( &uz[16], &uz[18], &vInference[1] );
	maximum( &uz[21], &uz[24], &vInference[1] );
//	Para Salida h-1 	( reglas 15, 18, 20 )
	maximum( &uz[14], &uz[17], &vInference[2] );
	maximum( &uz[17], &uz[19], &vInference[2] );
//	Para Salida h0		(reglas 12, 13, 14 )
	maximum( &uz[11], &uz[12], &vInference[3] );
	maximum( &uz[12], &uz[13], &vInference[3] );
//	Para Salida h+1		(reglas 6, 8, 11 )
	maximum( &uz[5], &uz[7], &vInference[4] );
	maximum( &uz[7], &uz[10], &vInference[4] );
//	Para Salida h+2		(reglas 1, 3, 4, 7, 9)
	maximum( &uz[0], &uz[2], &vInference[5] );
	maximum( &uz[3], &uz[6], &vInference[5] );
	maximum( &uz[6], &uz[8], &vInference[5] );
//	Para Salida h+3		(reglas 2, 5, 10 )
	maximum( &uz[1], &uz[4], &vInference[6] );
	maximum( &uz[4], &uz[9], &vInference[6] );

	printf("\nInference: {%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f,%3.1f}",vInference[0],vInference[1],vInference[2],vInference[3],vInference[4],vInference[5],vInference[6] );

}

//	Método de inferencia difusa: Zadeh fuzzy logic AND
void minimum( double *var1 , double *var2 , double *var3 ){

	if( *var1 < *var2 ){
		*var3 = *var1;
	}else
		*var3 = *var2;
}

//	Método de inferencia difusa: Zadeh fuzzy logic OR
void maximum( double *var1, double *var2, double *var3){

	double aux = *var3;

	if ( *var1 >= *var2 && *var1 >= *var3)
		aux = *var1;
	else if ( *var2 > *var1 && *var2 >= *var3)
		aux = *var2;				 
	
	*var3 = aux;
}

//	Defusificación: Linear Defuzzyfier
void Defuzz( double *u , double *h , double *ku , double *vout ){
	double aux = 0.0;
	*vout = 0.0;

	int i;
	for( i = 0; i < 7; i++ ){
		aux += u[i];
		*vout += h[i] * u[i];
	};

	*vout = *ku * *vout;
	if( aux != 0.0 ) 
		*vout = *vout/aux;
	else 
		*vout = 1;

}

