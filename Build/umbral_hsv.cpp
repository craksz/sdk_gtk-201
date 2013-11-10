#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <math.h>
#include "ttiofiles.h"

using namespace std;

void dhist( CvHistogram* histograma1, CvHistogram* histograma2, IplImage* graphist1, IplImage* graphist2 );
int fast_otsu(CvHistogram *histograma);

int main( /*int argc, char **argv*/ ) {
//============================================================================================================
//============================================================================================================
//					VARIABLES
//============================================================================================================
//============================================================================================================

	IplImage* graphist_s = cvCreateImage(cvSize(310,256),IPL_DEPTH_8U,3);	// Imagen para el histograma.
	IplImage* graphist_v = cvCreateImage(cvSize(310,256),IPL_DEPTH_8U,3);	// Imagen para el histograma.

	IplImage *src = cvLoadImage( "crash.jpg" );
	assert(src!=NULL);
	IplImage *dst = cvCreateImage( cvSize( src->width, src->height), IPL_DEPTH_8U, 3 );
	IplImage *h = cvCreateImage( cvSize( src->width, src->height), IPL_DEPTH_8U, 1 );
	IplImage *s = cvCreateImage( cvSize( src->width, src->height), IPL_DEPTH_8U, 1 );
	IplImage *v = cvCreateImage( cvSize( src->width, src->height), IPL_DEPTH_8U, 1 );

	int umbral_s, umbral_v;

	cvCvtColor( src , dst , CV_BGR2HSV );
	cvSplit( dst , h , s , v , 0 ); //	método que separa la imagen por canal

//============================================================================================================
//============================================================================================================
//					HISTOGRAMA
//============================================================================================================
//============================================================================================================

	int sizes = 256;
	float ranges_arr[] = {0,255}; //imágenes en gris tienen rango de valores de 0 a 255
	float *ranges[] = {ranges_arr};

	CvHistogram* histograma_s = cvCreateHist( 1 , &sizes , CV_HIST_ARRAY , ranges , 1 );
	CvHistogram* histograma_v = cvCreateHist( 1 , &sizes , CV_HIST_ARRAY , ranges , 1 );

	cvCalcHist( &s, histograma_s, 0, NULL );
	cvCalcHist( &v, histograma_v, 0, NULL );

	cvNormalizeHist( histograma_s, 300.0);
	cvNormalizeHist( histograma_v, 300.0);

	dhist( histograma_s, histograma_v, graphist_s, graphist_v );

//============================================================================================================
//============================================================================================================
//					UMBRALADO
//============================================================================================================
//============================================================================================================

	umbral_s = fast_otsu( histograma_s);
	umbral_v = fast_otsu( histograma_v);

//============================================================================================================
//============================================================================================================
//					PANTALLA
//============================================================================================================
//============================================================================================================

	cvNamedWindow( "OpenCv" );
	cvNamedWindow( "Histograma Saturation" );
	cvNamedWindow( "Histograma Value" );
	cvMoveWindow("OpenCv",0,0);
	cvMoveWindow("Histograma Saturation",500,400);
	cvMoveWindow("Histograma Value",900,400);
	cvShowImage( "OpenCv" , dst );
	cvShowImage( "Histograma Saturation" , graphist_s );
	cvShowImage( "Histograma Value" , graphist_v);
	cout<<"Otsu Saturation:\t"<<umbral_s<<"\n";
	cout<<"Otsu Value:\t"<<umbral_v<<"\n";
        /* Libera memoria */
	cvWaitKey( );
        cvDestroyAllWindows( );
 	cvReleaseImage( &src );
 	cvReleaseImage( &dst );
	cvReleaseImage( &h );
	cvReleaseImage( &s );
	cvReleaseImage( &v );
	cvReleaseImage( &graphist_s );
	cvReleaseImage( &graphist_v );

//============================================================================================================
//============================================================================================================
//					FIN
//============================================================================================================
//============================================================================================================
        return 0;
}

//	Umbralado basado en el Algoritmo de Otsu
int fast_otsu(CvHistogram *histograma){
	float hist[255], N = 0.0, p[255], w1[255], w2[255], u1=0.0, u2=0.0, uT, sB2, aux = 0.0;
	int i, j, umbral = 0;

	for( i = 0; i<256; i++) hist[i] = cvQueryHistValue_1D( histograma , i );

	for( i = 0; i<256; i++) N += hist[i]; 

	for( i = 0; i<256; i++) p[i] = hist[i]/N;

	w1[0] = p[0];
	w2[0] = 1 - w1[0];

	for( i = 1; i<256; i++){
 
		w1[i] = w1[i+1] + p[i];
		w2[i] = 1 - w1[i];
			if( ( w1[i] != 0.0) && ( w2[i] != 0.0)){
				for( j = 0; j<i; j++)	u1 += (j+1)*p[j]/w1[i];
				for( j = i; j<256; j++)	u2 += (j+1)*p[j]/w2[i];
				uT =  u1*w1[i] + u2*w2[i];
				sB2 = w1[i]*pow( u1-uT, 2) + w2[i]*pow( u2-uT, 2);
			};
			if( aux < sB2){ 
			aux = sB2;
			umbral = i-1;
			};
			u1 = 0.0;
			u2 = 0.0;

	};

	return umbral;

}

//	DIBUJA EL HISTOGRAMA
void dhist( CvHistogram* histograma1, CvHistogram* histograma2, IplImage* graphist1, IplImage* graphist2 ){
	float val_a, val_b, aux;
	int i;

	for( i = 1; i < 256; i++) {

		aux = cvRound( cvQueryHistValue_1D( histograma1 , i - 1 ));
		val_a = cvRound( cvQueryHistValue_1D( histograma1, i));

		cvLine( graphist1, cvPoint( 0 , i ), cvPoint( 9 , i ), CV_RGB( i , i , 0), 1 );
		cvLine( graphist1, cvPoint( 10 , i ), cvPoint( val_a + 10 , i ), CV_RGB( 255 , 255 , 0 ), 1 );

	};

	for( i = 1; i < 256; i++) {

		aux = cvRound( cvQueryHistValue_1D( histograma2 , i - 1 ));
		val_b = cvRound( cvQueryHistValue_1D( histograma2, i));

		cvLine( graphist2, cvPoint( 0 , i ), cvPoint( 9 , i ), CV_RGB( 0 , i , i), 1 );
  		cvLine( graphist2, cvPoint( 10 , i), cvPoint( val_b + 10 , i), CV_RGB( 0 , 255 , 255 ), 1 );

	};
}

