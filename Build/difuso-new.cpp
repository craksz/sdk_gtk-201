#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <math.h>

using namespace std;

/*	Estructura para el control difuso	*/
struct Control{

	float Ref;
	float x0;
	float xin;
	float vout;

	float ke;
	float kr;
	float ku;

	float error;
	float rate;

	void condi( ){
		Ref = 0.0;
		x0 = 0.0;
		vout = 0.0; 
	};

	void gains( float a , float b , float c ){
		ke = a;
		kr = b;
		ku = c;
	};	

	void varin( float &x1 ) {
		error = ke * ( Ref - x1 );
		rate = kr * ( x0 - x1 );
		xin = x0;
		printf("\n\n**%0.2f**\n**%0.2f**\n**%0.2f**\n**%0.2f**\n\n",kr,x0,x1,rate);
		x0 = x1;
	};
};

/*	Variables Globales	*/
int sets[4];	//	Para la Fusificación
float fuzzy_E[7];
float fuzzy_R[7];

float h1[] = {-1500,-1000,-700,-400,-100,0,100,400,700,1000,1500};	//	Conjunto de Salida 1
float h2[] = {-1500,-1000,-700,-400,-100,0,100,400,700,1000,1500};	//	Conjunto de Salida 2
float h3[] = {-1500,-1000,-700,-400,-100,0,100,400,700,1000,1500};	//	Conjunto de Salida 3
float h4[] = {-1500,-1000,-700,-400,-100,0,100,400,700,1000,1500};	//	Conjunto de Salida 4

float H[11][9];		//	Inferencia Difusa
float inferencia[11];

//	Proceso del Control Difuso
void FuzzyControl( Control & , float & , float * );
//	Impresión de los datos de Control
void dispControl( Control & , char );
//	Ecuación de la recta
void rect_ec( int  , int  , int  , int  , float * , float * );
//	Mínimo entre dos valores
void minimo( float * , float * , float * );
//	Máximo entre variables
void maximo( float * );
//	Limpia Matriz H
void clean_matH( );
//	Limpia Vectores
void clean_vec( float * , int );
//	Imprime Matriz H
void disp_matH( );
//	Imprime Vectores
void disp_vec( float * , int );

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		/*	Fuzzificación		*/
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Fuzzy_Mem( float * , float * );
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		/*	Inferencia Difuzza	*/
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Fuzzy_inf( float * , float * , float * );
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		/*	Defuzzificación		*/
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void Defuzz( float * , float * , float * , float *);
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	
/*			Programa Principal			*/
//
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
int main( ){
//=================================================================
/*		Configuración del Control Difuso		*/
//=================================================================
//	Definición de los arreglos de tipo Control	
	Control X;
	Control Y;
	Control Z;
	Control Yaw;

	Control *ptrX = &X;
	Control *ptrY = &Y;
	Control *ptrZ = &Z;
	Control *ptrYaw = &Yaw;

//	Se establecen las condiciones iniciales (variables de entrada a 0)
	ptrX->condi( );
	ptrY->condi( );
	ptrZ->condi( );
	ptrYaw->condi( );

//	Se establecen las ganancias; en este orden: Ke, kr, Ku.
	ptrX->gains( 100.0 , 150.0 , 1.0/1500 );
	ptrY->gains( 80.0 , 160.0 , 1.0/1500 );
	ptrZ->gains( 100.0 , 1.0 , 1.0/1500 );
	ptrYaw->gains( 4500.0 , 9000.0 , 1.0/1500 );

//	Se establecen las Referencias
	ptrX->Ref = 88.0;	//	Referencias fijas: Ref X y Ref Y 
	ptrY->Ref = 72.0;
	ptrZ->Ref = 500.0;	//	Referencias variables: Ref Z y Ref Yaw	
	ptrYaw->Ref = 0;
//====================================================================
//====================================================================
//********************************************************************
//********************************************************************
//							Control X
//********************************************************************
	float prueba1 = 140;
	float prueba2 = -180;
	float prueba3 = 215;
	float angulo = atan( ( prueba2 - ptrY->Ref )/( prueba1 - ptrX->Ref ) );
	cout<<"angulo:\t"<<angulo*180<<endl;
/*	FuzzyControl( X , prueba1 , h1 );
	dispControl( X , 'X' );
//********************************************************************
//********************************************************************
//							Control Y
//********************************************************************
	FuzzyControl( Y , prueba2 , h2 );
	dispControl( Y , 'Y' );
//********************************************************************
//********************************************************************
//							Control Yaw
//********************************************************************
	FuzzyControl( Yaw , angulo , h3 );
	dispControl( Yaw , 'w' );//*/
//********************************************************************
//********************************************************************
//							Control Z
//********************************************************************
	FuzzyControl( Z , prueba3 , h4 );
	dispControl( Z , 'Z' );
//********************************************************************
//********************************************************************
	return 0;
}

