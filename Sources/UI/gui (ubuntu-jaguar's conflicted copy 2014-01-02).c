#include <stdlib.h>
#include "gui.h"
#include "string.h"
#include "cv.h"
#include "highgui.h"

#include <ardrone_tool/UI/ardrone_input.h>
#include "ImgProc/ImgProc.h"
#include "ttiofiles/ttiofiles.h"
#include <ardrone_api.h>
#include "gamepad.h"

gui_t *gui = NULL;
GtkWidget *popup_window; 
GtkWidget *fpopup_window; 




#if USE_IMAGE_MANUAL_CONTROL
IplImage * imageUp;
IplImage * imageDown;
IplImage * imageLeft;
IplImage * imageRight;
IplImage * imageCw;
IplImage * imageCcw;
IplImage * imageCenter;

#endif // USE_IMAGE_MANUAL_CONTROL



gui_t *get_gui()
{
  return gui; 
}

void saveWorkspace(){
		
	var_t theVars[13];
	
		strcpy(theVars[0].theString,"XGAIN1");		theVars[0].theInt=XGAIN1;
		strcpy(theVars[1].theString,"XGAIN2");		theVars[1].theInt=XGAIN2;
		strcpy(theVars[2].theString,"XGAIN3");		theVars[2].theInt=XGAIN3;
		strcpy(theVars[3].theString,"YGAIN1");		theVars[3].theInt=YGAIN1;
		strcpy(theVars[4].theString,"YGAIN2");		theVars[4].theInt=YGAIN2;
		strcpy(theVars[5].theString,"YGAIN3");		theVars[5].theInt=YGAIN3;
		strcpy(theVars[6].theString,"ZGAIN1");		theVars[6].theInt=ZGAIN1;
		strcpy(theVars[7].theString,"ZGAIN2");		theVars[7].theInt=ZGAIN2;
		strcpy(theVars[8].theString,"ZGAIN3");		theVars[8].theInt=ZGAIN3;
		strcpy(theVars[9].theString,"WGAIN1");		theVars[9].theInt=WGAIN1;
		strcpy(theVars[10].theString,"WGAIN2");		theVars[10].theInt=WGAIN2;
		strcpy(theVars[11].theString,"WGAIN3");		theVars[11].theInt=WGAIN3;
		strcpy(theVars[12].theString,"battery");		theVars[12].theInt=gui->batteryLevel;
	
		strcpy(theVars[13].theString,"EOF");
		saveVars("userValues.dat",theVars,0);
}
/* If the drone is landed, only start is clickable,
   if the drone is in the air, only stop is clickable
*/
static void keyPressed(GtkWidget *widget,GdkEventKey *kevent, gpointer data){
	static int value = 1;
    
    if(kevent->type == GDK_KEY_PRESS){  
    
        switch(kevent->keyval){
            case '2': 
                if(kevent->type == GDK_KEY_PRESS)  {
                ardrone_tool_set_ui_pad_start(value);
                value = (value + 1) % 2;
    //	   	 		printf("++%d++\n",value);
                }
                return;
#if USE_IMAGE_MANUAL_CONTROL

            case 'w':
                gui->ManualControlImage=imageUp;
                return;
 		 	case 's':
                gui->ManualControlImage=imageDown;
				return;
 		 	case 'a':
                gui->ManualControlImage=imageLeft;
				return;
 		 	case 'd':
                gui->ManualControlImage=imageRight;
				return;
 		 	case 'j':
                gui->ManualControlImage=imageCcw;
				return;
 		 	case 'l':
                gui->ManualControlImage=imageCw;
				return;
#else
            case 'w':
                setManualVariable(MANUAL_THETA,-step);
    //  			ctrlBuff->theta=-step;
                return;
 		 	case 's':
                setManualVariable(MANUAL_THETA,step);
//  			ctrlBuff->theta=step;
				return;
 		 	case 'a':
                setManualVariable(MANUAL_PHI,-step);
//  			ctrlBuff->phi=-step;
				return;
 		 	case 'd':
                setManualVariable(MANUAL_PHI,step);
//  			ctrlBuff->phi=step;
				return;
 		 	case 'j':
                setManualVariable(MANUAL_YAW,step);
//  			ctrlBuff->yaw=step;
				return;
 		 	case 'l':
                setManualVariable(MANUAL_YAW,-step);
//  			ctrlBuff->yaw=-step;
				return;
#endif // USE_IMAGE_MANUAL_CONTROL
 		 	case 'i':
                setManualVariable(MANUAL_GAZ,step);
//  			ctrlBuff->gaz=step;
				return;
 		 	case 'k':
                setManualVariable(MANUAL_GAZ,-step);
//  			ctrlBuff->gaz=-step;
				return;
 		 	case '+':
                vControlUpdateRef(varZ,vControlGetRef(varZ)+100);
 //		 		printf("newZvalue=%2.2f\n",buffControl->Ref);
				return;
 		 	case '-':
                vControlUpdateRef(varZ,vControlGetRef(varZ)-100);
 //		 		printf("newZvalue=%2.2f\n",buffControl->Ref);
				return;
			default:
				//printf("not found\n");
			//ardrone_at_set_led_animation(KEY_OK,1,1);
				break;
		}
        
	}
		 
	if(kevent->keyval=='1'){
    	if(kevent->type == GDK_KEY_PRESS) 
        	ardrone_tool_set_ui_pad_select(1);
        else ardrone_tool_set_ui_pad_select(0);
        return;
  	}
	if(kevent->type==GDK_KEY_RELEASE){
#if USE_IMAGE_MANUAL_CONTROL
        gui->ManualControlImage=imageCenter;
#endif //USE_IMAGE_MANUAL_CONTROL
        clearManual();
    }
	
}
////////////////// 

gboolean on_popup_focus_out (GtkWidget *widget,GdkEventFocus *event,gpointer data){

	//gtk_container_remove(GTK_CONTAINER(widget),gui->classImage);
	int t=(int)	data;
	
	//printf("ASDFASDFA %d",t);
  gtk_widget_hide (widget);
/*  if(t==CLASS_IMAGE_WIDGET)
    gui->classImageWidgetInit=1;
  else 
    gui->fuzzyControlGraphWidgetInit=1;//*/
  gui->fuzzyControlGraphWidgetReq=0;
  gui->classImageWidgetReq=0;
  
  return TRUE;
}

static void classImageDisplay (GtkButton* button, GtkWidget* pWindow){
	if(gui->classImageWidgetInit!=1){
        popup_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title (GTK_WINDOW (popup_window), "Pop Up window");
        gtk_container_set_border_width (GTK_CONTAINER (popup_window), 10);
        gtk_window_set_resizable (GTK_WINDOW (popup_window), FALSE);
        gtk_window_set_decorated (GTK_WINDOW (popup_window), FALSE);
        gtk_window_set_skip_taskbar_hint (GTK_WINDOW (popup_window), TRUE);
        gtk_window_set_skip_pager_hint (GTK_WINDOW (popup_window), TRUE);
        gtk_widget_set_size_request (popup_window, HU_MOMENTS_LIMIT, HU_MOMENTS_LIMIT);
        gtk_window_set_transient_for (GTK_WINDOW (popup_window), GTK_WINDOW (pWindow));
        gtk_window_set_position (GTK_WINDOW (popup_window), GTK_WIN_POS_NONE);

        gtk_widget_set_events (popup_window, GDK_FOCUS_CHANGE_MASK);
        g_signal_connect (G_OBJECT (popup_window),
                          "focus-out-event",
                          G_CALLBACK (on_popup_focus_out),
                          (gpointer)CLASS_IMAGE_WIDGET);

        GdkColor color;
        gdk_color_parse ("#000000", &color);
        gtk_widget_modify_bg (GTK_WIDGET (popup_window), GTK_STATE_NORMAL, &color);
        gtk_container_add(GTK_CONTAINER(popup_window),gui->classImage);
        gtk_widget_show_all (popup_window);
        gtk_widget_grab_focus (popup_window);
        gui->classImageWidgetInit=1;
    }
    else{
		gtk_widget_show(popup_window);
    }
	gui->classImageWidgetReq=1;
}

static void fuzzyImageDisplay (GtkButton* button, GtkWidget* pWindow)
{
	if(gui->fuzzyControlGraphWidgetInit!=1){

        fpopup_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title (GTK_WINDOW (fpopup_window), "Pop Up window");
        gtk_container_set_border_width (GTK_CONTAINER (fpopup_window), 5);
        gtk_window_set_resizable (GTK_WINDOW (fpopup_window), FALSE);
        gtk_window_set_decorated (GTK_WINDOW (fpopup_window), FALSE);
        gtk_window_set_skip_taskbar_hint (GTK_WINDOW (fpopup_window), TRUE);
        gtk_window_set_skip_pager_hint (GTK_WINDOW (fpopup_window), TRUE);
        gtk_widget_set_size_request (fpopup_window, 600, 600);
        gtk_window_set_transient_for (GTK_WINDOW (fpopup_window), GTK_WINDOW (pWindow));
        gtk_window_set_position (GTK_WINDOW (fpopup_window), GTK_WIN_POS_NONE);

        gtk_widget_set_events (fpopup_window, GDK_FOCUS_CHANGE_MASK);
        g_signal_connect (G_OBJECT (fpopup_window),
                          "focus-out-event",
                          G_CALLBACK (on_popup_focus_out),
                          (gpointer)FUZZY_CONTROL_GRAPH_WIDGET);

        GdkColor color;
        gdk_color_parse ("#ffffff", &color);
        gtk_widget_modify_bg (GTK_WIDGET (fpopup_window), GTK_STATE_NORMAL, &color);

        gtk_container_add(GTK_CONTAINER(fpopup_window),gui->fuzzyImage);
        gtk_widget_show_all (fpopup_window);
        gtk_widget_grab_focus (fpopup_window);

        gui->fuzzyControlGraphWidgetInit=1; 
    }
    else{
		gtk_widget_show(fpopup_window);
	}
	gui-> fuzzyControlGraphWidgetReq=1;
}

///////////////////////////////////////////////////////////////


static void switch_callback(GtkWidget *widget,gpointer   data ){
	gui->use_contours++;
	gui->use_contours%=2;
    return;
}

static void snap_callback(GtkWidget *widget,gpointer   data ){	
    gui->saveSnapshot=1;
    return;
}
 
static void combo_callback(GtkWidget *widget,GdkEventKey *kevent, gpointer data){
	//printf("convivir!!\n");
	gchar *theString=gtk_combo_box_get_active_text (GTK_COMBO_BOX(widget));
	printf("%s\n",theString);
	if(strcmp(theString,"c1")==0){
		gui->currentClassValue=1;
	}
	else
	if(strcmp(theString,"c2")==0){
		gui->currentClassValue=2;
	}
	else
	if(strcmp(theString,"c3")==0){
		gui->currentClassValue=3;
	}
	else
	if(strcmp(theString,"c4")==0){
		gui->currentClassValue=4;
	}
	else
	if(strcmp(theString,"c5")==0){
		gui->currentClassValue=5;
	}
	else
	if(strcmp(theString,"c6")==0){
		gui->currentClassValue=6;
	}
	else
	if(strcmp(theString,"c7")==0){
		gui->currentClassValue=7;
	}
	else
	if(strcmp(theString,"c8")==0){
		gui->currentClassValue=8;
	}
	else
	if(strcmp(theString,"c9")==0){
		gui->currentClassValue=9;
	}
	else
	if(strcmp(theString,"c0")==0){
		gui->currentClassValue=0;
	}
}
static void radio_callback(GtkWidget *widget, gpointer data){
	int t=(int)	data;
	gui->segColor=t;
	//printf("ayquir... %d\n",t);
	
}
static void on_destroy(GtkWidget *widget, gpointer data){
      saveWorkspace();
      vp_os_free(gui);
      gtk_main_quit();
      exit(0);
}
 

#ifndef STREAM_WIDTH
#define STREAM_WIDTH 512
#endif
#ifndef STREAM_HEIGHT
#define STREAM_HEIGHT 512
#endif


C_RESULT gtkToOcv(uint8_t* data, IplImage * dst){
  //IplImage *currframe;
        if(data==NULL) return C_FAIL; 
        if(dst==NULL) return C_FAIL;
        memcpy(dst->imageData,data,STREAM_WIDTH*STREAM_HEIGHT);
        //cvCvtColor(dst, dst, CV_BGR2RGB);
        return C_OK;
}


void PrintOnGui(const char * theString){
   gtk_label_set_text((GtkLabel*)gui->textBox,theString);

    
}



void init_gui(int argc, char **argv)
{
  gui = vp_os_malloc(sizeof (gui_t));
 
 	//OPENCV
	ttInitImgProc();
	
  ///OPENCV//*/
  g_thread_init(NULL);
  gdk_threads_init();
  gtk_init(&argc, &argv);
 
 	
  gui->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  //gui->ppWindow = gtk_window_new(GTK_WINDOW_POPUP);
  gui->box = gtk_vbox_new(FALSE, 10);
  gui->hbox = gtk_hbox_new(FALSE, 10);
  gui->hbox2= gtk_hbox_new(FALSE, 10);
  gui->hbox3= gtk_hbox_new(FALSE, 10);
  gui->vbox= gtk_vbox_new(FALSE, 10);
  gui->vbox2= gtk_vbox_new(FALSE, 10);
  gui->cam = gtk_image_new();
  gtk_widget_set_size_request(gui->cam,176,144);
  gui->classImage = gtk_image_new();
  gui->fuzzyImage = gtk_image_new();
  gui->combo=gtk_combo_box_new_text();
  	gtk_combo_box_append_text(GTK_COMBO_BOX(gui->combo),"c0");
  	gtk_combo_box_append_text(GTK_COMBO_BOX(gui->combo),"c1");
  	gtk_combo_box_append_text(GTK_COMBO_BOX(gui->combo),"c2");
  	gtk_combo_box_append_text(GTK_COMBO_BOX(gui->combo),"c3");
  	gtk_combo_box_append_text(GTK_COMBO_BOX(gui->combo),"c4");
  	gtk_combo_box_append_text(GTK_COMBO_BOX(gui->combo),"c5");
  	gtk_combo_box_append_text(GTK_COMBO_BOX(gui->combo),"c6");
  	gtk_combo_box_append_text(GTK_COMBO_BOX(gui->combo),"c7");
  	gtk_combo_box_append_text(GTK_COMBO_BOX(gui->combo),"c8");
  	gtk_combo_box_append_text(GTK_COMBO_BOX(gui->combo),"c9");
  	gtk_combo_box_set_active(GTK_COMBO_BOX(gui->combo),9);
  gui->red = gtk_radio_button_new_with_label(NULL,"Red");
  gui->green = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(gui->red),"Green");
  gui->blue = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(gui->red),"Blue");
  gui->popw = gtk_button_new_with_label("Classes");
  gui->sw = gtk_button_new_with_label("Contours");
  gui->snap = gtk_button_new_with_label("SnapShot!");
  gui->fuzzyGraph = gtk_button_new_with_label("Graph!");
  gui->labelR= gtk_label_new("");
  gui->labelL= gtk_label_new("");
  gui->labelX= gtk_label_new("x");
  gui->labelY= gtk_label_new("y");
  gui->labelW= gtk_label_new("w");
  gui->labelZ= gtk_label_new("z");
  gui->textBox= gtk_label_new("this is some text!");
  gui->use_contours=1;
  gui->segColor=0;
  gui->currentClassValue=9;
  gui->ihm_is_initialized=0;
  gui->triggerOutput=0;
  gui->saveSnapshot=0;
  gui->counter=0;
  
  gui-> classImageWidgetInit=0;
  gui-> fuzzyControlGraphWidgetInit=0;
  gui-> classImageWidgetReq=0;
  gui-> fuzzyControlGraphWidgetReq=0;
  
  
	 #if USE_IMAGE_MANUAL_CONTROL
        imageUp=cvLoadImage("imageUp.bmp",1);
        imageDown=cvLoadImage("imageDown.bmp",1);
        imageLeft=cvLoadImage("imageLeft.bmp",1);
        imageRight=cvLoadImage("imageRight.bmp",1);
        imageCw=cvLoadImage("imageCw.bmp",1);
        imageCcw=cvLoadImage("imageCcw.bmp",1);
        imageCenter=cvLoadImage("imageCenter.bmp",1);

        if(imageUp==NULL||imageDown==NULL||imageLeft==NULL||imageRight==NULL||imageCenter==NULL){
            //PrintOnGui("image Loading went wrong!");
            printf("\n%d %d %d %d %d\n\n",(int)imageUp,(int)imageDown,(int)imageLeft,(int)imageRight,(int)imageCenter);
            signal_exit();
            exit(-1);
        };
        cvCvtColor(imageUp,imageUp,CV_RGB2BGR);
        cvCvtColor(imageDown,imageDown,CV_RGB2BGR);
        cvCvtColor(imageLeft,imageLeft,CV_RGB2BGR);
        cvCvtColor(imageRight,imageRight,CV_RGB2BGR);
        cvCvtColor(imageCenter,imageCenter,CV_RGB2BGR);
        gui->ManualControlImage=imageCenter;
    #endif
  
  
  g_signal_connect(G_OBJECT(gui->window),"destroy",G_CALLBACK(on_destroy),NULL);
  g_signal_connect (gui->sw, "clicked",G_CALLBACK (switch_callback), NULL);
  g_signal_connect (gui->red, "clicked",G_CALLBACK (radio_callback), (gpointer)RED_COLOR);
  g_signal_connect (gui->green, "clicked",G_CALLBACK (radio_callback), (gpointer)GREEN_COLOR);
  g_signal_connect (gui->blue, "clicked",G_CALLBACK (radio_callback), (gpointer)BLUE_COLOR);
  g_signal_connect (gui->combo, "changed",G_CALLBACK (combo_callback), NULL);
  g_signal_connect (gui->snap, "clicked",G_CALLBACK (snap_callback), NULL);
  g_signal_connect (gui->popw, "clicked",G_CALLBACK (classImageDisplay), NULL);
  g_signal_connect (gui->fuzzyGraph, "clicked",G_CALLBACK (fuzzyImageDisplay), NULL);
  g_signal_connect(G_OBJECT(gui->window), "key_press_event", G_CALLBACK(keyPressed),gui->window);
  g_signal_connect(G_OBJECT(gui->window), "key_release_event", G_CALLBACK(keyPressed),gui->window);
  
  gtk_box_pack_start(GTK_BOX(gui->vbox2), gui->cam, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(gui->vbox2), gui->hbox3, TRUE, TRUE, 10);
 
	gtk_box_pack_start(GTK_BOX(gui->hbox3), gui->sw, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(gui->hbox3), gui->snap, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(gui->hbox3), gui->popw, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(gui->hbox3), gui->fuzzyGraph, TRUE, TRUE, 10);
	
  gtk_box_pack_start(GTK_BOX(gui->hbox), gui->labelL, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(gui->hbox), gui->vbox2, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(gui->hbox), gui->labelR, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(gui->hbox), gui->box, FALSE, TRUE, 10);	
  //gtk_box_pack_start(GTK_BOX(gui->hbox), gui->classImage, FALSE, TRUE, 10);	
  
  gtk_box_pack_start(GTK_BOX(gui->hbox2), gui->labelX, FALSE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(gui->hbox2), gui->labelY, FALSE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(gui->hbox2), gui->labelW, FALSE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(gui->hbox2), gui->labelZ, FALSE, TRUE, 10);
  
  gtk_box_pack_start(GTK_BOX(gui->vbox), gui->hbox, FALSE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(gui->vbox), gui->hbox2, FALSE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(gui->vbox), gui->textBox, FALSE, TRUE, 10);
  
  gtk_container_add(GTK_CONTAINER(gui->window),gui->vbox);
  gtk_box_pack_start(GTK_BOX(gui->box), gui->red, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(gui->box), gui->green, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(gui->box), gui->blue, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(gui->box), gui->combo, TRUE, TRUE, 10);
 
  
  gui->configured=1;
  gtk_widget_show_all(gui->window);
}
