/**
 *  \brief    gamepad handling implementation
 *  \author   Sylvain Gaeremynck <sylvain.gaeremynck@parrot.fr>
 *  \version  1.0
 *  \date     04/06/2007
 *  \warning  Subject to completion
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#include <linux/joystick.h>

#include <ardrone_api.h>
#include <VP_Os/vp_os_print.h>
#include "gamepad.h"
#include "UI/gui.h"


#define heightTop 700
#define heightTestMode 0
#define heightTestModeTop 500
#define heightTestModeH1 1500
#define heightTestModeH2 1000


#define NegativeTolerance -10
#define PositiveTolerance 10

typedef struct {
  int32_t bus;
  int32_t vendor;
  int32_t product;
  int32_t version;
  char    name[MAX_NAME_LENGTH];
  char    handlers[MAX_NAME_LENGTH];
} device_t;

extern int32_t MiscVar[];

static C_RESULT add_device(device_t* device, const int32_t id);

static C_RESULT parse_proc_input_devices(FILE* f, const int32_t id);

/*input_device_t gamepad = {
  "Gamepad",
  open_gamepad,
  update_gamepad,
  close_gamepad
};*/

static int32_t joy_dev = 0;

/*input_device_t radioGP = {
  "GreatPlanes",
  open_radioGP,
  update_radioGP,
  close_radioGP
};/*/


/*input_device_t ps3pad = {
  "PS3Gamepad",
  open_ps3pad,
  update_ps3pad,
  close_ps3pad
};//*/

input_device_t fpad = {
  "fuzzyPad",
  open_fpad,
  update_fpad,
  close_fpad
};

///////////////////////////////
//  FuzzyGP input functions  //
//////////////////////////////

Manual manualControl;
IplImage* zvg;// = cvCreateImage(cvSize(300,500),IPL_DEPTH_8U,3); 
fGraph zg;	
FILE *datosG;
	

Manual* getManual(){
	return &manualControl;
};
IplImage* getImage(){
	return zvg;
};

void clearManual(){
	manualControl.theta=0;
	manualControl.phi=0;
	manualControl.gaz=0;
	manualControl.yaw=0;
}


float getManualVariable(int index){
    switch(index){
        case MANUAL_PHI:
            return manualControl.phi;
        case MANUAL_THETA:
            return manualControl.theta;
        case MANUAL_GAZ:
            return manualControl.gaz;
        case MANUAL_YAW:
            return manualControl.yaw;
        default:
            return 0;
    }
}

        vControl Z;
	vControl X;
	vControl Y;
	vControl Yaw;

