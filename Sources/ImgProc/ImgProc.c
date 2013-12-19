//#include "ttcontours.h"
#include "ImgProc.h"

#include "cv.h"
#include "highgui.h"
#include "stdio.h"
#include "UI/gui.h"
#include "UI/gamepad.h"

#define hTolerance 15
static int segValues[18];
CvMemStorage* storage;
FILE *theFile;
CvBox2D theBox;
CvSeq* theContour;
CvFont theFont;
CvMoments themoments;
CvHuMoments thehu;
CvMoments *theMoments = &themoments;
CvHuMoments *theHu = &thehu;
CvVideoWriter *theWriter;
IplImage * classSpaceFrame;         //labeled mode
IplImage * outputClassSpaceFrame;   //color mode updated frequently
IplImage * classSpaceClear;         //color mode backup
IplImage * outputFrame;         
IplImage * tempFrame;         



int ttInitImgProc(void){

	ttInitSegValues();
		
#if VIDEO_SAVE
	char filename[100];
	sprintf(filename,
					"gnuplot/theVid-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%d.avi",
					XGAIN1,XGAIN2,XGAIN3,YGAIN1,YGAIN2,YGAIN3,ZGAIN1,ZGAIN2,ZGAIN3,WGAIN1,WGAIN2,WGAIN3,switchTop);
				
	theWriter=cvCreateVideoWriter(filename,CV_FOURCC('M','J','P','G'),25,cvSize(justWidth,justHeight),1);
#endif
  
    tempFrame=cvCreateImage(cvSize(VIDEO_VERT_WIDTH,VIDEO_VERT_HEIGHT),IPL_DEPTH_8U,1);
    outputFrame=cvCreateImage(cvSize(VIDEO_VERT_WIDTH,VIDEO_VERT_HEIGHT),IPL_DEPTH_8U,3);
    cvInitFont(&theFont,CV_FONT_HERSHEY_PLAIN,1.0,1.0,0,1,8);
	classSpaceFrame=cvLoadImage("spaceClasses.bmp",0);
	outputClassSpaceFrame=cvLoadImage("spaceClasses2.bmp",1);
	classSpaceClear=cvLoadImage("spaceClasses2.bmp",1);
    printf("\n%d\n\n",(int)classSpaceClear);
    
    
   
    printf("\nTTINIT OK\n\n");
    //assert(classSpaceFrame!=NULL);
    //assert(outputClassSpaceFrame!=NULL);
    
    return 0;
     
}
CvSeq* ttFindContours(IplImage *src){
        
    if(src==NULL){ printf("\nttFindContours null pointer\n\n"); exit(-1); }
	CvMemStorage* storage =cvCreateMemStorage(0);
	CvSeq* firstContour = NULL,*lastContour=NULL;
	int n=cvFindContours(src,storage,&firstContour,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_NONE,cvPoint(0,0));//*/
	if(n==0){
		//printf("no contours\n\n");
		return NULL;
	}
	
	CvSeq* i=firstContour,*sContour=NULL;
	double area,lastArea=0.0;
	while(1){	
		area=cvContourArea(i,CV_WHOLE_SEQ,0);
		if(area>MINAREA){
		
			if(area>lastArea){
				lastContour=i;
			}
		}	
		//printf("file");
		if(i->h_next){
			if(i->v_next){
				//SAVECURRENT
				sContour=i->h_next;
				i=i->v_next;
				//printf("saveCurrent\n");
			}	
			else {
				//GOHNEXT
				i=i->h_next;
				//printf("goHnext1\n");
			}
		}
		else {
			if(i->v_next){
				//GOVNEXT
				i=i->v_next;
				//printf("goVnext1\n");
			}
			else{
				if (sContour!=NULL){
					//GOSAVED
					i=sContour;
					sContour=NULL;
					//printf("goSaved\n");
				}
				else break;
			}
		}
		
		//printf("Continuing\n");
	}
	
/*	if(lastContour->v_next!=NULL&&number!=4&&number!=6&&number!=8&&number!=9)
		return lastContour->v_next;
	else//*/
	if(lastContour==NULL)
		return NULL;
	
	if( lastContour->v_next){
		area=cvContourArea(lastContour->v_next,CV_WHOLE_SEQ,0);
		if(area>340)
			return lastContour->v_next;
		else
			return lastContour;
	}
	else 
		return lastContour;
	
}




