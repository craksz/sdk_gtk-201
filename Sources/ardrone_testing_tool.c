/**
 * @file main.c
 * @author sylvain.gaeremynck@parrot.com
 * @date 2009/07/01
 */
#include <ardrone_testing_tool.h>

//ARDroneLib
#include <ardrone_tool/ardrone_version.h>
#include <ardrone_tool/Navdata/ardrone_navdata_client.h>
#include <ardrone_tool/ardrone_tool_configuration.h>
//#include <ardrone_tool/ardrone_time.h>
#include <ardrone_tool/Control/ardrone_control.h>
#include <ardrone_tool/UI/ardrone_input.h>
#include <ardrone_tool/Video/video_stage.h>

//Common
#include <config.h>
#include <ardrone_api.h>

//VP_SDK
#include <ATcodec/ATcodec_api.h>
#include <VP_Os/vp_os_print.h>
#include <VP_Api/vp_api_thread_helper.h>
#include <VP_Os/vp_os_signal.h>

//Local project
#include "Video/ihm_stages_o_gtk.h"
#include <UI/gamepad.h>
#include <Video/video_stage.h>
#include "UI/gui.h"
#include "ImgProc/ImgProc.h"

#define customPad 1

#ifndef STREAM_WIDTH
#define STREAM_WIDTH 512
#endif
#ifndef STREAM_HEIGHT
#define STREAM_HEIGHT 512
#endif

vp_stages_latency_estimation_config_t vlat;

vp_stages_gtk_config_t gtkconf;

extern video_decoder_config_t vec;
extern vp_stages_draw_trackers_config_t draw_trackers_cfg;

static int32_t exit_ihm_program = 1;

#define FILENAMESIZE (256)
char encodedFileName[FILENAMESIZE] = {0};


int exit_program = 1;
/*
vp_stages_latency_estimation_config_t vlat;
//vp_stages_gtk_config_t gtkconf;

extern video_decoder_config_t vec;
//extern vp_stages_draw_trackers_config_t draw_trackers_cfg;
extern const vp_api_stage_funcs_t vp_stages_output_gtk_funcs;
//*/


DEFINE_THREAD_ROUTINE(imgproc, data) /* imgproc is the routine's name */
{
  return (THREAD_RET)0;
}

DEFINE_THREAD_ROUTINE(ttiofiles, data) /* imgproc is the routine's name */
{
  return (THREAD_RET)0;
}




/* Implementing Custom methods for the main function of an ARDrone application */
int main(int argc, char** argv)
{
    printf("\n\nTHIS IS MAIN RUNNING !\n\n");
        return ardrone_tool_main(argc, argv);
}


