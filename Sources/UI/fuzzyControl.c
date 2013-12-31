#include "fuzzyControl.h"

vControl Z;
vControl X;
vControl Y;
vControl Yaw;
fGraph zg;	

fGraph * getFGraph(){
    return &zg;
}


void fGraphUpdateRef(double ref){
    zg.ref=ref;
}

void fGraphUpdate(){
    
    zg.z=vControlGetVin(varZ);
    zg.x=vControlGetVin(varX);
    zg.y=vControlGetVin(varY);
    zg.w=vControlGetVin(varYaw);
}

vControl* getVControl(vControlVars index){
	switch (index){
		case varX:
			return &X; 
		case varY:
			return &Y;
		case varYaw:
			return &Yaw;
		case varZ:
			return &Z;
		default:
        	return NULL;
	}
}


void inGraph( fGraph *gX ){
    
    if(gX==NULL){ printf("\ninGraph null pointer\n\n"); exit(-1); }
	
	gX->x 		= 0.0;	
	gX->y 		= 0.0;
	gX->z 		= 0.0;
	gX->w 		= 0.0;
	gX->ref 	= 0.0;
	gX->counter 	= 0;
	gX->counter2	= 0;

}

// Función para graficar;
void fuzzyGraph( fGraph *gX, IplImage *graph ){

        if(gX==NULL){ printf("\nfuzzyGraph null pointer\n\n"); exit(-1); }
	if(graph==NULL){ printf("\nfuzzyGraph graph null pointer\n\n"); exit(-1); }
	
	//char filename[50];
	int i;

	CvFont font;
	cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX_SMALL, 0.5, 0.5, 0.5, 0, 8);

	if( gX -> counter == 300 || gX -> counter == 0){
		if( gX -> counter == 300 ){
			gX->counter2++;
			/*if(gX->counter2<10)
				sprintf( filename, "./graph/graph0%d.BMP", gX->counter2 );
			else
				sprintf( filename, "./graph/graph%d.BMP", gX->counter2 );
			cvSaveImage( filename, graph, 0 );//*/
		}

		gX->counter = 0;
		cvSetZero( graph );
		
		i = 0;
		for( i = 1; i < 60; i++){
			cvLine( graph, cvPoint(0,i*10-1), cvPoint(599,i*10-1), CV_RGB(20,20,20), 0, 8, 0 );			
		};

		cvLine( graph, cvPoint(0,299), cvPoint(599,299), CV_RGB(255,255,255), 2, 8, 0 );
		cvLine( graph, cvPoint(299,0), cvPoint(299,599), CV_RGB(255,255,255), 2, 8, 0 );

		cvLine( graph, cvPoint(0,149), cvPoint(599,149), CV_RGB(0,0,255), 0, 8, 0 );
		cvLine( graph, cvPoint(300,449), cvPoint(599,449), CV_RGB(0,0,255), 0, 8, 0 );

		cvPutText( graph, "Control en X", cvPoint(4,10), &font, CV_RGB(0,255,0) );
		cvPutText( graph, "Control en Y", cvPoint(304,10), &font, CV_RGB(0,255,0) );
		cvPutText( graph, "Control en Z", cvPoint(4,310), &font, CV_RGB(0,255,0) );
		cvPutText( graph, "Control en Yaw", cvPoint(304,310), &font, CV_RGB(0,255,0) );

	};
	
	CvPoint ref;
	CvPoint pos;

////////////////	X

	pos.x = gX->counter;
	pos.y = cvRound( 149 - gX->x );

	cvCircle( graph, pos, 0, CV_RGB(255,255,0), 0, 8, 0 );
	
////////////////	Y

	pos.x = 300 + gX->counter;
	pos.y = cvRound( 149 - gX->y );

	cvCircle( graph, pos, 0, CV_RGB(255,255,0), 0, 8, 0 );

////////////////	Z

	ref.x = gX->counter;
	ref.y = cvRound( 599 - gX->ref/10 );

	pos.x = gX->counter;
	pos.y = cvRound( 599 - gX->z/10 );

	cvCircle( graph, ref, 0, CV_RGB( 200, 0, 0), 0, 8, 0);
	cvCircle( graph, pos, 0, CV_RGB(255,255,0), 0, 8, 0);

////////////////	Yaw

	pos.x = 300 + gX->counter;
	pos.y = cvRound( 449 - gX->w );
	//pos.y = cvRound( 449 - ( 360 * atan2( gX->y , gX->x ) )/(2*PI)/1.5 );

	cvCircle( graph, pos, 0, CV_RGB(255,255,0), 0, 8, 0 );

////////////////
	gX->counter++;
    
    gui_t *gui=get_gui();
    if(gui->fuzzyControlGraphWidgetReq==1){	
        GdkPixbuf * pixbuf;	
  		pixbuf = gdk_pixbuf_new_from_data((const guchar*)graph->imageData,
                    GDK_COLORSPACE_RGB,
                    FALSE,  
                    8,     
                    graph->width,    
                    graph->height,     
                    graph->widthStep, 
                    NULL,    
                    NULL);
                              
        gtk_image_set_from_pixbuf(GTK_IMAGE(gui->fuzzyImage), pixbuf);
    }//*/
}