void ttInitSegValues(void){
	
	storage  =cvCreateMemStorage(0);

#if USE_TTIOFILES
		var_t theSegLoader[18]; 
		loadVars("segVals.dat",theSegLoader);	
		int i;
		
		for(i=0;i<18;i++){
			//printf("%s:\t%d\n",theSegLoader[i].theString,	theSegLoader[i].theInt);
			segValues[i]=(int)theSegLoader[i].theInt;
		}

#else
		segValues[0]=0;
		segValues[1]=0;
		segValues[2]=70;
		segValues[3]=0;
		segValues[4]=103;
		segValues[5]=0;
		
		segValues[6]=30;
		segValues[7]=151;
		segValues[8]=26;
		segValues[9]=106;
		segValues[10]=27;
		segValues[11]=151;
		
		segValues[12]=174;
		segValues[13]=250;
		segValues[14]=66;
		segValues[15]=145;
		segValues[16]=39;
		segValues[17]=139;
		
#endif
}

void ttSegmenter(IplImage *src, IplImage *dst, int color){
    
        if(src==NULL){ printf("\nttSegmenter null pointer\n\n"); exit(-1); }
        if(dst==NULL){ printf("\nttSegmenter dst null pointer\n\n"); exit(-1); }
        switch(color){
            case RED_COLOR:
            case GREEN_COLOR:
            case BLUE_COLOR:
                break;
            default:
                printf("\nttSegmenter invalid color\n\n");
                exit(-1);
        }
	
	IplImage *c1,*c2,*c3;
	int cRange[6];
	c1=cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1);
	c2=cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1);
	c3=cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1);
	cvCvtColor(src,src,CV_RGB2HSV);
	
	//printf("\n\t\tEEEEEECOLE!\n\n");
	
	cvSplit(src,c1,c2,c3,NULL);
	/*rojo 105:135
	azul	
	verde
	*/
	//assert(segValues[0]!=NULL);
	/*switch (color){
		case 0://red
			break;
		case 1://green
			cRange[0]=segValues[2];
			cRange[1]=segValues[3];
			break;
		case 2://blue
			cRange[0]=segValues[4];
			cRange[1]=segValues[5];
			break;
		default:
			break;
	}*/
	cRange[0]=segValues[color*2+0];
	cRange[1]=segValues[color*2+1];
	cRange[2]=segValues[color*2+6];
	cRange[3]=segValues[color*2+7];
	cRange[4]=segValues[color*2+12];
	cRange[5]=segValues[color*2+13];
	//printf("%d\n",cRange[0]);
	
	
		if(hTolerance <= cRange[0] && 180-hTolerance >= cRange[0]){
			//printf("%d\n",cRange[0]);
			cvThreshold(c1,c1,cRange[0]+hTolerance,255,CV_THRESH_TOZERO_INV);
			cvThreshold(c1,c1,cRange[0]-hTolerance,255,CV_THRESH_BINARY);
		}
		else{
			IplImage *cH=cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1);
			if(cRange[0]<=180-hTolerance){
				cvThreshold(c1,cH,cRange[0]+hTolerance,255,CV_THRESH_BINARY_INV);
				cvThreshold(c1,c1,180+cRange[0]-hTolerance*2,255,CV_THRESH_BINARY);
			}
			else{
				cvThreshold(c1,cH,cRange[0]+hTolerance-180,255,CV_THRESH_BINARY_INV);
				cvThreshold(c1,c1,cRange[0]-hTolerance*2,255,CV_THRESH_BINARY);
				
			}
			cvOr(c1,cH,c1,NULL);
		}
	
	//cvThreshold(c1,c1,cRange[1],255,CV_THRESH_TOZERO_INV);
	//cvThreshold(c1,c1,cRange[0],255,CV_THRESH_BINARY);
	cvThreshold(c2,c2,cRange[3],255,CV_THRESH_TOZERO_INV);
	cvThreshold(c2,c2,cRange[2],255,CV_THRESH_BINARY);
	cvThreshold(c3,c3,cRange[5],255,CV_THRESH_TOZERO_INV);
	cvThreshold(c3,c3,cRange[4],255,CV_THRESH_BINARY);
	/*cvShowImage("H",c1);
	cvShowImage("S",c2);
	cvShowImage("V",c3);//*/
	
	//cvCvtColor(src,src,CV_HSV2RGB);
	//printf("chinga");	
	cvAnd(c1,c2,dst,c3);
	//cvCopy(c1,dst,NULL);
	//cvShowImage("win2",dst);
	//cvShowImage("win2",c3);
	//cvMerge(c1,c2,c3,NULL,src);
}

