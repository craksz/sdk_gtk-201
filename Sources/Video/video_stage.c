/*
 * @video_stage.c
 * @author marc-olivier.dzeukou@parrot.com
 * @date 2007/07/27
 *
 * ihm vision thread implementation
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <sys/time.h>
#include <time.h>

#include <VP_Api/vp_api.h>
#include <VP_Api/vp_api_error.h>
#include <VP_Api/vp_api_stage.h>
#include <VP_Api/vp_api_picture.h>
#include <VP_Stages/vp_stages_io_file.h>
#include <VP_Stages/vp_stages_i_camif.h>

#include <config.h>
#include <VP_Os/vp_os_print.h>
#include <VP_Os/vp_os_malloc.h>
#include <VP_Os/vp_os_delay.h>
#include <VP_Stages/vp_stages_yuv2rgb.h>
#include <VP_Stages/vp_stages_buffer_to_picture.h>
#include <VLIB/Stages/vlib_stage_decode.h>


#include <ardrone_tool/ardrone_tool_configuration.h>
#include <ardrone_tool/ardrone_tool.h>
#include <ardrone_tool/Com/config_com.h>
#include <gtk/gtk.h>
#include "UI/gui.h"
#include "UI/gamepad.h"
#include "ImgProc/ImgProc.h"
#include "cv.h"
#include "highgui.h"



#ifndef RECORD_VIDEO
#define RECORD_VIDEO
#endif
#ifdef RECORD_VIDEO
#include <ardrone_tool/Video/video_stage_recorder.h>
#endif

#include <ardrone_tool/Video/video_com_stage.h>

#include "Video/video_stage.h"
        
#define NB_STAGES 10
#define use_frontal 0
#define calib_mode 0
#define upTo 0
#define xyTestMode 0
#define xyTestModeX	0
#define xyTestModeY	0
#define xyTestModeTop	15
#define saveVideo 0

PIPELINE_HANDLE pipeline_handle;
int justWidth=176, justHeight=144;
static uint8_t*  pixbuf_data       = NULL;
static vp_os_mutex_t  video_update_lock = PTHREAD_MUTEX_INITIALIZER;
//static int theCounter=0;
//int theSegColor=2;
ttModels theModels;
static Manual *theControl;
IplImage* theFrame,*theGFrame;
IplImage* theBFrame, *theCFrame, *theCFrame2,*theBCFrame2;
CvBox2D theBox;
CvSeq* theContour;
CvFont theFont;
CvMoments themoments;
CvHuMoments thehu;
CvMoments *theMoments = &themoments;
CvHuMoments *theHu = &thehu;
CvVideoWriter *theWriter;
//CvMoments *theMoments;
//CvHuMoments *theHu;
	
	

C_RESULT output_gtk_stage_open( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out)
{
	ttInit(&theModels);
	
	char filename[300];
	sprintf(filename,
		"gnuplot/theVid-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%d.avi",
		XGAIN1,XGAIN2,XGAIN3,YGAIN1,YGAIN2,YGAIN3,ZGAIN1,ZGAIN2,ZGAIN3,WGAIN1,WGAIN2,WGAIN3,switchTop);
					
	if(saveVideo==1){
		theWriter=cvCreateVideoWriter(filename,CV_FOURCC('M','J','P','G'),25,cvSize(justWidth,justHeight),1);
	}
	theGFrame=cvCreateImage(cvSize(justWidth,justHeight),IPL_DEPTH_8U,1);
        theBFrame=cvCreateImage(cvSize(justWidth,justHeight),IPL_DEPTH_8U,3);
        cvInitFont(&theFont,CV_FONT_HERSHEY_PLAIN,1.0,1.0,0,1,8);
  //printf("sdfasdfasdfad\n\n");
	theCFrame=cvLoadImage("spaceClasses.bmp",0);
	theCFrame2=cvLoadImage("spaceClasses2.bmp",1);
	theBCFrame2=cvLoadImage("spaceClasses2.bmp",1);
        assert(theCFrame!=NULL);
        assert(theCFrame2!=NULL);
        assert(theBCFrame2!=NULL);
  //cvShowImage("win2",theCFrame);
  //cvWaitKey(0);
  return (SUCCESS);
}

C_RESULT output_gtk_stage_transform( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out)
{
  vp_os_mutex_lock(&video_update_lock);
  // Get a reference to the last decoded picture
  pixbuf_data      = (uint8_t*)in->buffers[0];
  vp_os_mutex_unlock(&video_update_lock);
  
  gui_t *gui = get_gui();
  theControl=getManual(); 
 	vControl *buffControl;
  
  // GdkPixbuf structure to store the displayed picture
  
	//printf("%d\n\n\n",theCFrame2->nChannels);
  //OPENCV
 	theFrame=gtkToOcv(pixbuf_data,use_frontal);
 	
 	
 		cvZero(theBFrame);
 		cvZero(theGFrame);
 		cvCvtColor(theFrame,theFrame,CV_RGB2BGR);
 		cvCopy(theFrame,theBFrame,NULL);
 		cvCopy(theBCFrame2,theCFrame2,NULL);
 		cvCvtColor(theCFrame2,theCFrame2,CV_RGB2BGR);
 		
		double ydir=0;
		double xdir=0;
 	if(xyTestMode==1){
 	
		static int theCounter=0;
		static int xyTestModeXDir=1;
		static int xyTestModeYDir=1;
		if(theCounter>xyTestModeTop){
			xyTestModeXDir*=-1;
			xyTestModeYDir*=-1;
			//printf("%d\n",theCounter);
			theCounter=0;
		}
		else
			theCounter++;
					
		buffControl=getSomeData(varX);
		buffControl->vin=xyTestModeX*xyTestModeXDir;
		xdir=buffControl->vout;
		
		buffControl=getSomeData(varY);
		buffControl->vin=xyTestModeY*xyTestModeYDir;
		ydir=buffControl->vout;
	}
	else{
		ttSegmenter(theFrame,theGFrame,gui->segColor);
		ttImprover(theGFrame,theGFrame);
		theContour=ttFindContours(theGFrame);
		if (theContour!=NULL){
			
			double area=cvContourArea(theContour,CV_WHOLE_SEQ,0);
			cvContourMoments( theContour , theMoments );
			cvGetHuMoments( theMoments , theHu );

			//assert(theMoments!=NULL);
			//assert(theHu!=NULL);
			float theH1=theHu->hu1*skale1;
			float theH2=theHu->hu4*skale4;
			
			/*theH1*=skale1;
			theH2*=skale2;//*/
			char angt[20];
			double final=0;
			
			//if(theH1<lim&&theH1>=0&&theH2>=0&&theH2<lim){
			if(theH1>lim) theH1=lim-1;
			if(theH2>lim) theH2=lim-1;
			if(theH1<0) theH1=0;
			if(theH2<0) theH2=0;
				final=cvGetReal2D(theCFrame,(int)theH2,(int)theH1);
				if(gui->currentNumber==final) 
					//sprintf(angt,"%2.2f :D",final);
					sprintf(angt,"%2.2f, %5.2f :D",final,area);
				else
					//sprintf(angt,"%2.2f :(",final);
					sprintf(angt,"%2.2f, %5.2f :(",final,area);
				cvPutText(theBFrame,angt,cvPoint(10,25),&theFont,CV_RGB(255,255,255));
				cvCircle(theCFrame2,cvPoint((int)theH1,(int)theH2),25,CV_RGB(255,255,255),1,8,0);
				cvCircle(theCFrame2,cvPoint((int)theH1,(int)theH2),15,CV_RGB(0,0,0),1,8,0);
				cvCircle(theCFrame2,cvPoint((int)theH1,(int)theH2),5,CV_RGB(255,255,255),1,8,0);
				cvCircle(theCFrame2,cvPoint((int)theH1,(int)theH2),3,CV_RGB(0,0,0),1,8,0);
			//}
			
			
			//if(gui->currentNumber==final){ 
				CvPoint p1,p2,pa,pb;
				float sAngle,otroAngle;
				p1=cvPoint(176/2,144/2);
				theBox=ttFindBox(theContour);
				p2=cvPoint(theBox.center.x,theBox.center.y);//*/
				float theSide;
				if(gui->use_contours==1){
					cvCircle(theBFrame,p2,1,CV_RGB(255,255,255),1,8,0);
					cvCircle(theBFrame,p2,6,CV_RGB(0,0,255),1,8,0);
					cvCircle(theBFrame,p1,1,CV_RGB(0,0,0),1,8,0);
					cvCircle(theBFrame,p1,6,CV_RGB(255,0,0),1,8,0);
					cvLine(theBFrame,p1,p2,CV_RGB(255,255,255),1,8,0);
					cvDrawContours(theBFrame,theContour,CV_RGB(255,255,255),CV_RGB(255,255,255),1,2,8,cvPoint(0,0));
				}
				if(theBox.size.width<theBox.size.height){
					theSide=theBox.size.width/2;
					otroAngle=-theBox.angle*PI/180+PI/2;
					pa=cvPoint(p2.x+theSide*sin(otroAngle),p2.y+theSide*cos(otroAngle));
					pb=cvPoint(p2.x-theSide*sin(otroAngle),p2.y-theSide*cos(otroAngle));
				}
				else {
				theSide=theBox.size.height/2;
					otroAngle=-theBox.angle*PI/180;
					pa=cvPoint(p2.x+theSide*sin(otroAngle),p2.y+theSide*cos(otroAngle));
					pb=cvPoint(p2.x-theSide*sin(otroAngle),p2.y-theSide*cos(otroAngle));
				}
				//theSide=theBox.size.width;
				
					buffControl=getSomeData(varX);
					buffControl->vin=p2.x-p1.x;
					
					buffControl=getSomeData(varY);
					buffControl->vin=p2.y-p1.y;
					
					
				
				//fuzzyControl( buffControl , p2.y-p1.y);//*/
				//*/
				
				/*cvRectangle(theBFrame,cvPoint(p2.x-theBox.size.width/2,p2.y-theBox.size.height/2)
															,cvPoint(p2.x+theBox.size.width/2,p2.y+theBox.size.height/2),CV_RGB(255,255,255),2,8,0);//*/
															
				if(gui->use_contours==1){
					sAngle=atan2(p2.y-p1.y,p2.x-p1.x);
					sprintf(angt,"%4.2f",otroAngle*180/PI-90);
					cvPutText(theBFrame,angt,cvPoint(theBox.center.x,theBox.center.y),&theFont,CV_RGB(0,0,0));
					cvLine(theBFrame,pa,pb,CV_RGB(0,0,0),2,8,0);//*/
					cvLine(theBFrame,p2,cvPoint(theBox.center.x+20,theBox.center.y),CV_RGB(255,0,0),1,8,0);
					cvLine(theBFrame,p2,cvPoint(theBox.center.x,20+theBox.center.y),CV_RGB(0,0,255),1,8,0);//*/
					cvLine(theBFrame,p1,cvPoint(p1.x+20,p1.y),CV_RGB(255,0,0),1,8,0);
					cvLine(theBFrame,p1,cvPoint(p1.x,20+p1.y),CV_RGB(0,0,255),1,8,0);//*/
				}
				
				
				buffControl=getSomeData(varYaw);
				buffControl->vin=-otroAngle*180/PI+90;
			//}
			//buffControl->vin=sAngle*180/PI;
			/*fuzzyControl( buffControl , sAngle);//*/
			//theCounter=0;
		}
	}
    buffControl=getSomeData(varX);
    xdir=buffControl->vout;

    buffControl=getSomeData(varY);
    ydir=buffControl->vout;
	if(gui->use_contours==1){
		if(xdir>0)
			cvCircle(theBFrame,cvPoint(10,72),1,CV_RGB(255,255,255),10,8,0);
		if(xdir<0)
			cvCircle(theBFrame,cvPoint(166,72),1,CV_RGB(255,255,255),10,8,0);
		if(ydir>0)
			cvCircle(theBFrame,cvPoint(88,10),1,CV_RGB(255,255,255),10,8,0);
		if(ydir<0)
			cvCircle(theBFrame,cvPoint(88,134),1,CV_RGB(255,255,255),10,8,0);
			
		if(theControl->phi>0)
			cvCircle(theBFrame,cvPoint(10,72),1,CV_RGB(255,0,0),10,8,0);
		if(theControl->phi<0)
			cvCircle(theBFrame,cvPoint(166,72),1,CV_RGB(255,0,0),10,8,0);
		if(theControl->theta>0)
			cvCircle(theBFrame,cvPoint(88,10),1,CV_RGB(255,0,0),10,8,0);
		if(theControl->theta<0)
			cvCircle(theBFrame,cvPoint(88,134),1,CV_RGB(255,0,0),10,8,0);
	}
  ///OPENCV
  gdk_threads_enter();
  static GdkPixbuf *pixbuf = NULL;
  static GdkPixbuf *pixbuf2 = NULL;
  static GdkPixbuf *pixbuf3 = NULL;
  IplImage *zvg;
  zvg=getImage();
  /*if(pixbuf!=NULL||pixbuf2!=NULL||pixbuf3!=NULL)
    {
      g_object_unref(pixbuf);
      pixbuf=NULL;
      g_object_unref(pixbuf2);
      pixbuf2=NULL;
      g_object_unref(pixbuf3);
      pixbuf3=NULL;
    }//*/
  //OPENCV
 	
	
 	///OPENCV//*/
  // Creating the GdkPixbuf from the transmited data
  if(saveVideo==1)
  	cvWriteFrame(theWriter,theBFrame);
  
  pixbuf = gdk_pixbuf_new_from_data((const guchar*)theBFrame->imageData,
                    GDK_COLORSPACE_RGB,
                    FALSE,   // No alpha channel
                    8,       // 8 bits per pixel
                    theFrame->width,     // Image width
                    theFrame->height,     // Image height
                    theFrame->widthStep, // New pixel every 3 bytes (3channel per pixel)
                    NULL,    // Function pointers
                    NULL);
  if(gui->preq==1){         
 		pixbuf2 = gdk_pixbuf_new_from_data((const guchar*)theCFrame2->imageData,
                    GDK_COLORSPACE_RGB,
                    FALSE,   
                    8,       
                    theCFrame2->width,     
                    theCFrame2->height,     
                    theCFrame2->widthStep, 
                    NULL,    
                    NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(gui->classImage), pixbuf2);
 	}//*/
 	
	if(gui->fpreq==1){		
  		pixbuf3 = gdk_pixbuf_new_from_data((const guchar*)zvg->imageData,
                    GDK_COLORSPACE_RGB,
                    FALSE,  
                    8,     
                    zvg->width,    
                    zvg->height,     
                    zvg->widthStep, 
                    NULL,    
                    NULL);
                    
   		//if(gui && gui->zImage){               
    		gtk_image_set_from_pixbuf(GTK_IMAGE(gui->fuzzyImage), pixbuf3);
  }//*/
  if (gui && gui->cam) // Displaying the image
    gtk_image_set_from_pixbuf(GTK_IMAGE(gui->cam), pixbuf);
  gdk_threads_leave();
  return (SUCCESS);
}

