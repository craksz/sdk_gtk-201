#include "ttcontourstest.h"
#include "ml.h"
#include "iostream"

using namespace std;

#define show_results 1
#define show_knn  0
#define show_space 1
#define ts_count 300
#define skale1 1E3
#define skale2 1E4
#define K 11

int main(void){
	char theString[50];
	ttModels theModels;
	ttInit(&theModels);
	
	CvSeq* theContour;
	IplImage *theFrame,*theSFrame=cvCreateImage(cvSize(176,144),IPL_DEPTH_8U,1);
	int dir,i,theColor,flag,j;

	CvMoments themoments;
	CvHuMoments thehu;
	CvMoments *theMoments = &themoments;
	CvHuMoments *theHu = &thehu;

    	CvMat* trainData = cvCreateMat( 300, 2, CV_32FC1 );
    	CvMat* trainClasses = cvCreateMat( 300, 1, CV_32FC1 );
    	IplImage* spaceClasses = cvCreateImage( cvSize( 200, 200 ), 8, 3 );
    	cvZero( spaceClasses );

   	CvMat trainData1, trainData2, trainData3, trainData4, trainData5, trainData0; 
	CvMat trainClasses1, trainClasses2, trainClasses3, trainClasses4, trainClasses5, trainClasses0;

    	// entrenamiento
    	cvGetRows( trainData, &trainData0, 0, 50);
    	cvGetRows( trainData, &trainData1, 50, 100);
    	cvGetRows( trainData, &trainData2, 100, 150);
    	cvGetRows( trainData, &trainData3, 150, 200);
    	cvGetRows( trainData, &trainData4, 200, 250);
    	cvGetRows( trainData, &trainData5, 250, 300);

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

	for (dir=0;dir<6;dir++){
		theColor=1;flag=0;

		for (i=0;i<51;i++){
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
			if(dir==2) theColor=0;

			cvCvtColor(theFrame,theFrame,CV_RGB2BGR);
			ttSegmenter(theFrame,theSFrame,theColor);
			ttImprover(theSFrame,theSFrame);
			theContour=ttTrainingContours(theSFrame,dir);
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::			
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
			cvContourMoments( theContour , theMoments );
			cvGetHuMoments( theMoments , theHu );

			trainData->data.fl[0] = theHu->hu2 * skale1; 
            		trainData->data.fl[1] = theHu->hu3 * skale2;
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::			
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
			if(show_results==1){
				//cvDrawContours(theFrame,theContour,CV_RGB(255,255,255),CV_RGB(255,0,0),100,5,8,cvPoint(0,0));
				//cvShowImage("win",theFrame);
				//printf("\n%d,%d,%s\n",dir,i,theString);
				printf("\n%f||%f",theHu->hu1 * skale1,theHu->hu2 * skale1);
				printf("||%f||%f",theHu->hu3 * skale2,theHu->hu4 * skale2);
				printf("||%f||%f",theHu->hu5 * skale1,theHu->hu6 * skale1);
				printf("||%f",theHu->hu7);
			}
		}
		if(show_results==1)
			printf("\n====================================================================\n");
	}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::			
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   		// learn classifier
    	CvKNearest knn( trainData, trainClasses, 0, false, K );
    	CvMat* theNearests = cvCreateMat( 1, K, CV_32FC1);
	cvZero( theNearests );

    	int k;
    	float response = 0.0;
    	float _sample[] = {0.0,0.0};
    	CvMat sample = cvMat( 1, 2, CV_32FC1, _sample );
 
    	for( i = 0; i < spaceClasses->height; i++ ){
        	for( j = 0; j < spaceClasses->width; j++ ){
            		sample.data.fl[0] = (float)j;
            		sample.data.fl[1] = (float)i;

            		// estimates the response and get the neighbors' labels
            		response = knn.find_nearest(&sample,K,0,0,theNearests,0);

			if(show_space == 1)
            		// highlight the pixel depending on confidence
			switch ((int)response){
				case 0:		cvSet2D( spaceClasses, i, j, CV_RGB(0,0,200));
						break;
				case 1:		cvSet2D( spaceClasses, i, j, CV_RGB(0,200,0));
						break;
				case 2:		cvSet2D( spaceClasses, i, j, CV_RGB(200,0,0));
						break;
				case 3:		cvSet2D( spaceClasses, i, j, CV_RGB(200,0,200));
						break;
				case 4:		cvSet2D( spaceClasses, i, j, CV_RGB(200,200,0));
						break;
				default:	cvSet2D( spaceClasses, i, j, CV_RGB(200,200,200));
						break;
			}
        	}
    	}
	if(show_space == 1){
	    	cvNamedWindow( "classifier result", 1 );
    		cvShowImage( "classifier result", spaceClasses );
		cvWaitKey(0);
	}
/*
    	// display the original training samples
    	for( i = 0; i < train_sample_count/2; i++ ){
        	CvPoint pt;
        	pt.x = cvRound(trainData1.data.fl[i*2]);
        	pt.y = cvRound(trainData1.data.fl[i*2+1]);
        	cvCircle( img, pt, 2, CV_RGB(255,0,0), CV_FILLED );
        	pt.x = cvRound(trainData2.data.fl[i*2]);
        	pt.y = cvRound(trainData2.data.fl[i*2+1]);
        	cvCircle( img, pt, 2, CV_RGB(0,255,0), CV_FILLED );
	}*/
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::			
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        cvDestroyAllWindows(	);
	cvReleaseImage( &theFrame );
	cvReleaseImage( &theSFrame );
	cvReleaseImage( &spaceClasses );
	return (0);
}