void ttImprover(IplImage *src,IplImage *dst){
        if(src==NULL){ printf("\nttImprover null pointer\n\n"); exit(-1); }
	if(dst==NULL){ printf("\nttImprover dst null pointer\n\n"); exit(-1); }
	cvDilate(src,dst,NULL,1);
	cvErode(src,dst,NULL,1);
	//cvSmooth(src,dst,CV_GAUSSIAN,3,3,0,0);
	//cvCvtColor(dst,dst,CV_RGB2GRAY);
}

CvBox2D ttFindBox(CvSeq* theContour){
	if(theContour==NULL){ printf("\nttFindBox null pointer\n\n"); exit(-1); }
        CvBox2D theBox=cvMinAreaRect2(theContour,NULL);
	return theBox;
}

int theC=0;
void ttCalibration(IplImage *src){
	
    if(theC)
	
	theC++;
}

IplImage * ttGetClassSpaceImage(){
    if(classSpaceFrame==NULL){
        printf("\nclassSpace null\n\n");
        exit(C_FAIL);
    }
    return classSpaceFrame;
}

IplImage * ttGetOutputClassSpaceImage(){
    if(outputClassSpaceFrame==NULL){
        printf("\noutputClassSpace null\n\n");
        exit(C_FAIL);
    }
    return outputClassSpaceFrame;
}

void ttResetOutputClassSpaceFrame(){
    
 	cvCopy(classSpaceClear,outputClassSpaceFrame,NULL);
}

double ttGetClassValue(int H2, int H1){
    return cvGetReal2D(ttGetClassSpaceImage(),H2,H1);
}

double ttGetAndDrawClassValue(int theH1,int theH2){
        double classValue=0;
        classValue=ttGetClassValue((int)theH2,(int)theH1);
		ttResetOutputClassSpaceFrame();
        cvCircle(outputClassSpaceFrame,cvPoint((int)theH1,(int)theH2),25,CV_RGB(255,255,255),1,8,0);
        cvCircle(outputClassSpaceFrame,cvPoint((int)theH1,(int)theH2),15,CV_RGB(0,0,0),1,8,0);
        cvCircle(outputClassSpaceFrame,cvPoint((int)theH1,(int)theH2),5,CV_RGB(255,255,255),1,8,0);
        cvCircle(outputClassSpaceFrame,cvPoint((int)theH1,(int)theH2),3,CV_RGB(0,0,0),1,8,0);
        
        gui_t* gui=get_gui();
        //if(gui->classImageWidgetReq==1){		
            GdkPixbuf * pixbuf;
            pixbuf = gdk_pixbuf_new_from_data((uint8_t*)outputClassSpaceFrame->imageData,
                GDK_COLORSPACE_RGB,
                FALSE,
                8,
                outputClassSpaceFrame->width,
                outputClassSpaceFrame->height,
                outputClassSpaceFrame->widthStep,
                NULL,
                NULL);

            gtk_image_set_from_pixbuf(GTK_IMAGE(gui->classImage),pixbuf);
        //}
        return classValue;
}

void ttHueMomentsSetup(CvHuMoments * theHu,float * theH1, float * theH2){
    
    *theH1=theHu->hu1*HU_MOMENTS_1_SCALE;
    *theH2=theHu->hu4*HU_MOMENTS_4_SCALE;

    if(*theH1>HU_MOMENTS_LIMIT) *theH1=HU_MOMENTS_LIMIT-1;
    if(*theH2>HU_MOMENTS_LIMIT) *theH2=HU_MOMENTS_LIMIT-1;
    if(*theH1<0) *theH1=0;
    if(*theH2<0) *theH2=0;
}

