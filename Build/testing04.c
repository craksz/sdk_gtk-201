#include "stdio.h"
#include "cv.h"
#include "highgui.h"
#include "ttcontourstest.h"

//test code for webcam+openCV+ttcontours

int main(void){

	IplImage *theFrame, *theFrame2, *theFrame3;
	CvCapture *theCam=cvCreateCameraCapture(-1);
	char theChar;
	CvSize size;
	CvPoint theCentroid;
	CvSeq* theContour;
	
	ttModels theModels;
	ttInit(&theModels);
	
	if(!theCam) {
		printf("\nCamera not found\n");
		return(0);
	}
	/*theFrame=cvLoadImage("colormap.png",1);
	theFrame2=cvLoadImage("colormap.png",1);//*/
	size=cvSize(cvGetCaptureProperty(theCam,CV_CAP_PROP_FRAME_WIDTH),cvGetCaptureProperty(theCam,CV_CAP_PROP_FRAME_HEIGHT));
	theFrame2=cvCreateImage(size,IPL_DEPTH_8U,1);
	theFrame3=cvCreateImage(size,IPL_DEPTH_8U,3);
	
	cvNamedWindow("win1",1);
	cvNamedWindow("win2",1);
	cvMoveWindow("win1",0,0);
	cvMoveWindow("win2",700,0);
	/*cvNamedWindow("H",0);
	cvNamedWindow("S",0);
	cvNamedWindow("V",0);
	cvMoveWindow("H",0,500);
	cvMoveWindow("S",350,500);
	cvMoveWindow("V",700,500);//*/
	while ((theChar=cvWaitKey(20))!=0x1B){
		theFrame=cvQueryFrame(theCam);
		cvCopy(theFrame,theFrame3,NULL);
		cvZero(theFrame2);
		ttSegmenter(theFrame,theFrame2,1);
		ttImprover(theFrame2,theFrame2);
		//printf("nchannels %d\n",theFrame2->nChannels);
		//printf("chingao!\n");
		cvShowImage("win2",theFrame2);
		theContour=ttContours(theFrame2,2,&theModels,NULL);
		
		//cvZero(out);
		if (theContour==NULL)
			continue;
		theCentroid=ttFindCentroid(theContour);
		cvCircle(theFrame3,theCentroid,1,CV_RGB(255,255,255),1,8,0);
		cvCircle(theFrame3,theCentroid,6,CV_RGB(255,0,0),1,8,0);
		//cvCircle(theFrame3,theCentroid,11,CV_RGB(255,255,255),1,8,0);
		//cvCircle(theFrame3,theCentroid,16,CV_RGB(255,0,0),1,8,0);
		//cvCircle(theFrame3,ttFindCentroid(theContour),15,CV_RGB(255,255,255),1,8,0);
		cvDrawContours(theFrame3,theContour,CV_RGB(255,255,255),CV_RGB(255,255,255),1,5,8,cvPoint(0,0));
		cvShowImage("win1",theFrame3);//*/
	}
	cvDestroyAllWindows();
	cvReleaseData(theFrame);
	cvReleaseData(theFrame2);
	cvReleaseData(theFrame3);
	cvReleaseCapture(&theCam);
	return(0);
}