/* The delegate object calls this method during initialization of an ARDrone application */
C_RESULT ardrone_tool_init_custom(void)
{
  
  if (customPad)
	  ardrone_tool_input_add( &fpad );
	  
  //init_gui(0, NULL); /// Creating the GUI 
	START_THREAD(gui, NULL); // Starting the GUI thread 
	START_THREAD(imgproc,NULL);
	START_THREAD(ttiofiles,NULL);

    ardrone_application_default_config.video_channel = ZAP_CHANNEL_VERT;
   // ARDRONE_TOOL_CONFIGURATION_ADDEVENT (video_channel, &ZAP_CHANNEL_VERT, NULL);	

    
    /************************ VIDEO STAGE CONFIG ******************************/

    #define NB_NAVIGATION_POST_STAGES   10
    uint8_t post_stages_index = 0;

    //Alloc structs
    specific_parameters_t * params             = (specific_parameters_t *)vp_os_calloc(1,sizeof(specific_parameters_t));
    specific_stages_t * navigation_pre_stages  = (specific_stages_t*)vp_os_calloc(1, sizeof(specific_stages_t));
    specific_stages_t * navigation_post_stages = (specific_stages_t*)vp_os_calloc(1, sizeof(specific_stages_t));
    vp_api_picture_t  * in_picture             = (vp_api_picture_t*) vp_os_calloc(1, sizeof(vp_api_picture_t));
    vp_api_picture_t  * out_picture            = (vp_api_picture_t*) vp_os_calloc(1, sizeof(vp_api_picture_t));

    in_picture->width          = STREAM_WIDTH;
    in_picture->height         = STREAM_HEIGHT;

    out_picture->framerate     = 20;
    out_picture->format        = PIX_FMT_RGB24;
    out_picture->width         = STREAM_WIDTH;
    out_picture->height        = STREAM_HEIGHT;

    out_picture->y_buf         = vp_os_malloc( STREAM_WIDTH * STREAM_HEIGHT * 3 );
    out_picture->cr_buf        = NULL;
    out_picture->cb_buf        = NULL;

    out_picture->y_line_size   = STREAM_WIDTH * 3;
    out_picture->cb_line_size  = 0;
    out_picture->cr_line_size  = 0;

    //Alloc the lists
    navigation_pre_stages->stages_list  = NULL;
    navigation_post_stages->stages_list = (vp_api_io_stage_t*)vp_os_calloc(NB_NAVIGATION_POST_STAGES,sizeof(vp_api_io_stage_t));

    //Fill the POST-stages------------------------------------------------------
    vp_os_memset(&vlat,         0, sizeof( vlat ));
    vlat.state = 0;
    vlat.last_decoded_frame_info = (void*)&vec;
    navigation_post_stages->stages_list[post_stages_index].name    = "(latency estimator)";
    navigation_post_stages->stages_list[post_stages_index].type    = VP_API_FILTER_DECODER;
    navigation_post_stages->stages_list[post_stages_index].cfg     = (void*)&vlat;
    navigation_post_stages->stages_list[post_stages_index++].funcs = vp_stages_latency_estimation_funcs;


    vp_os_memset(&draw_trackers_cfg,         0, sizeof( draw_trackers_funcs ));
    draw_trackers_cfg.last_decoded_frame_info = (void*)&vec;
    navigation_post_stages->stages_list[post_stages_index].type    = VP_API_FILTER_DECODER;
    navigation_post_stages->stages_list[post_stages_index].cfg     = (void*)&draw_trackers_cfg;
    navigation_post_stages->stages_list[post_stages_index++].funcs   = draw_trackers_funcs;


    vp_os_memset(&gtkconf,         0, sizeof( gtkconf ));
    gtkconf.rowstride               = out_picture->width * 3;
    gtkconf.last_decoded_frame_info = (void*)&vec;
    gtkconf.desired_display_width   = 0;  /* auto */
    gtkconf.desired_display_height  = 0;  /* auto */
    gtkconf.gdk_interpolation_mode  = 0;  /* fastest */
    navigation_post_stages->stages_list[post_stages_index].name    = "(Gtk display)";
    navigation_post_stages->stages_list[post_stages_index].type    = VP_API_OUTPUT_SDL;
    navigation_post_stages->stages_list[post_stages_index].cfg     = (void*)&gtkconf;
    navigation_post_stages->stages_list[post_stages_index++].funcs   = vp_stages_output_gtk_funcs;

    //Define the list of stages size
    navigation_pre_stages->length  = 0;
    navigation_post_stages->length = post_stages_index;

    params->in_pic = in_picture;
    params->out_pic = out_picture;
    params->pre_processing_stages_list  = navigation_pre_stages;
    params->post_processing_stages_list = navigation_post_stages;
    params->needSetPriority = 0;
    params->priority = 0;

    START_THREAD(video_stage, params);
    video_stage_init();
    video_stage_resume_thread ();
    
    /************************ END OF VIDEO STAGE CONFIG ***********************/
    

    return C_OK;
    
  //START_THREAD( video_stage, NULL );
  
  return C_OK;
}//*/

/* The delegate object calls this method when the event loop exit */
C_RESULT ardrone_tool_shutdown_custom()
{
  /* Relinquish all threads of your application */
    JOIN_THREAD( video_stage );
	JOIN_THREAD(imgproc);
	JOIN_THREAD(gui);
	JOIN_THREAD(ttiofiles);
  /* Unregistering for the current device */
  
  if (customPad)
	  ardrone_tool_input_remove( &fpad );

  return C_OK;
}

/* The event loop calls this method for the exit condition */
bool_t ardrone_tool_exit()
{
  return exit_ihm_program == 0;
}

C_RESULT signal_exit()
{
  exit_ihm_program = 0;

  return C_OK;
}

/* Implementing thread table in which you add routines of your application and those provided by the SDK */

DEFINE_THREAD_ROUTINE(gui, data) /* gui is the routine's name */
{
  gdk_threads_enter();
    
  init_gui(0,NULL);
    
  gtk_main();
  gdk_threads_leave();
  return (THREAD_RET)0;
}

BEGIN_THREAD_TABLE
	THREAD_TABLE_ENTRY(gui, 20)
	THREAD_TABLE_ENTRY(imgproc, 20)
	THREAD_TABLE_ENTRY(ttiofiles, 20)
  THREAD_TABLE_ENTRY( ardrone_control, 20 )
  THREAD_TABLE_ENTRY( navdata_update, 20 )
  THREAD_TABLE_ENTRY( video_stage, 20 )
END_THREAD_TABLE