void ttDrawDirections(IplImage * outputFrame,int use_contours){
    int horMargin=10;
    int vertMargin=10;
    double xdir=getVControlVout(varX);
    double ydir=getVControlVout(varY);
    float phi=getManualVariable(MANUAL_PHI);
    float theta=getManualVariable(MANUAL_THETA);
    
    
    if(use_contours==1){
		if(xdir>0)
			cvCircle(outputFrame,cvPoint(horMargin,VIDEO_VERT_HEIGHT/2),1,CV_RGB(255,255,255),10,8,0);
		if(xdir<0)
			cvCircle(outputFrame,cvPoint(VIDEO_VERT_HEIGHT-horMargin,VIDEO_VERT_HEIGHT/2),1,CV_RGB(255,255,255),10,8,0);
		if(ydir>0)
			cvCircle(outputFrame,cvPoint(VIDEO_VERT_WIDTH/2,vertMargin),1,CV_RGB(255,255,255),10,8,0);
		if(ydir<0)
			cvCircle(outputFrame,cvPoint(VIDEO_VERT_WIDTH/2,VIDEO_VERT_WIDTH-vertMargin),1,CV_RGB(255,255,255),10,8,0);
			
		if(phi>0)
			cvCircle(outputFrame,cvPoint(horMargin,VIDEO_VERT_HEIGHT/2),1,CV_RGB(255,0,0),10,8,0);
		if(phi<0)
			cvCircle(outputFrame,cvPoint(VIDEO_VERT_HEIGHT-horMargin,VIDEO_VERT_HEIGHT/2),1,CV_RGB(255,0,0),10,8,0);
		if(theta>0)
			cvCircle(outputFrame,cvPoint(VIDEO_VERT_WIDTH/2,vertMargin),1,CV_RGB(255,0,0),10,8,0);
		if(theta<0)
			cvCircle(outputFrame,cvPoint(VIDEO_VERT_WIDTH/2,VIDEO_VERT_WIDTH-vertMargin),1,CV_RGB(255,0,0),10,8,0);
	}
}