//	Función de Fuzzificación
void Fuzzy_Mem( float *var , float *fuzzy_var ){
	clean_vec( fuzzy_var, 7);
//	Selección de cunjuntos positivos o negativos
	if ( *var < 0) {
		*var =	-*var;
		sets[0] = 3;
		sets[1] = 2;
		sets[2] = 1;
		sets[3] = 0;
	}else {
		sets[0] = 3;
		sets[1] = 4;
		sets[2] = 5;
		sets[3] = 6;
	};
//	Conjunto A0
	if ( ( *var >= 0 ) && ( *var < 500) ){	
		fuzzy_var[ sets[0] ] = 1;
	}else if ( ( *var >= 500 ) && ( *var <= 1500) ){ 
		rect_ec( 500, 1, 1500, 0, var , &fuzzy_var[ sets[0] ] );
	}else 	fuzzy_var[ sets[0] ] = 0;
//	Conjunto A1||A-1
	if ( ( *var >= 500 ) && ( *var < 1500 ) ){
		rect_ec( 500, 0, 1500, 1, var , &fuzzy_var[ sets[1] ] );
	}else if ( ( *var >= 1500 ) && ( *var < 2500 ) ){
		fuzzy_var[ sets[1] ] = 1;
	}else if ( ( *var >= 2500 ) && ( *var <= 3500 ) ){
		rect_ec( 2500, 1, 3500, 0, var , &fuzzy_var[ sets[1] ] );
	}else	fuzzy_var[ sets[1] ] = 0;
//	Conjunto A2||A-2
	if ( ( *var >= 2500 ) && ( *var < 3500 ) ){
		rect_ec( 2500, 0, 3500, 1, var , &fuzzy_var[ sets[2] ] );
	}else if ( ( *var >= 3500 ) && ( *var < 4500 ) ){
		fuzzy_var[ sets[2] ] = 1;
	}else if ( ( *var >= 4500 ) && ( *var <= 5500 ) ){
		rect_ec( 4500, 1, 5500, 0, var , &fuzzy_var[ sets[2] ] );
	}else	fuzzy_var[ sets[2] ] = 0;
//	Conjunto A3||A-3
	if ( ( *var < 4500 ) ){
		fuzzy_var[ sets[3] ] = 0;
	}else if ( ( *var >= 4500 ) && ( *var < 5500 ) ){	
		rect_ec( 4500, 0, 5500, 1, var , &fuzzy_var[ sets[3] ] );
	}else 	fuzzy_var[ sets[3] ] = 1;
}

