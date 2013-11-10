#include "cv.h"
#include "highgui.h"
#include "stdio.h"
#include "ttcontours.h"

//testing program that loads a particular image and shows the actual behavior of ttcontours.h

int main(void){
	ttModels theModels;
	int theVals[6];
	ttInit(theModels);
	
	/*for(int i=0;i<6;i++){
		printf("%d\t:\t%d\n",i,theVals[i]);
	}//*/
	/*cvShowImage("model1",theModels.model1);
	cvShowImage("model2",theModels.model2);
	cvShowImage("model3",theModels.model3);
	cvWaitKey();//*/
	IplImage *theImg=cvLoadImage("testImage05.jpg",0);
	assert(theImg!=NULL);
	cvThreshold(theImg,theImg,180,1,CV_THRESH_BINARY);
	ttContours(theImg,1,theModels,NULL);
	ttContours(theImg,2,theModels,NULL);
	ttContours(theImg,3,theModels,NULL);//*/
	/*cvNamedWindow("chie",1);
	IplImage *someFrame=cvCreateImage(cvSize(500,500),IPL_DEPTH_8U,1);
	cvDrawContours(someFrame,theModels.c2,CV_RGB(255,255,255),CV_RGB(0,0,0),100,1,8);
	cvShowImage("chie",someFrame);	
	cvWaitKey();//*/
	return 0;
}
