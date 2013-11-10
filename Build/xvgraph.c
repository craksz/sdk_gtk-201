#include "highgui.h"

//	Estructura para las gráficas
typedef struct {

	double x;
	double ref;

	int counter;

}	fGraph;

void inGraph( fGraph *gX );
void fuzzyGraph( fGraph *gX, IplImage *graph );

int main(){
	IplImage* xvg = cvCreateImage(cvSize(300,500),IPL_DEPTH_8U,3); 
	fGraph xg;	inGraph( &xg );
	

	cvNamedWindow("altura",CV_WINDOW_AUTOSIZE);
		int i;
		for( i = 0; i < 201; i++ ){
	xg.ref = 1500;
	xg.x = i*10;
	fuzzyGraph( &xg, xvg );
	cvShowImage("altura",xvg);
		}

	cvWaitKey(0);
	cvDestroyAllWindows();
	cvReleaseImage(&xvg);
	
return 0;
}

// Estructura de la gráfica a cero
void inGraph( fGraph *gX ){

	gX->x 		= 0.0;	
	gX->ref 	= 0.0;
	gX->counter 	= 0;

}

// Función para graficar;
void fuzzyGraph( fGraph *gX, IplImage *graph ){

	if(gX->counter == 201){
		gX->counter = 0;
		cvSetZero( graph );
	}
	
	CvPoint ref;
	CvPoint pos;
	CvPoint cero;

	cero.x = gX->counter;
	cero.y = 250;
	cvCircle( graph, cero, 1, CV_RGB( 255, 255, 255), 1, CV_FILLED, 0);

	ref.x = gX->counter;
	ref.y = 250 - gX->ref/10;

	pos.x = gX->counter;
	pos.y = 250 - gX->x/10;

	cvCircle( graph, ref, 1, CV_RGB( 0, 0, 255), 1, CV_FILLED, 0);
	cvCircle( graph, pos, 1, CV_RGB( 255, 255, 0), 1, CV_FILLED, 0);

	gX->counter = gX->counter++;
}
