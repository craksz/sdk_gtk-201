#include "gtk/gtk.h"
#include "stdio.h"
#include "cv.h"
#include "highgui.h"
#include "ttcontours.h"


//static GtkWidget *entry;
static int theColor=0;
//static int segValues[6];

static gboolean kill_window(GtkWidget *widget,GdkEvent *event, gpointer data){
	//gtk_main_quit();
	exit(0);
	return FALSE;
}
static void pressed(GtkWidget *widget, gpointer data){
	//gtk_main_quit();
	theColor++;
	theColor%=3;
	printf("switched!\n\n");

}

int main(void){
	/*g_thread_init(NULL);
  gdk_threads_init();*/
  
		ttModels theModels;
		ttInit(&theModels);
		
		//printf("%d,%d\n",segValues[2],segValues[3]);

	GtkWidget *window;
	GtkWidget *image, *image2;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *button;
	IplImage *theFrame, *segmented, *theImage;
	CvCapture* theCamera;
	theCamera=cvCaptureFromCAM(-1);
		
	CvSize size=cvSize(cvGetCaptureProperty(theCamera,CV_CAP_PROP_FRAME_WIDTH),cvGetCaptureProperty(theCamera,CV_CAP_PROP_FRAME_HEIGHT));
  //theFrame=cvLoadImage("colormap.jpg",1);
  segmented=cvCreateImage(size,IPL_DEPTH_8U,1);
  theFrame=cvCreateImage(size,IPL_DEPTH_8U,3);
  theImage=cvCreateImage(size,IPL_DEPTH_8U,3);
	char theChar;
	//theImage=cvLoadImage("colormap.jpg",1);
	
		//printf("chingao\n");
	gtk_init(0,NULL);
	
	static GdkPixbuf *pixbuf;
 	static GdkPixbuf *pixbuf2;
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	vbox=gtk_vbox_new(FALSE,2);
	hbox=gtk_hbox_new(FALSE,2);
	image=gtk_image_new();
	image2=gtk_image_new();
	button=gtk_button_new_with_label("convive");
	
	gtk_window_set_title(GTK_WINDOW(window),"gtk application");
	
	g_signal_connect(window,"delete_event",G_CALLBACK(kill_window),NULL);
	g_signal_connect(button,"clicked",G_CALLBACK(pressed),NULL);
	
	gtk_box_pack_start(GTK_BOX(hbox),image,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox),image2,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,FALSE,2);
	gtk_container_add(GTK_CONTAINER(window),vbox);

	/*gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();*/
 	while (1){
	
	//gtk_main_quit();
		//gdk_threads_enter();
	
		theImage=cvQueryFrame(theCamera);
		assert(theImage!=NULL);
		cvCopy(theImage,theFrame,NULL);
		ttSegmenter(theFrame,segmented,theColor);
		cvZero(theFrame);
		cvMerge(segmented,segmented,segmented,NULL,theFrame);
		
		//cvConvertImage(theFrame,theFrame,CV_CVTIMG_SWAP_RB);
		cvConvertImage(theImage,theImage,CV_CVTIMG_SWAP_RB);
		//cvShowImage("window",theImage);
		//theFrame=theImage;
		
		//cvWaitKey(5000);
		
		pixbuf = gdk_pixbuf_new_from_data ((guchar*) theFrame->imageData,
																		GDK_COLORSPACE_RGB,
																		FALSE,
																		theFrame->depth,
																		theFrame->width,
																		theFrame->height,
																		(theFrame->widthStep),
																		NULL,
																		NULL); 
		pixbuf2 = gdk_pixbuf_new_from_data ((guchar*) theImage->imageData,
																		GDK_COLORSPACE_RGB,
																		FALSE,
																		theImage->depth,
																		theImage->width,
																		theImage->height,
																		(theImage->widthStep),
																		NULL,
																		NULL); 
                    
	//printf("\n\nchingadamadre!\n");CV_CVTIMG_SWAP_RB
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image2), pixbuf2);
	//	cvConvertImage(theImage,theImage,CV_CVTIMG_SWAP_RB);
		//gdk_threads_leave();
 
	
	gtk_widget_show_all(window);
	

		cvWaitKey(5);
		cvZero(theFrame);
		//printf("chi\n");
		//printf("eso!");	
	}
	//	gtk_main();
	
	return 0;
}	