int ttMain(IplImage * theFrame){
      //OPENCV
    gui_t *gui = get_gui();
    /*
#if USE_IMAGE_MANUAL_CONTROL
    //cvResetImageROI(theFrame);
    if(gui->ManualControlImage!=NULL){
        //printf("\n%d %d %d %d\n\n",gui->ManualControlImage->width,gui->ManualControlImage->depth,theFrame->width,theFrame->depth);
        cvCopy(gui->ManualControlImage,theFrame,NULL);
        PrintOnGui("ok!");
    }
    else PrintOnGui("fuck!!!");
    //printf("\nttMain1 ok!\n\n");
    //CvRect * roi=getRoi();
    //   cvSetImageROI(theFrame,*roi);

#endif//*/
 
    assert(theFrame!=NULL);
    assert(outputFrame!=NULL);
    assert(tempFrame!=NULL);
 	
    cvZero(outputFrame);
    cvZero(tempFrame);
    //cvCvtColor(theFrame,theFrame,CV_RGB2BGR);
    cvCopy(theFrame,outputFrame,NULL);
    //printf("\nTTcopy OK\n\n");
    //cvCvtColor(outputClassSpaceFrame,outputClassSpaceFrame,CV_RGB2BGR);

#if TEST_MODE_XY 	
    static int theCounter=0;
    static int xyTestModeXDir=1;
    static int xyTestModeYDir=1;
    if(theCounter>TEST_MODE_XY_TOP){
        xyTestModeXDir*=-1;
        xyTestModeYDir*=-1;
        //printf("%d\n",theCounter);
        theCounter=0;
    }
    else
        theCounter++;

    vControlUpdate(varX,TEST_MODE_XY_X*xyTestModeXDir);
    vControlUpdate(varY,TEST_MODE_XY_Y*xyTestModeYDir);

#else
    ttSegmenter(theFrame,tempFrame,gui->segColor);
    ttImprover(tempFrame,tempFrame);
    theContour=ttFindContours(tempFrame);
    if (theContour==NULL){
        PrintOnGui("no contour found!!");
        return IMG_PROC_ERROR_NO_CONTOURS_FOUND;
    }
    PrintOnGui("");

    double area=cvContourArea(theContour,CV_WHOLE_SEQ,0);
    cvContourMoments( theContour , theMoments );
    cvGetHuMoments( theMoments , theHu );
    
    float theH1;
    float theH2;
    ttHueMomentsSetup(theHu,&theH1,&theH2);

    char angt[20];
    double classValue=ttGetAndDrawClassValue((int)theH1,(int)theH2);
    if(gui->currentClassValue==classValue) 
        sprintf(angt,"%2.2f, %5.2f :D",classValue,area);
    else
        sprintf(angt,"%2.2f, %5.2f :(",classValue,area);
    cvPutText(outputFrame,angt,cvPoint(10,25),&theFont,CV_RGB(255,255,255));
 
    CvPoint centerPoint,boxCentroid,pa,pb;
    float sAngle,otroAngle;
    centerPoint=cvPoint(VIDEO_VERT_WIDTH/2,VIDEO_VERT_HEIGHT/2);
    theBox=ttFindBox(theContour);
    boxCentroid=cvPoint(theBox.center.x,theBox.center.y);//*/
    float theSide;
    if(gui->use_contours==1){
        cvCircle(outputFrame,boxCentroid,1,CV_RGB(255,255,255),1,8,0);
        cvCircle(outputFrame,boxCentroid,6,CV_RGB(0,0,255),1,8,0);
        cvCircle(outputFrame,centerPoint,1,CV_RGB(0,0,0),1,8,0);
        cvCircle(outputFrame,centerPoint,6,CV_RGB(255,0,0),1,8,0);
        cvLine(outputFrame,centerPoint,boxCentroid,CV_RGB(255,255,255),1,8,0);
        cvDrawContours(outputFrame,theContour,CV_RGB(255,255,255),CV_RGB(255,255,255),1,2,8,cvPoint(0,0));
    }
    if(theBox.size.width<theBox.size.height){
        theSide=theBox.size.width/2;
        otroAngle=-theBox.angle*PI/180+PI/2;
        pa=cvPoint(boxCentroid.x+theSide*sin(otroAngle),boxCentroid.y+theSide*cos(otroAngle));
        pb=cvPoint(boxCentroid.x-theSide*sin(otroAngle),boxCentroid.y-theSide*cos(otroAngle));
    }
    else {
    theSide=theBox.size.height/2;
        otroAngle=-theBox.angle*PI/180;
        pa=cvPoint(boxCentroid.x+theSide*sin(otroAngle),boxCentroid.y+theSide*cos(otroAngle));
        pb=cvPoint(boxCentroid.x-theSide*sin(otroAngle),boxCentroid.y-theSide*cos(otroAngle));
    }
    //theSide=theBox.size.width;
    vControlUpdate(varX,boxCentroid.x-centerPoint.x);
    vControlUpdate(varY,boxCentroid.y-centerPoint.y);


    if(gui->use_contours==1){
        sAngle=atan2(boxCentroid.y-centerPoint.y,boxCentroid.x-centerPoint.x);
        sprintf(angt,"%4.2f",otroAngle*180/PI-90);
        cvPutText(outputFrame,angt,cvPoint(theBox.center.x,theBox.center.y),&theFont,CV_RGB(0,0,0));
        cvLine(outputFrame,pa,pb,CV_RGB(0,0,0),2,8,0);//*/
        cvLine(outputFrame,boxCentroid,cvPoint(theBox.center.x+20,theBox.center.y),CV_RGB(255,0,0),1,8,0);
        cvLine(outputFrame,boxCentroid,cvPoint(theBox.center.x,20+theBox.center.y),CV_RGB(0,0,255),1,8,0);//*/
        cvLine(outputFrame,centerPoint,cvPoint(centerPoint.x+20,centerPoint.y),CV_RGB(255,0,0),1,8,0);
        cvLine(outputFrame,centerPoint,cvPoint(centerPoint.x,20+centerPoint.y),CV_RGB(0,0,255),1,8,0);//*/
    }

    vControlUpdate(varYaw,-otroAngle*180/PI+90);
#endif
    ttDrawDirections(outputFrame,gui->use_contours);
    cvZero(theFrame);
    cvCopy(outputFrame,theFrame,NULL);
    return C_OK;
}
