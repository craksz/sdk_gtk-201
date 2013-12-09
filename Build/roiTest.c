#include <stdio.h>
#include <cv.h>
#include <highgui.h>

int main(void){
	IplImage * theFrame;
	theFrame=cvLoadImage("spaceClasses2.bmp",1);
	CvRect theRoi=cvRect(0,0,100,100);
	cvNamedWindow("theWindow",CV_WINDOW_AUTOSIZE);
	cvSetImageROI(theFrame,theRoi);
	cvShowImage("theWindow",theFrame);
	cvWaitKey(0);
	cvDestroyWindow("theWindow");
}