void fuzzyControlPerVar(vControl *theVar){
    
//	Variables auxiliares
	double fuzzyMem1[5];	// Auxiliar para la fuzzyficación 
	double fuzzyMem2[5];	// Auxiliar para la fuzzyficación
	double vInference[7];			// Auxiliar para la inferencia difusa
	double h[] = {-1,-0.8,-0.5,0,0.5,0.8,1};		// Conjuntos Singletons de salida
	//double prevError;
	double Error;
	double Rate;
    
	theVar->error 	= theVar->ref - theVar->vin;
	//setSkale( altitud, 0.08, 0.9, 1.0 );

	Error = theVar->ke * theVar->error;
	Rate = theVar->kr * ( theVar->error - theVar->error0 );

	theVar->error0 = theVar->error;

	fuzzification ( &Error , fuzzyMem1 );
	fuzzification ( &Rate , fuzzyMem2 );
	fuzzyInferenceAlt( fuzzyMem1, fuzzyMem2, vInference );
	Defuzz( theVar, vInference , h);
}

void fuzzyControl (){
    
    fuzzyControlPerVar(&Z);
    fuzzyControlPerVar(&X);
    fuzzyControlPerVar(&Y);
    fuzzyControlPerVar(&Yaw);
    /*
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Altitud
	char name[] = "altitud";
	altitud->name = name;
	altitud->error 	= altitud->ref - altitud->vin;
	//setSkale( altitud, 0.08, 0.9, 1.0 );
	setSkale( altitud, ZGAIN1, ZGAIN2, ZGAIN3 );

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
	//setSkale( X, 2.0, 3.5, 1.0 );
	setSkale( X, XGAIN1, XGAIN2, XGAIN3 );

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
	//setSkale( Y, 2.0, 3.5, 1.0 );
	setSkale(Y , YGAIN1, YGAIN2, YGAIN3 );

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
	//setSkale( Yaw, 1.5, 3.0, 1.0 );
	setSkale( Yaw, WGAIN1, WGAIN2, WGAIN3 );

	Error = Yaw->ke * Yaw->error;
	Rate = Yaw->kr * ( Yaw->error - Yaw->error0 );

	Yaw->error0 = Yaw->error;

	fuzzification ( &Error , fuzzyMem1 );
	fuzzification ( &Rate , fuzzyMem2 );
	fuzzyInferenceAlt( fuzzyMem1, fuzzyMem2, vInference );
	Defuzz( vInference , h , &Yaw->ku , &Yaw->vout );//*/
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
}

void inControlName(vControlVars theVarIndex){
    vControl *theVar=getVControl(theVarIndex);
    switch (theVarIndex){
        case varX:
            theVar->name=VCONTROL_NAME_VARX;
            setSkale( theVar, XGAIN1, XGAIN2, XGAIN3 );
            return;
        case varY:
            theVar->name=VCONTROL_NAME_VARY;
            setSkale( theVar, YGAIN1, YGAIN2, YGAIN3 );
            return;
        case varZ:
            theVar->name=VCONTROL_NAME_VARZ;
            setSkale( theVar, ZGAIN1, ZGAIN2, ZGAIN3 );
            return;
        case varYaw:
            theVar->name=VCONTROL_NAME_VARW;
            setSkale( theVar, WGAIN1, WGAIN2, WGAIN3 );
            return;
        default:
            return;
    }
}

//	Inicializa las structura de control a cero
void inControl ( vControlVars theVarIndex ){
    
    vControl * control=getVControl(theVarIndex);

	if(control==NULL){ printf("\ninControl null pointer\n\n"); exit(-1); }
	control->vin 	= 0.0;
	control->ref 	= 0.0;
	control->vout 	= 0.0;

	control->ke 	= 0.0;
	control->kr 	= 0.0;
	control->ku	= 0.0;

	control->error	= 0.0;
	control->error0 = 0.0;
    
    inControlName(theVarIndex);

	//control->name	= name;

}

void vControlInit(){
    
	inGraph( &zg );
	

	inControl( varZ );
	inControl( varX );
	inControl( varY );
	inControl( varYaw );
    vControlSetRef(varZ,VCONTROL_REFERENCE_VARZ);
    vControlSetRef(varX,VCONTROL_REFERENCE_VARX);
    vControlSetRef(varY,VCONTROL_REFERENCE_VARY);
    vControlSetRef(varYaw,VCONTROL_REFERENCE_VARW);
    fGraphUpdateRef(vControlGetRef(varZ));
}

//	Establece las escalas de Error, Rate para el control de altitud
void setSkale ( vControl *control, double ke, double kr, double ku ){

	if(control==NULL){ printf("\nsetSkale null pointer\n\n"); exit(-1); }
	control->ke = ke;
	control->kr = kr;
	control->ku = ku;

}