vControl* getVControl(int index){
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

double getVControlVout(int index){
    switch(index){
        case varX:
			return X.vout;
		case varY:
			return Y.vout;
		case varYaw:
			return Yaw.vout;
		case varZ:
			return Z.vout;
		default:
            return 0;
    }
}
//static char theString[500];

void vControlUpdate(vControlVars theVar,double vin){
    vControl * buffControl=getVControl(theVar);
    buffControl->vin=vin;
}

	
C_RESULT open_fpad(void){
	
	//double dG[5];
	char filename[100];
	sprintf(filename,
					"gnuplot/grafica-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%d.dat",
					XGAIN1,XGAIN2,XGAIN3,YGAIN1,YGAIN2,YGAIN3,ZGAIN1,ZGAIN2,ZGAIN3,WGAIN1,WGAIN2,WGAIN3,switchTop);
	datosG = fopen( filename, "w");
	fprintf( datosG, "#	Proyecto: Control de un Quadrotor por VA\n");
	fprintf( datosG, "#	Datos para la gráfica en GNUplot.\n\n");
	fprintf( datosG, "#	Error X	    	Error Y	 	Altura		Alt. Ref	Yaw\n");	

	zvg = cvCreateImage(cvSize(600,600),IPL_DEPTH_8U,3); 
	inGraph( &zg );
	
	inControl( &Z );
	inControl( &X );
	inControl( &Y );
	inControl( &Yaw );
	Z.ref=heightTop;
	X.ref = 0.0;
	Y.ref = 0.0;
	Yaw.ref = 0.0;
	zg.ref=Z.ref;
	
	/*Z.name="Z";
	X.name="X";
	Y.name="Y";
	Y.name="Y";*/
	
	//definicion
	return C_OK;
}

C_RESULT update_fpad(void){

	/*FuzzyControl( &X , 0.0 , h1 );
	FuzzyControl( &Y , 0.0 , h2 );
	FuzzyControl( &Yaw , 0.0 , h3 );
	FuzzyControl( &Z , 0.0 , h4 );//*/
	//printf("%d\n",fcx);
	//dispControl(&Z);
	/*ardrone_at_set_progress_cmd( 1,
                                    //roll/(float)(stick1LR-center_x1)/32767.0f,
                                    //pitch/(float)(stick1UD-center_y1)/32767.0f,
                                    //gaz/-(float)(stick2UD-center_x2)/32767.0f,
                                    //yaw/(float)(stick2LR-center_y2)/32767.0f );*/
                                    //printf("++%0.2f++\n",Z.vout);
	if(heightTestMode==0){
	  if(manualControl.phi!=0||manualControl.theta!=0||manualControl.gaz!=0||manualControl.yaw!=0){
			ardrone_at_set_progress_cmd( 1,manualControl.phi,manualControl.theta,manualControl.gaz,manualControl.yaw);
			//ardrone_at_set_led_animation(FIRE,10,1);
		}
		else{
			bringItOn();
		}
	}
	else{
		
		static int timer=0;
		static int theFlag=0;
		if(timer<heightTestModeTop){
			if(theFlag%2==0){
				Z.ref=heightTestModeH1;
				zg.ref=Z.ref;
			}
			else{
				Z.ref=heightTestModeH2;
				zg.ref=Z.ref;
			}
			timer++;
			//printf("%d!\n",timer);
		}
		else {
			printf("CHANGE to %0.2f !!!\n\n",Z.ref);
			theFlag++;
			timer=0;
		}
		bringItOn();
	}
	return C_OK;
}

C_RESULT close_fpad(void){
	return C_OK;
}

void bringItOn(){

  		//static GdkPixbuf *pixbuf = NULL;
  		
		  //gui_t *gui = get_gui();
			//static int counter001=0;
			zg.z=Z.vin;
			zg.x=X.vin;
			zg.y=Y.vin;
			zg.w=Yaw.vin;
			
			fprintf( datosG, "%d\t%lf\t%lf\t%lf\t%lf\t%lf\n", zg.counter+300*zg.counter2, zg.x, zg.y, zg.z, zg.ref, zg.w );

			fuzzyGraph( &zg, zvg );
    	
			fuzzyControl(&Z,&X,&Y,&Yaw);
			//printf(">>>%5.2f",fabs(Z.vin-Z.ref));
			//if(fabs(Z.vin-Z.ref)<200||Z.vin>Z.ref){
				//ardrone_at_set_progress_cmd( 0,-X.vout,Y.vout,Z.vout,Yaw.vout);
			/*}
			else
				ardrone_at_set_progress_cmd( 0,0,0,Z.vout,0);//*/
			/*if(fabs(X.vin-X.ref)<YAWTOLERANCE&&fabs(Y.vin-Y.ref)<YAWTOLERANCE){
				ardrone_at_set_progress_cmd( 1,-X.vout,Y.vout,Z.vout,0);
			}
			else{
				//ardrone_at_reset_com_watchdog();//*/
				if(Z.vout<0){
					Z.vout*=0.125;
				}
			if(X.vout!=0||Y.vout!=0){
				ardrone_at_set_progress_cmd( 1,X.vout,-Y.vout,Z.vout,0);
				//counter001++;
			}
			else{
				//counter001=0;
				ardrone_at_set_progress_cmd( 0,0,0,Z.vout,0);
				//sprintf(theString,"X:\t%0.3f\nY:\t%0.3f\nZ:\t%0.3f\n",-X.vout,Y.vout,Z.vout);
				//gtk_label_set_text((GtkLabel*)gui->textBox,theString);
				//printf("%s\n",theString);
				//ardrone_at_set_led_animation(1,2,1);
			}
			//}
			//printf("%2.5f\n",-X.vout);
			//printf("-");
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

void fuzzyControl ( vControl *altitud, vControl *X, vControl *Y, vControl *Yaw ){

        if(altitud==NULL){ printf("\nfuzzyControl h null pointer\n\n"); exit(-1); }
	if(X==NULL){ printf("\nfuzzyControl x null pointer\n\n"); exit(-1); }
	if(Y==NULL){ printf("\nfuzzyControl y null pointer\n\n"); exit(-1); }
	if(Yaw==NULL){ printf("\nfuzzyControl yaw null pointer\n\n"); exit(-1); }
	
//	Variables auxiliares
	double fuzzyMem1[5];	// Auxiliar para la fuzzyficación 
	double fuzzyMem2[5];	// Auxiliar para la fuzzyficación
	double vInference[7];			// Auxiliar para la inferencia difusa
	double h[] = {-1,-0.8,-0.5,0,0.5,0.8,1};		// Conjuntos Singletons de salida
	//double prevError;
	double Error;
	double Rate;
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
	setSkale( X, 2.0, 3.5, 1.0 );
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
	Defuzz( vInference , h , &Yaw->ku , &Yaw->vout );
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
}

//	Inicializa las structura de control a cero
void inControl ( vControl *control ){

	if(control==NULL){ printf("\ninControl null pointer\n\n"); exit(-1); }
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






static int32_t make_id(device_t* device)
{
  return ( (device->vendor << 16) & 0xffff0000) | (device->product & 0xffff);
}

static C_RESULT add_device(device_t* device, const int32_t id_wanted)
{
  int32_t id = make_id(device);

  if( id_wanted == GAMEPAD_LOGICTECH_ID && id == id_wanted)
  {
		PRINT("Input device %s found\n", device->name);
    strncpy(gamepad.name, device->handlers, MAX_NAME_LENGTH);
    return C_OK;
	}

	if(id_wanted == RADIO_GP_ID && id==id_wanted)
	{
		PRINT("Input device %s found\n", device->name);
    strncpy(radioGP.name, device->handlers, MAX_NAME_LENGTH);
    return C_OK;
  }

    if(id_wanted == GAMEPAD_PLAYSTATION3_ID && id==id_wanted)
	{
		PRINT("PS3 : Input device %s found\n", device->name);
        strncpy(ps3pad.name, device->handlers, MAX_NAME_LENGTH);
        return C_OK;
  }

  return C_FAIL;
}





/** simple /proc/bus/input/devices generic LL(1) parser **/

#define KW_MAX_LEN 64

typedef enum {
  KW_BUS,
  KW_VENDOR,
  KW_PRODUCT,
  KW_VERSION,
  KW_NAME,
  KW_HANDLERS,
  KW_MAX
} keyword_t;

typedef enum {
  INT,
  STRING,
} value_type_t;

typedef struct {
  const char*   name;
  value_type_t  value_type;
  int32_t       value_offset;
} kw_tab_entry_t;

static int current_c;
static int next_c; // look ahead buffer

static device_t current_device;

static const int separators[] = { ' ',  ':', '=', '\"', '\n' };
static const int quote = '\"';
static const int eol = '\n';

static kw_tab_entry_t kw_tab[] = {
  [KW_BUS]      = {  "Bus",      INT,    offsetof(device_t, bus)       },
  [KW_VENDOR]   = {  "Vendor",   INT,    offsetof(device_t, vendor)    },
  [KW_PRODUCT]  = {  "Product",  INT,    offsetof(device_t, product)   },
  [KW_VERSION]  = {  "Version",  INT,    offsetof(device_t, version)   },
  [KW_NAME]     = {  "Name",     STRING, offsetof(device_t, name)      },
  [KW_HANDLERS] = {  "Handlers", STRING, offsetof(device_t, handlers)  }
};

static const char* handler_names[] = {
  "js0",
  "js1",
  "js2",
  "js3",
  0
};

static bool_t is_separator(int c)
{
  int32_t i;
  bool_t found = FALSE;

  for( i = 0; i < sizeof separators && !found; i++ )
  {
    found = ( c == separators[i] );
  }

  return found;
}

static bool_t is_quote(int c)
{
  return c == quote;
}

static bool_t is_eol(int c)
{
  return c == eol;
}

static C_RESULT fetch_char(FILE* f)
{
  C_RESULT res = C_FAIL;

  current_c = next_c;

  if( !feof(f) )
  {
    next_c = fgetc(f);
    res = C_OK;
  }

  // PRINT("current_c = %c, next_c = %c\n", current_c, next_c );

  return res;
}

static C_RESULT parse_string(FILE* f, char* str, int32_t maxlen)
{
  int32_t i = 0;
  bool_t is_quoted = is_quote(current_c);

  if( is_quoted )
  {
    while( SUCCEED(fetch_char(f)) && ! ( is_separator(current_c) && is_quote(current_c) ) )  {
      str[i] = current_c;
      i++;
    }
  }
  else
  {
    while( SUCCEED(fetch_char(f)) && !is_separator(current_c) )  {
      str[i] = current_c;
      i++;
    }
  }

  str[i] = '\0';
  // PRINT("parse_string: %s\n", str);

  return is_eol( current_c ) ? C_FAIL : C_OK;
}

static C_RESULT parse_int(FILE* f, int32_t* i)
{
  C_RESULT res = C_OK;
  int value;

  *i = 0;

  while( !is_separator(next_c) && SUCCEED(fetch_char(f)) && res == C_OK )  {
    value = current_c - '0';

    if (value > 9 || value < 0)
    {
      value = current_c - 'a' + 10;
      res = (value > 0xF || value < 0xa) ? C_FAIL : C_OK;
    }

    *i *= 16;
    *i += value;
  }

  return res;
}

static C_RESULT skip_line(FILE* f)
{
  while( !is_eol(next_c) && SUCCEED(fetch_char(f)) );

  return C_OK;
}

static C_RESULT match_keyword( const char* keyword, keyword_t* kw )
{
  int32_t i;
  C_RESULT res = C_FAIL;

  for( i = 0; i < KW_MAX && res != C_OK; i++ )
  {
    res = ( strcmp( keyword, kw_tab[i].name ) == 0 ) ? C_OK : C_FAIL;
  }

  *kw = i-1;

  return res;
}

static C_RESULT match_handler( void )
{
  int32_t i = 0;
  bool_t found = FALSE;

  while( !found && handler_names[i] != 0 )
  {
    found = strcmp( (char*)((char*)&current_device + kw_tab[KW_HANDLERS].value_offset), handler_names[i] ) == 0;

    i ++;
  }

	if(found)
	{
		strcpy(current_device.handlers, handler_names[i-1]);
	}

  return found ? C_OK : C_FAIL;
}

static C_RESULT parse_keyword( FILE* f, keyword_t kw )
{
  C_RESULT res = C_OK;

  while( is_separator(next_c) && SUCCEED(fetch_char(f)) );

  switch( kw_tab[kw].value_type ) {
    case INT:
      parse_int( f, (int32_t*)((char*)&current_device + kw_tab[kw].value_offset) );
      //PRINT("%s = %x\n", kw_tab[kw].name, *(int32_t*)((char*)&current_device + kw_tab[kw].value_offset) );
      break;

    case STRING:
      parse_string( f, (char*)((char*)&current_device + kw_tab[kw].value_offset), KW_MAX_LEN );
      //PRINT("%s = %s\n", kw_tab[kw].name, (char*)((char*)&current_device + kw_tab[kw].value_offset) );
      break;

    default:
      res = C_FAIL;
      break;
  }

  return res;
}

static C_RESULT parse_I(FILE* f)
{
  char keyword[KW_MAX_LEN];

  while( SUCCEED(fetch_char(f)) && is_separator(next_c) );

  while( !is_eol(next_c) ) {
    keyword_t kw;

    parse_string( f, keyword, KW_MAX_LEN );
    if( SUCCEED( match_keyword( keyword, &kw ) ) )
    {
      parse_keyword( f, kw );
    }
  }

  return C_OK;
}

static C_RESULT parse_N(FILE* f)
{
  char keyword[KW_MAX_LEN];

  while( SUCCEED(fetch_char(f)) && is_separator(next_c) );

  while( !is_eol(next_c) ) {
    keyword_t kw;

    parse_string( f, keyword, KW_MAX_LEN );
    if( SUCCEED( match_keyword( keyword, &kw ) ) )
    {
      parse_keyword( f, kw );
    }
  }


  return C_OK;
}

static C_RESULT parse_H(FILE* f)
{
  C_RESULT res = C_FAIL;
  char keyword[KW_MAX_LEN];

  while( SUCCEED(fetch_char(f)) && is_separator(next_c) );

  while( !is_eol(next_c) ) {
    parse_string( f, keyword, KW_MAX_LEN );
    if( strcmp( keyword, kw_tab[KW_HANDLERS].name ) == 0 )
    {
      while( FAILED(res) && SUCCEED( parse_string(f,
                                                  (char*)((char*)&current_device + kw_tab[KW_HANDLERS].value_offset),
                                                  KW_MAX_LEN ) ) )
      {
        res = match_handler();
      }
    }
  }

  return res;
}

static C_RESULT end_device(const int32_t id)
{
  C_RESULT res = C_FAIL;
  res=add_device(&current_device, id);
  vp_os_memset( &current_device, 0, sizeof(device_t) );

  return res;
}

static C_RESULT parse_proc_input_devices(FILE* f, const int32_t id)
{
  C_RESULT res = C_FAIL;

  next_c = '\0';
  vp_os_memset( &current_device, 0, sizeof(device_t) );

  while( res != C_OK && SUCCEED( fetch_char(f) ) )
  {
    switch( next_c )
    {
      case 'I': parse_I(f); break;
      case 'N': parse_N(f); break;
      case 'H': if( SUCCEED( parse_H(f) ) ) res = end_device(id); break;
      case 'P':
      case 'S':
      case 'B':
      default: skip_line(f); break;
    }
  }

  return res;
}
