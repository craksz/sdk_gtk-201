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
	/*ttInit(&theModels);
	
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
  //cvWaitKey(0);//*/
  return (SUCCESS);
}

C_RESULT output_gtk_stage_transform( void *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out)
{
  vp_os_mutex_lock(&video_update_lock);
  // Get a reference to the last decoded picture
  pixbuf_data      = (uint8_t*)in->buffers[0];
  vp_os_mutex_unlock(&video_update_lock);
  
  gui_t *gui = get_gui();
    gdk_threads_enter();
  static GdkPixbuf *pixbuf=NULL;

  // GdkPixbuf structure to store the displayed picture
  
	//printf("%d\n\n\n",theCFrame2->nChannels);
  //OPENCV
  
  pixbuf = gdk_pixbuf_new_from_data(pixbuf_data,
                    GDK_COLORSPACE_RGB,
                    FALSE,   // No alpha channel
                    8,       // 8 bits per pixel
                    theFrame->width,     // Image width
                    theFrame->height,     // Image height
                    theFrame->widthStep, // New pixel every 3 bytes (3channel per pixel)
                    NULL,    // Function pointers
                    NULL);
  
  if (gui && gui->cam) // Displaying the image
    gtk_image_set_from_pixbuf(GTK_IMAGE(gui->cam), pixbuf);
  gdk_threads_leave();
  PRINT("some text\n\n");
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

