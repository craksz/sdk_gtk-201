#include "ttcontourstest.h"
#include "ml.h"
//#include "ml.h"

#define show_results 1
#define ts_count 300
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

    	const int K = 15;
    	int k, accuracy;
    	float response;
    	CvMat* trainData = cvCreateMat( 300, 2, CV_32FC1 );
    	CvMat* trainClasses = cvCreateMat( 300, 1, CV_32FC1 );
    	IplImage* spaceClasses = cvCreateImage( cvSize( 700, 700 ), 8, 3 );
    	cvZero( spaceClasses );

   	CvMat trainData1, trainData2, trainData3, trainData4, trainData5, trainData0; 
	CvMat trainClasses1, trainClasses2, trainClasses3, trainClasses4, trainClasses5, trainClasses0;

    	// entrenamiento
    	cvGetRows( trainData, &trainData0, 0, 50, 1);
    	cvGetRows( trainData, &trainData1, 50, 100, 1);
    	cvGetRows( trainData, &trainData2, 100, 150, 1);
    	cvGetRows( trainData, &trainData3, 150, 200, 1);
    	cvGetRows( trainData, &trainData4, 200, 250, 1);
    	cvGetRows( trainData, &trainData5, 250, 300, 1);

    	cvGetRows( trainClasses, &trainClasses0, 0, 50, 1 );
    		cvSet( &trainClasses0, cvScalar(0,0,0,0) , NULL);
    	cvGetRows( trainClasses, &trainClasses1, 50, 100, 1 );
    		cvSet( &trainClasses1, cvScalar(1,1,1,1) , NULL);
    	cvGetRows( trainClasses, &trainClasses2, 100, 150, 1 );
    		cvSet( &trainClasses2, cvScalar(2,2,2,2) , NULL);
    	cvGetRows( trainClasses, &trainClasses3, 150, 200, 1 );
    		cvSet( &trainClasses3, cvScalar(3,3,3,3) , NULL);
    	cvGetRows( trainClasses, &trainClasses4, 200, 250, 1 );
    		cvSet( &trainClasses4, cvScalar(4,4,4,4) , NULL);
    	cvGetRows( trainClasses, &trainClasses5, 250, 300, 1 );
    		cvSet( &trainClasses5, cvScalar(5,5,5,5) , NULL);

	for (dir=0;dir<6;dir++){
		theColor=1;flag=0;

		for (i=0;i<51;i++){
			cvZero(theSFrame);

			if(i+flag<10)
				sprintf(theString,"../images/%d/image0%d.jpg",dir,i+flag);
			else
				sprintf(theString,"../images/%d/image%d.jpg",dir,i+flag);
			
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

			trainData->data.fl[0] = theHu->hu1 * 1000; 
            		trainData->data.fl[1] = theHu->hu4 * 100000;
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::			
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
			if(show_results==1){
				cvDrawContours(theFrame,theContour,CV_RGB(255,255,255),CV_RGB(255,0,0),100,5,8,cvPoint(0,0));
				cvShowImage("win",theFrame);
				//printf("\n%d,%d,%s\n",dir,i,theString);
				printf("\n%f||%f",trainData->data.fl[0],trainData->data.fl[0]);
				cvWaitKey(0);
			}
		}
		printf("\n==================================");
	}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::			
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   		// learn classifier
    	CvKNearest knn( trainData, trainClasses, 0, false, K );
    	CvMat* theNearests = cvCreateMat( 1, K, CV_32FC1);
    	float _sample[2];
    	CvMat sample = cvMat( 1, 2, CV_32FC1, _sample );
    	cvNamedWindow( "classifier result", 1 );
 
    	for( i = 0; i < spaceClasses->height; i++ ){
        	for( j = 0; j < spaceClasses->width; j++ ){
            		sample.data.fl[0] = (float)j;
            		sample.data.fl[1] = (float)i;

            		// estimates the response and get the neighbors' labels
            		response = knn.find_nearest(&sample,K,0,0,theNearests,0);

            		// highlight the pixel depending on the accuracy (or confidence)
            		cvSet2D( spaceClasses, i, j, response == 0 ?
                		CV_RGB(0,0,150) : response == 1 ?
				CV_RGB(0,150,0) : response == 2 ?
				CV_RGB(150,0,0) : response == 3 ?
				CV_RGB(150,0,150) : response == 4 ?
				CV_RGB(150,150,0) : CV_RGB(150,150,150) );
        	}
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
