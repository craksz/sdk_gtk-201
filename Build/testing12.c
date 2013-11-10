#include "ttcontourstest.h"

//calibration 

#define show_results 1
int main(void){
	char theString[50];
	ttModels theModels;
	ttInit(&theModels);
	
	CvSeq* theContour;
	IplImage *theFrame,*theSFrame=cvCreateImage(cvSize(176,144),IPL_DEPTH_8U,1);
	int dir,i,theColor,flag;
	for (dir=0;dir<10;dir++){
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
			if(dir==2||dir==6||dir==7||dir==8||dir==9) theColor=0;
			cvCvtColor(theFrame,theFrame,CV_RGB2BGR);
			ttSegmenter(theFrame,theSFrame,theColor);
			//cvShowImage("img",theSFrame);
			//cvWaitKey(0);
			ttImprover(theSFrame,theSFrame);
			theContour=ttTrainingContours(theSFrame,dir);
			//printf("\n\nsdasdfa\n\n");
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
