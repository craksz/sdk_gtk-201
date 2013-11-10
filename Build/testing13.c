#include "cv.h"
#include "highgui.h"
#include "stdio.h"

IplImage * theFrame;
CvPoint theCenter;
int width,height;
CvPoint p1,p2,p3,p4;
float theAngle;

void drawit(void){
				cvCircle(theFrame,theCenter,2,CV_RGB(255,0,0),1,8,0);
				//cvCircle(theFrame,p1,2,CV_RGB(255,0,0),1,8,0);
				//cvCircle(theFrame,p2,2,CV_RGB(255,0,0),1,8,0);
				cvLine(theFrame,p1,p2,CV_RGB(255,255,255),1,8,0);
				//cvLine(theFrame,p3,p4,CV_RGB(0,0,255),1,8,0);
}

void main(void){
	
	
	theFrame =cvCreateImage(cvSize(200,200),IPL_DEPTH_8U,3);
	theCenter=cvPoint(100,100);
	theAngle=0;
	width=50; height=60;
	int i;
	for (i=0;i<9;i++){
		p1=cvPoint(theCenter.x+width/2*sin(-theAngle),theCenter.y+width/2*cos(-theAngle));
		p2=cvPoint(theCenter.x-width/2*sin(-theAngle),theCenter.y-width/2*cos(-theAngle));/*
		p3=cvPoint(theCenter.x+height/2*cos(theAngle),theCenter.y+height/2*sin(theAngle));
		p4=cvPoint(theCenter.x-height/2*cos(theAngle),theCenter.y-height/2*sin(theAngle));//*/
		theAngle+=10*3.14159/180;
	
	drawit();
	cvShowImage("win",theFrame);
	cvWaitKey(0);
	}	
	
}

