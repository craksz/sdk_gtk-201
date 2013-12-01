#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include "ImgProc/ImgProc.h"
#include "UI/gui.h"
#include "UI/gamepad.h"

void main(int argc, char ** argv){
	int a=1234;
	IplImage * theFrame;
	IplImage * originalFrame;
	cvNamedWindow("main",CV_WINDOW_AUTOSIZE);
	if(argc<2) {
		printf("\ngotta provide image path to load\n\n");
		exit(-1);
	}
	ttInitVars();
	
	
	init_gui(0,NULL);
	if(argc==3){
		gui_t * gui=get_gui();
		gui->segColor=atoi(argv[2]);
	}
//	theFrame=cvLoadImage("./../images/image05.jpg",-1);
	theFrame=cvLoadImage(argv[1],-1);
	originalFrame=cvLoadImage(argv[1],-1);
	cvShowImage("main",originalFrame);
	cvWaitKey(0);
	if(ttMain(theFrame)!=C_OK){
		printf("\nno contours found!\n\n");
		exit(0);
	}
	cvCvtColor(theFrame,theFrame,CV_RGB2BGR);
	cvShowImage("main",theFrame);
	cvWaitKey(0);
	cvDestroyWindow("main");
	
}