//	Proceso de fusificación
void fuzzification ( double *element , double *fuzzyMem ){

	if(element==NULL){ printf("\nfuzzification null pointer\n\n"); exit(-1); }
	if(fuzzyMem==NULL){ printf("\nfuzzification fuzzyMem null pointer\n\n"); exit(-1); }
        
//	conjunto A-2: 	Muy Negativo
	if ( *element < -100 )
		fuzzyMem[0] = 1;
	else if ( *element >= -100 && *element <= -70 )
		ecRecNeg( element, &fuzzyMem[0], -100, -70);
	else
		fuzzyMem[0] = 0;

//	conjunto A-1: 	Negativo
	if ( *element < -80 )
		fuzzyMem[1] = 0;
	else if ( *element >= -80 && *element <= -50 )
		ecRecPos( element, &fuzzyMem[1], -80, -50);
	else if ( *element >= -50 && *element <= NegativeTolerance )
		ecRecNeg( element, &fuzzyMem[1], -50, NegativeTolerance );
	else
		fuzzyMem[1] = 0;

//	conjunto A0: 	Cero
	if ( *element < -20 )
		fuzzyMem[2] = 0;
	else if ( *element >= -20 && *element <= 0 )
		ecRecPos( element, &fuzzyMem[2], -20, 0);
	else if ( *element >= 0 && *element <= 20 )
		ecRecNeg( element, &fuzzyMem[2], 0, 20);
	else
 		fuzzyMem[2] = 0;

//	conjunto A1: 	Positivo
	if ( *element < PositiveTolerance )
		fuzzyMem[3] = 0;
	else if ( *element >= PositiveTolerance && *element <= 50 )
		ecRecPos( element, &fuzzyMem[3], 10, 50);
	else if ( *element >= 50 && *element <= 80 )
		ecRecNeg( element, &fuzzyMem[3], 50, 80);
	else
 		fuzzyMem[3] = 0;

//	conjunto A2:	Muy Positivo
	if ( *element < 70 )
		fuzzyMem[4] = 0;
	else if ( *element >= 70 && *element <= 100 )
		ecRecPos( element, &fuzzyMem[4], 70, 100);
	else
 		fuzzyMem[4] = 1;

}

//	Ecuación de la recta con pendiente positiva
void ecRecPos( double *x, double *y , int A, int C ){
    
	if(x==NULL){ printf("\necRecPos x null pointer\n\n"); exit(-1); }
	if(y==NULL){ printf("\necRecPos y null pointer\n\n"); exit(-1); }
	*y = -(*x-A)/(A-C);

}

//	Ecuación de la recta con pendiente negativa
void ecRecNeg( double *x, double *y , int A, int C ){

	if(x==NULL){ printf("\necRecNeg x null pointer\n\n"); exit(-1); }
	if(y==NULL){ printf("\necRecNeg y null pointer\n\n"); exit(-1); }
	*y = (*x-A)/(A-C) + 1;

}

//	Inferencia Difusa para altitud
void fuzzyInferenceAlt( double *fuzzyError, double *fuzzyRate, double *vInference){
//	Reglas Difusas

	if(fuzzyError==NULL){ printf("\nfuzzyError null pointer\n\n"); exit(-1); }
	if(fuzzyRate==NULL){ printf("\nfuzzyError null pointer\n\n"); exit(-1); }
	if(vInference==NULL){ printf("\nfuzzyError null pointer\n\n"); exit(-1); }
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

	vInference[0] = 0;
	vInference[1] = 0;
	vInference[2] = 0;
	vInference[3] = 0;
	vInference[4] = 0;
	vInference[5] = 0;
	vInference[6] = 0;

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

}

//	Método de inferencia difusa: Zadeh fuzzy logic AND
void minimum( double *var1 , double *var2 , double *var3 ){

	if(var1==NULL||var2==NULL||var3==NULL){ printf("\nminimum null pointer\n\n"); exit(-1); }
	if( *var1 < *var2 ){
		*var3 = *var1;
	}else
		*var3 = *var2;
}

//	Método de inferencia difusa: Zadeh fuzzy logic OR
void maximum( double *var1, double *var2, double *var3){

	if(var1==NULL||var2==NULL||var3==NULL){ printf("\nmaximum null pointer\n\n"); exit(-1); }
	double aux = *var3;

	if ( *var1 >= *var2 && *var1 >= *var3)
		aux = *var1;
	else if ( *var2 > *var1 && *var2 >= *var3)
		aux = *var2;				 
	
	*var3 = aux;
}

//	Defusificación: Linear Defuzzyfier
void Defuzz( vControl * theVar, double *u , double *h){
	double aux = 0.0;
	theVar->vout = 0.0;

	int i;
	for( i = 0; i < 7; i++ ){
		aux += u[i];
		theVar->vout += h[i] * u[i];
	};

	theVar->vout = theVar->ku * theVar->vout;
	if( aux != 0.0 ) 
		theVar->vout = theVar->vout/aux;
	else 
		theVar->vout = 1;
}