C_RESULT output_gtk_stage_close( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out)
{	
	if(saveVideo==1)
		cvReleaseVideoWriter(&theWriter);
  return (SUCCESS);
}


const vp_api_stage_funcs_t vp_stages_output_gtk_funcs =
{
  NULL,
  (vp_api_stage_open_t)output_gtk_stage_open,
  (vp_api_stage_transform_t)output_gtk_stage_transform,
  (vp_api_stage_close_t)output_gtk_stage_close
};

DEFINE_THREAD_ROUTINE(video_stage, data)
{
  C_RESULT res;

  vp_api_io_pipeline_t    pipeline;
  vp_api_io_data_t        out;
  vp_api_io_stage_t       stages[NB_STAGES];

  vp_api_picture_t picture;

  video_com_config_t              icc;
  vlib_stage_decoding_config_t    vec;
  vp_stages_yuv2rgb_config_t      yuv2rgbconf;
#ifdef RECORD_VIDEO
  video_stage_recorder_config_t   vrc;
#endif
  /// Picture configuration

	int32_t nextChannel;

  if(use_frontal==0){
		justWidth=176;
		justHeight=144;
		nextChannel = ZAP_CHANNEL_VERT;
	}
	else {
		justWidth=QVGA_WIDTH;
		justHeight=QVGA_HEIGHT;
		nextChannel = ZAP_CHANNEL_HORI;
	}

	ARDRONE_TOOL_CONFIGURATION_ADDEVENT (video_channel, &nextChannel, NULL);	


    picture.format        = PIX_FMT_YUV420P;

  picture.width         = justWidth;
  picture.height        = justHeight;
  picture.framerate     = 30;

  picture.y_buf   = vp_os_malloc( justWidth * justHeight     );
  picture.cr_buf  = vp_os_malloc( justWidth * justHeight / 4 );
  picture.cb_buf  = vp_os_malloc( justWidth * justHeight / 4 );

  picture.y_line_size   = justWidth;
  picture.cb_line_size  = justWidth / 2;
  picture.cr_line_size  = justWidth / 2;

  vp_os_memset(&icc,          0, sizeof( icc ));
  vp_os_memset(&vec,          0, sizeof( vec ));
  vp_os_memset(&yuv2rgbconf,  0, sizeof( yuv2rgbconf ));

  icc.com                 = COM_VIDEO();
  icc.buffer_size         = 100000;
  icc.protocol            = VP_COM_UDP;
  COM_CONFIG_SOCKET_VIDEO(&icc.socket, VP_COM_CLIENT, VIDEO_PORT, wifi_ardrone_ip);

  vec.width               = justWidth;
  vec.height              = justHeight;
  vec.picture             = &picture;
  vec.block_mode_enable   = TRUE;
  vec.luma_only           = FALSE;

  yuv2rgbconf.rgb_format = VP_STAGES_RGB_FORMAT_RGB24;
#ifdef RECORD_VIDEO
  vrc.fp = NULL;
#endif
  pipeline.nb_stages = 0;
  

  stages[pipeline.nb_stages].type    = VP_API_INPUT_SOCKET;
  stages[pipeline.nb_stages].cfg     = (void *)&icc;
  stages[pipeline.nb_stages].funcs   = video_com_funcs;

  pipeline.nb_stages++;

#ifdef RECORD_VIDEO
  stages[pipeline.nb_stages].type    = VP_API_FILTER_DECODER;
  stages[pipeline.nb_stages].cfg     = (void*)&vrc;
  stages[pipeline.nb_stages].funcs   = video_recorder_funcs;

  pipeline.nb_stages++;
#endif // RECORD_VIDEO
  stages[pipeline.nb_stages].type    = VP_API_FILTER_DECODER;
  stages[pipeline.nb_stages].cfg     = (void*)&vec;
  stages[pipeline.nb_stages].funcs   = vlib_decoding_funcs;

  pipeline.nb_stages++;

  stages[pipeline.nb_stages].type    = VP_API_FILTER_YUV2RGB;
  stages[pipeline.nb_stages].cfg     = (void*)&yuv2rgbconf;
  stages[pipeline.nb_stages].funcs   = vp_stages_yuv2rgb_funcs;

  pipeline.nb_stages++;

  stages[pipeline.nb_stages].type    = VP_API_OUTPUT_SDL;
  stages[pipeline.nb_stages].cfg     = NULL;
  stages[pipeline.nb_stages].funcs   = vp_stages_output_gtk_funcs;

  pipeline.nb_stages++;

  pipeline.stages = &stages[0];
 
  /* Processing of a pipeline */
  if( !ardrone_tool_exit() )
  {
    PRINT("\n   Video stage thread initialisation\n\n");

    res = vp_api_open(&pipeline, &pipeline_handle);

    if( SUCCEED(res) )
    {
      int loop = SUCCESS;
      out.status = VP_API_STATUS_PROCESSING;

      while( !ardrone_tool_exit() && (loop == SUCCESS) )
      {
          if( SUCCEED(vp_api_run(&pipeline, &out)) ) {
            if( (out.status == VP_API_STATUS_PROCESSING || out.status == VP_API_STATUS_STILL_RUNNING) ) {
              loop = SUCCESS;
            }
          }
          else loop = -1; // Finish this thread
      }

      vp_api_close(&pipeline, &pipeline_handle);
    }
    PRINT("succeed did not happen!!! \n\n");
  }

  PRINT("   Video stage thread ended\n\n");

  return (THREAD_RET)0;
}

