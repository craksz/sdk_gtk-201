#include "ttcontourstest.h"

#define show_results 1
int main(void){
	char theString[55];
	ttModels theModels;
	ttInit(&theModels);
	
	CvSeq* theContour;
	IplImage *theFrame,*theSFrame=cvCreateImage(cvSize(176,144),IPL_DEPTH_8U,1);
	int dir,i,theColor,flag;
	for (dir=0;dir<6;dir++){
		theColor=0;flag=0;
		for (i=0;i<1;i++){
			cvZero(theSFrame);
			if(i+flag<10)
				sprintf(theString,"/images/%d/image0%d.jpg",dir,i+flag);
			else
				sprintf(theString,"/images/%d/image%d.jpg",dir,i+flag);
			//cvWaitKey(0);
			if((theFrame=cvLoadImage(theString,1))==NULL){
			
			printf("\n%s, %d,%d,%d\n",theString,i,flag,dir);
			
				flag++;
				i--;//break;
				continue;
			}
			
			if(dir==1) theColor=0;
			if(dir==2) theColor=0;
			cvCvtColor(theFrame,theFrame,CV_RGB2BGR);
			ttSegmenter(theFrame,theSFrame,theColor);
			ttImprover(theSFrame,theSFrame);
			theContour=ttTrainingContours(theSFrame,dir);
			if(show_results==1){
				cvDrawContours(theFrame,theContour,CV_RGB(255,255,255),CV_RGB(255,0,0),100,5,8,cvPoint(0,0));
				cvShowImage("win",theFrame);
				printf("%d,%d,%s\n",dir,i,theString);
				cvWaitKey(0);
			}
			/*#########
			aqui es donde pones el codigo para usar cada contorno*/
			
			
			
			
			
			
			/*aqui se termina tu codigo... creo
			###########*/
		}
	}
	return (0);
}
