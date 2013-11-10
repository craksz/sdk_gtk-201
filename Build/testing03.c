#include "stdio.h"
#include "cv.h"
#include "highgui.h"

//test code for webcam+opencv

int main(void){
	cvNamedWindow("theCam",1);
	cvNamedWindow("theCam2",1);
	cvMoveWindow("theCam",0,0);
	cvMoveWindow("theCam2",700,0);
	IplImage *theFrame;
	CvCapture* theCamera=cvCaptureFromCAM(-1);
	//CvCapture* theCamera=cvCreateCameraCapture(-1);
	if (!theCamera){
		printf("\n\nchinga\n\n");
		exit(0);
	}
	char theCh;
	while(theCh!=0x1B){
		theFrame=cvQueryFrame(theCamera);
		cvShowImage("theCam",theFrame);
		cvConvertImage(theFrame,theFrame,CV_CVTIMG_SWAP_RB);
		cvShowImage("theCam2",theFrame);
		theCh=cvWaitKey(2);
	}
	cvDestroyAllWindows();
	cvReleaseData(theFrame);
	cvReleaseCapture(&theCamera);
	return(0);
}