//	Inferencia Difusa
void Fuzzy_inf( float *fuzzy_E, float *fuzzy_R, float *inf_var){

//	Limpia variables
	clean_vec( inf_var, 11);
	clean_matH;

//	Reglas Difusas
//	fuzzy logic AND
//	1.-	Si E(n) es A[3] AND R(n) es A[3] entonces u(n) = C[10]
	minimo( &fuzzy_E[6] , &fuzzy_R[6] , &H[10][0] );
//	2.-	Si E(n) es A[3] AND R(n) es A[2] entonces u(n) = C[10]
	minimo( &fuzzy_E[6] , &fuzzy_R[5] , &H[10][1] );
//	3.-	Si E(n) es A[3] AND R(n) es A[1] entonces u(n) = C[9]
	minimo( &fuzzy_E[6] , &fuzzy_R[4] , &H[9][0] );
//	4.- 	Si E(n) es A[3] AND R(n) es A[0] entonces u(n) = C[7]
	minimo( &fuzzy_E[6] , &fuzzy_R[3] , &H[7][0] );
//	5.-	Si E(n) es A[3] AND R(n) es A[-1] entonces u(n) = C[9]
	minimo( &fuzzy_E[6] , &fuzzy_R[2] , &H[9][1] );
//	6.-	Si E(n) es A[3] AND R(n) es A[-2] entonces u(n) = C[10]
	minimo( &fuzzy_E[6] , &fuzzy_R[1] , &H[10][2] );
//	7.-	Si E(n) es A[3] AND R(n) es A[-3] entonces u(n) = C[10]
	minimo( &fuzzy_E[6] , &fuzzy_R[0] , &H[10][3] );

//	8.-	Si E(n) es A[2] AND R(n) es A[3] entonces u(n) = C[9]
	minimo( &fuzzy_E[5] , &fuzzy_R[6] , &H[9][2] );
//	9.-	Si E(n) es A[2] AND R(n) es A[2] entonces u(n) = C[8]
	minimo( &fuzzy_E[5] , &fuzzy_R[5] , &H[8][0] );
//	10.-	Si E(n) es A[2] AND R(n) es A[1] entonces u(n) = C[7]
	minimo( &fuzzy_E[5] , &fuzzy_R[4] , &H[7][1] );
//	11.-	Si E(n) es A[2] AND R(n) es A[0] entonces u(n) = C[6]
	minimo( &fuzzy_E[5] , &fuzzy_R[3] , &H[6][0] );
//	12.-	Si E(n) es A[2] AND R(n) es A[-3] entonces u(n) = C[7]
	minimo( &fuzzy_E[5] , &fuzzy_R[2] , &H[7][2] );
//	13.-	Si E(n) es A[2] AND R(n) es A[-2] entonces u(n) = C[8]
	minimo( &fuzzy_E[5] , &fuzzy_R[1] , &H[8][1] );
//	14.-	Si E(n) es A[2] AND R(n) es A[-1] entonces u(n) = C[9]
	minimo( &fuzzy_E[5] , &fuzzy_R[0] , &H[9][3] );

//	15.-	Si E(n) es A[1] AND R(n) es A[3] entonces u(n) = C[7]
	minimo( &fuzzy_E[4] , &fuzzy_R[6] , &H[7][2] );
//	16.-	Si E(n) es A[1] AND R(n) es A[2] entonces u(n) = C[6]
	minimo( &fuzzy_E[4] , &fuzzy_R[5] , &H[6][1] );
//	17.-	Si E(n) es A[1] AND R(n) es A[1] entonces u(n) = C[6]
	minimo( &fuzzy_E[4] , &fuzzy_R[4] , &H[6][2] );
//	18.-	Si E(n) es A[1] AND R(n) es A[0] entonces u(n) = C[5]
	minimo( &fuzzy_E[4] , &fuzzy_R[3] , &H[5][0] );
//	19.-	Si E(n) es A[1] AND R(n) es A[-1] entonces u(n) = C[6]
	minimo( &fuzzy_E[4] , &fuzzy_R[2] , &H[6][3] );
//	20.-	Si E(n) es A[1] AND R(n) es A[-2] entonces u(n) = C[6]
	minimo( &fuzzy_E[4] , &fuzzy_R[1] , &H[6][4] );
//	21.-	Si E(n) es A[1] AND R(n) es A[-3] entonces u(n) = C[7]
	minimo( &fuzzy_E[4] , &fuzzy_R[0] , &H[6][4] );

//	22.-	Si E(n) es A[0] AND R(n) es A[3] entonces u(n) = C[5]
	minimo( &fuzzy_E[3] , &fuzzy_R[6] , &H[5][1] );
//	23.-	Si E(n) es A[0] AND R(n) es A[2] entonces u(n) = C[5]
	minimo( &fuzzy_E[3] , &fuzzy_R[5] , &H[5][2] );
//	24.-	Si E(n) es A[0] AND R(n) es A[1] entonces u(n) = C[5]
	minimo( &fuzzy_E[3] , &fuzzy_R[4] , &H[5][3] );
//	25.-	Si E(n) es A[0] AND R(n) es A[0] entonces u(n) = C[5]
	minimo( &fuzzy_E[3] , &fuzzy_R[3] , &H[5][4] );
//	26.-	Si E(n) es A[0] AND R(n) es A[-1] entonces u(n) = C[5]
	minimo( &fuzzy_E[3] , &fuzzy_R[2] , &H[5][5] );
//	27.-	Si E(n) es A[0] AND R(n) es A[-2] entonces u(n) = C[5]
	minimo( &fuzzy_E[3] , &fuzzy_R[1] , &H[5][6] );
//	28.-	Si E(n) es A[0] AND R(n) es A[-3] entonces u(n) = C[5]
	minimo( &fuzzy_E[3] , &fuzzy_R[0] , &H[5][7] );

//	29.-	Si E(n) es A[-1] AND R(n) es A[3] entonces u(n) = C[3]
	minimo( &fuzzy_E[2] , &fuzzy_R[1] , &H[3][0] );
//	30.-	Si E(n) es A[-1] AND R(n) es A[2] entonces u(n) = C[4]
	minimo( &fuzzy_E[2] , &fuzzy_R[1] , &H[4][0] );
//	31.-	Si E(n) es A[-1] AND R(n) es A[1] entonces u(n) = C[4]
	minimo( &fuzzy_E[2] , &fuzzy_R[1] , &H[4][1] );
//	32.-	Si E(n) es A[-1] AND R(n) es A[0] entonces u(n) = C[5]
	minimo( &fuzzy_E[2] , &fuzzy_R[1] , &H[5][8] );
//	33.-	Si E(n) es A[-1] AND R(n) es A[-1] entonces u(n) = C[4]
	minimo( &fuzzy_E[2] , &fuzzy_R[1] , &H[4][2] );
//	34.-	Si E(n) es A[-1] AND R(n) es A[-2] entonces u(n) = C[4]
	minimo( &fuzzy_E[2] , &fuzzy_R[1] , &H[4][3] );
//	35.-	Si E(n) es A[-1] AND R(n) es A[-3] entonces u(n) = C[3]
	minimo( &fuzzy_E[2] , &fuzzy_R[0] , &H[3][1] );

//	36.-	Si E(n) es A[-2] AND R(n) es A[3] entonces u(n) = C[1]
	minimo( &fuzzy_E[1] , &fuzzy_R[6] , &H[1][0] );
//	37.-	Si E(n) es A[-2] AND R(n) es A[2] entonces u(n) = C[2]
	minimo( &fuzzy_E[1] , &fuzzy_R[5] , &H[2][0] );
//	38.-	Si E(n) es A[-2] AND R(n) es A[1] entonces u(n) = C[3]
	minimo( &fuzzy_E[1] , &fuzzy_R[4] , &H[3][2] );
//	39.-	Si E(n) es A[-2] AND R(n) es A[0] entonces u(n) = C[4]
	minimo( &fuzzy_E[1] , &fuzzy_R[3] , &H[4][5] );
//	40.-	Si E(n) es A[-2] AND R(n) es A[-1] entonces u(n) = C[3]
	minimo( &fuzzy_E[1] , &fuzzy_R[2] , &H[3][3] );
//	41.-	Si E(n) es A[-2] AND R(n) es A[-2] entonces u(n) = C[2]
	minimo( &fuzzy_E[1] , &fuzzy_R[1] , &H[2][1] );
//	42.-	Si E(n) es A[-2] AND R(n) es A[-3] entonces u(n) = C[1]
	minimo( &fuzzy_E[1] , &fuzzy_R[0] , &H[1][1] );

//	43.-	Si E(n) es A[-3] AND R(n) es A[3] entonces u(n) = C[0]
	minimo( &fuzzy_E[0] , &fuzzy_R[6] , &H[0][0] );
//	44.-	Si E(n) es A[-3] AND R(n) es A[2] entonces u(n) = C[0]
	minimo( &fuzzy_E[0] , &fuzzy_R[5] , &H[0][1] );
//	45.-	Si E(n) es A[-3] AND R(n) es A[1] entonces u(n) = C[1]
	minimo( &fuzzy_E[0] , &fuzzy_R[4] , &H[1][2] );
//	46.-	Si E(n) es A[-3] AND R(n) es A[0] entonces u(n) = C[3]
	minimo( &fuzzy_E[0] , &fuzzy_R[3] , &H[3][4] );
//	47.-	Si E(n) es A[-3] AND R(n) es A[-1] entonces u(n) = C[1]
	minimo( &fuzzy_E[0] , &fuzzy_R[2] , &H[1][3] );
//	48.-	Si E(n) es A[-3] AND R(n) es A[-2] entonces u(n) = C[0]
	minimo( &fuzzy_E[0] , &fuzzy_R[1] , &H[0][2] );
//	49.-	Si E(n) es A[-3] AND R(n) es A[-3] entonces u(n) = C[0]
	minimo( &fuzzy_E[0] , &fuzzy_R[0] , &H[0][3] );

//	fuzzy logic OR
	maximo( inf_var );
}
//	Defuzzificación.
void Defuzz( float *u , float *h , float *ku , float *Un ){
	float aux = 0.0;
	*Un = 0.0;

	for( int i = 0; i < 11; i++ ){
		aux += u[i];
		*Un += h[i] * u[i];
	};

	printf("++%0.2f++\n++%0.2f++\n\n",*Un,h[7]);
	*Un = *ku * *Un;
	if( *Un != 0.0 ) *Un = *Un/aux;
}
//	Proceso del Control Difuso
void FuzzyControl( Control &Difuso , float &vin , float *h ){

	Difuso.varin( vin );
	Fuzzy_Mem( &Difuso.error , fuzzy_E );
	Fuzzy_Mem( &Difuso.rate , fuzzy_R);
	Fuzzy_inf( fuzzy_E , fuzzy_R , inferencia );
	Defuzz( inferencia , h , &Difuso.ku , &Difuso.vout );
}
//	Impresión de los datos de Control
void dispControl( Control &Difuso, char tipo){

	cout<<"ccccccccccccccccccccccccccccccccccccccccccccccccccc"<<endl;
	cout<<"Referencia "<<tipo<<":\t"<<Difuso.Ref<<endl;
	cout<<"Posición Inicial:\t"<<Difuso.xin<<endl;
	cout<<"Posición Actual:\t"<<Difuso.x0<<endl;
	cout<<"Velocidad de Salida:\t"<<Difuso.vout<<endl;
}
//	Ecuación de la recta
void rect_ec( int A, int B, int C, int D, float *x, float *y ){

	*y = (B-D)*(*x-A)/(A-C) + B;
}
//	Mínimo entre dos valores
void minimo( float *var1 , float *var2 , float *var3 ){
	if( *var1 < *var2 ){
		*var3 = *var1;
	}else
		*var3 = *var2;
}
//	Máximo entre variables
void maximo( float *y ){

	for( int i = 0; i < 11; i++ ){
		y[i] = H[i][0];
		for( int j = 1; j < 9; j++)
			if( y[i] < H[i][j] ) y[i] = H[i][j];
	};
}
//	Limpia Matriz H
void clean_matH( ){
	for( int i = 0; i < 11; i++)	
		for( int j = 0; j < 9; j++)
			H[i][j] = 0;
}
//	Limpia Vectores
void clean_vec( float *A , int k){
	for( int i = 0; i < k; i++)
		A[i] = 0.0;
}
//	Imprime Matriz H
void disp_matH( ){
	for( int i = 0; i < 11; i++){	
		for( int j = 0; j < 9; j++)
			cout<<"\t"<<H[i][j];
		cout<<endl;
	};
}
//	Imprime Vectores
void disp_vec( float *A , int k){
	for( int i = 0; i < k; i++)
		cout<<"\t"<<A[i];
	cout<<endl;
}
