#include "ttcontourstest.h"
#include "ml.h"
#include "iostream"

using namespace std;

#define show_results 0
#define show_space 1
#define ts_count 500
#define cwidth 800
#define cheight 800
#define skale1 1.5E3
#define skale2 2.5E3
#define skale3 0.3E5
#define skale4 1.6E5
#define skale5 2E7
#define skale6 1E5
#define skale7 1E8
#define K 17
#define nClasses 10
#define topGray 255

int main(void){
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	char theString[50];
	ttModels theModels;
	ttInit(&theModels);

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Variables de segmentación
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	CvSeq* theContour;
	IplImage *theFrame,*theSFrame=cvCreateImage(cvSize(176,144),IPL_DEPTH_8U,1);
	int dir,i,theColor,flag,j;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Variables para el cálculo de momentos
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	CvMoments themoments;
	CvHuMoments thehu;
	CvMoments *theMoments = &themoments;
	CvHuMoments *theHu = &thehu;
	int theRow = 0;
	
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Variables para el entrenamiento
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    	CvMat* trainData = cvCreateMat( ts_count, 2, CV_32FC1 );
    	CvMat* trainClasses = cvCreateMat( ts_count, 1, CV_32FC1 );
    	IplImage* spaceClasses;
    	IplImage* spaceClasses2;
    	//spaceClasses = cvCreateImage( cvSize( cwidth , cwidth ), 8, 1 );
    	spaceClasses = cvCreateImage( cvSize( cwidth , cwidth ), 8, 1 );
    	spaceClasses2 = cvCreateImage( cvSize( cwidth , cwidth ), 8, 3 );
    	
    	cvZero( spaceClasses );//*/

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Divide en Clases
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

   	CvMat trainData1, trainData2, trainData3, trainData4, trainData5, trainData0; 
   	CvMat trainData6, trainData7, trainData8, trainData9;    	
	CvMat trainClasses1, trainClasses2, trainClasses3, trainClasses4, trainClasses5, trainClasses0;
	CvMat trainClasses6, trainClasses7, trainClasses8, trainClasses9;

    	cvGetRows( trainData, &trainData0, 0, 50);
    	cvGetRows( trainData, &trainData1, 50, 100);
    	cvGetRows( trainData, &trainData2, 100, 150);
    	cvGetRows( trainData, &trainData3, 150, 200);
    	cvGetRows( trainData, &trainData4, 200, 250);
    	cvGetRows( trainData, &trainData5, 250, 300);
    	cvGetRows( trainData, &trainData4, 300, 350);
    	cvGetRows( trainData, &trainData5, 350, 400);
    	cvGetRows( trainData, &trainData4, 400, 450);
    	cvGetRows( trainData, &trainData5, 450, 500);

    	cvGetRows( trainClasses, &trainClasses0, 0, 50);
    		cvSet( &trainClasses0, cvScalar(0) );
    	cvGetRows( trainClasses, &trainClasses1, 50, 100);
    		cvSet( &trainClasses1, cvScalar(1) );
    	cvGetRows( trainClasses, &trainClasses2, 100, 150);
    		cvSet( &trainClasses2, cvScalar(2) );
    	cvGetRows( trainClasses, &trainClasses3, 150, 200);
    		cvSet( &trainClasses3, cvScalar(3) );
    	cvGetRows( trainClasses, &trainClasses4, 200, 250);
    		cvSet( &trainClasses4, cvScalar(4) );
    	cvGetRows( trainClasses, &trainClasses5, 250, 300);
    		cvSet( &trainClasses5, cvScalar(5) );
    	cvGetRows( trainClasses, &trainClasses5, 300, 350);
    		cvSet( &trainClasses5, cvScalar(6) );
    	cvGetRows( trainClasses, &trainClasses5, 350, 400);
    		cvSet( &trainClasses5, cvScalar(7) );
    	cvGetRows( trainClasses, &trainClasses5, 400, 450);
    		cvSet( &trainClasses5, cvScalar(8) );
    	cvGetRows( trainClasses, &trainClasses5, 450, 500);
    		cvSet( &trainClasses5, cvScalar(9) );

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Segmentación y entrenamiento
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	/*for (dir=0;dir<6;dir++){
		theColor=1;flag=0;
		for (i=0;i<50;i++){
			cvZero(theSFrame);
			if(i+flag<10)
				sprintf(theString,"images/%d/image0%d.jpg",dir,i+flag);
			else
				sprintf(theString,"images/%d/image%d.jpg",dir,i+flag);
			
			if((theFrame=cvLoadImage(theString,1))==NULL){
				flag++;
				i--;
				continue;
			}
			if(dir==1) theColor=2;
			if(dir==2) theColor=0;//*/
			
	for (dir=0;dir<nClasses;dir++){
		theColor=1;flag=0;
		for (i=0;i<50;i++){
		cvZero(theSFrame);
			if(i+flag<10)
				sprintf(theString,"images/%d/image0%d.jpg",dir,i+flag);
			else
				sprintf(theString,"images/%d/image%d.jpg",dir,i+flag);

			
			if((theFrame=cvLoadImage(theString,1))==NULL){
				flag++;
				i--;
				continue;
			}
			if(dir==1) theColor=2;
			if(dir==2||dir==6||dir==7||dir==8||dir==9) theColor=0;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Segmentación
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	
			cvCvtColor(theFrame,theFrame,CV_RGB2BGR);
			ttSegmenter(theFrame,theSFrame,theColor);
			ttImprover(theSFrame,theSFrame);
			theContour=ttTrainingContours(theSFrame,dir);

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Entrenamiento
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::			

			cvContourMoments( theContour , theMoments );
			cvGetHuMoments( theMoments , theHu );

			cvSetReal2D( trainData, theRow, 0, theHu->hu1 * skale1);
			cvSetReal2D( trainData, theRow, 1, theHu->hu4 * skale4);
			theRow++;

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Despliega los resultados
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

			if(show_results==1){
				//cvDrawContours(theFrame,theContour,CV_RGB(255,255,255),CV_RGB(255,0,0),100,5,8,cvPoint(0,0));
				//cvShowImage("win",theFrame);
				//printf("\n%d,%d,%s\n",dir,i,theString);
				printf("\n%f\t||%f\t",theHu->hu1 * skale1,theHu->hu2 * skale2);
				printf("||%f\t||%f\t",theHu->hu3 * skale3,theHu->hu4 * skale4);
				printf("||%f\t||%f\t",theHu->hu5 * skale5,theHu->hu6 * skale6);
				printf("||%f\t",theHu->hu7*skale7);
			}
		}
		if(show_results==1)
			printf("\n====================================================================\n");
			printf("%d\n",dir);
	}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//	Clasificación y aprendizaje
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::			

    	CvKNearest knn( trainData, trainClasses, 0, false, K );
    	CvMat* theNearests = cvCreateMat( 1, K, CV_32FC1);
	cvZero( theNearests );

    	int k;
    	float response = 0.0;
    	float _sample[] = {0.0,0.0};
    	CvMat sample = cvMat( 1, 2, CV_32FC1, _sample );
 	if(show_space == 1)
    	for( i = 0; i < spaceClasses->height; i++ ){
        	for( j = 0; j < spaceClasses->width; j++ ){
            		sample.data.fl[0] = (float)j;
            		sample.data.fl[1] = (float)i;

            		// estimates the response and get the neighbors' labels
            		response = knn.find_nearest(&sample,K,0,0,theNearests,0);

 	           		// highlight the pixel depending on confidence
						cvSetReal2D( spaceClasses, i, j, response);
						/*else//*/
							switch ((int)response){
								case 0:		cvSet2D( spaceClasses2, i, j, CV_RGB(0,0,0));
										break;
								case 1:		cvSet2D( spaceClasses2, i, j, CV_RGB(0,0,100));
										break;
								case 2:		cvSet2D( spaceClasses2, i, j, CV_RGB(0,100,0));
										break;
								case 3:		cvSet2D( spaceClasses2, i, j, CV_RGB(100,0,0));
										break;
								case 4:		cvSet2D( spaceClasses2, i, j, CV_RGB(100,0,100));
										break;
								case 5:		cvSet2D( spaceClasses2, i, j, CV_RGB(100,100,0));
										break;
								case 6:		cvSet2D( spaceClasses2, i, j, CV_RGB(0,100,100));
										break;
								case 7:		cvSet2D( spaceClasses2, i, j, CV_RGB(255,0,255));
										break;
								case 8:		cvSet2D( spaceClasses2, i, j, CV_RGB(255,255,0));
										break;
								default:	cvSet2D( spaceClasses2, i, j, CV_RGB(255,255,255));
									break;
							}//*/
        	}
    	}//*/
    	
	cvSaveImage("../spaceClasses.bmp",spaceClasses);
	//cvAddWeighted(spaceClasses,0,spaceClasses,topGray/nClasses,0,spaceClasses);
	cvCvtColor(spaceClasses2,spaceClasses2,CV_RGB2BGR);
	cvSaveImage("../spaceClasses2.bmp",spaceClasses2);
	if(show_space == 1){
	    	cvNamedWindow( "classifier result", 1 );
    		cvShowImage( "classifier result", spaceClasses );
	    	cvNamedWindow( "classifier result e", 1 );
    		cvShowImage( "classifier result e", spaceClasses2 );
		cvWaitKey(0);
	}//*/
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        cvDestroyAllWindows(	);
	cvReleaseImage( &theFrame );
	cvReleaseImage( &theSFrame );
	cvReleaseImage( &spaceClasses );
	return (0);
}
