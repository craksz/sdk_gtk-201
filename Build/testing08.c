#include "cv.h"
#include "highgui.h"
#include "stdio.h"
#include "ttcontours.h"

//this file saves vars for the statistic errors!
//it compares the models with samples of the objects and returns a mean value for each error

int main(void){
	static char theStr[10];
	static double theErr;
	static double theSum[3];
	static IplImage* theImg;
	static ttModels theModels;
	static int theVals[6];
	ttInit(theModels,theVals);
	static theCell theSaver;
	
	/*cvShowImage("model1",theModels.model1);
	cvShowImage("model2",theModels.model2);
	cvShowImage("model3",theModels.model3);
	cvWaitKey();//*/
	for (int i=1;i<4;i++){
		theSum[i-1]=0;
		for (int j=1;j<11;j++){
			sprintf(theStr,"./theReference/%d%d.jpg",i,j);
			theImg=cvLoadImage(theStr,0);
			assert(theImg!=NULL);
			cvThreshold(theImg,theImg,180,1,CV_THRESH_BINARY);
			ttContours(theImg,i,theModels,&theErr);
			//printf("%0.2f\n",theErr);
			theSum[i-1]+=theErr*ERR_RESOLUTION;
		}
		//printf("%d<-Sum!\n",(int)theSum[i-1]);
		theSaver[i-1].theInt=(int)theSum[i-1];
		sprintf(theStr,"theErr%d",i);
		strcpy(theSaver[i-1].theString,theStr);
	}
	saveVars("errors.dat",theSaver);
	printf("Error Values Saved! :)\n");
	return 0;
}

