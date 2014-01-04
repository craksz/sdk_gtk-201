#ifndef GUI_H_
#define GUI_H_

//#include "ttiofiles.h"
#include "ImgProc/ImgProc.h"
#include "UI/gamepad.h"
#include "cv.h"
#include <gtk/gtk.h>
#include <VP_Os/vp_os_types.h>

#define step 0.4


#ifndef STREAM_WIDTH
#define STREAM_WIDTH 512
#endif
#ifndef STREAM_HEIGHT
#define STREAM_HEIGHT 512
#endif



typedef enum{
    RED_COLOR=0,
    GREEN_COLOR,
    BLUE_COLOR
}SegColors;

typedef enum{
    CLASS_IMAGE_WIDGET=0,
    FUZZY_CONTROL_GRAPH_WIDGET
}popUpWindows;

typedef struct gui
{
  GtkWidget *window;
  GtkWidget *red;
  GtkWidget *green;
  GtkWidget *blue;
  GtkWidget *combo;
  GtkWidget *sw;
  GtkWidget *snap;
  GtkWidget *popw;
  GtkWidget *box;
  GtkWidget *cam;
  GtkWidget *fuzzyGraph;
  GtkWidget *labelR;
  GtkWidget *labelL;
  GtkWidget *labelX;
  GtkWidget *labelY;
  GtkWidget *labelW;
  GtkWidget *labelZ; 
  GtkWidget *hbox;
  GtkWidget *hbox2;
  GtkWidget *hbox3;
  GtkWidget *vbox2;
  GtkWidget *vbox;
  GtkWidget *classImage;
  GtkWidget *fuzzyImage;
  GtkWidget *textBox;
  
  GtkWidget *mainContainer;
  GtkWidget *containerW;
  GtkWidget *containerX;
  GtkWidget *containerY;
  GtkWidget *containerZ;
  GtkWidget *spinGainZ1;
  GtkWidget *spinGainX1;
  GtkWidget *spinGainY1;
  GtkWidget *spinGainW1;
  GtkWidget *spinGainZ2;
  GtkWidget *spinGainX2;
  GtkWidget *spinGainY2;
  GtkWidget *spinGainW2;
  GtkWidget *spinGainZ3;
  GtkWidget *spinGainX3;
  GtkWidget *spinGainY3;
  GtkWidget *spinGainW3;
  
  float gainSetBuffer[3];
  
  int ihm_is_initialized;
  int triggerOutput;
  
  int batteryLevel;
  
  int classImageWidgetInit;
  int fuzzyControlGraphWidgetInit;
  int classImageWidgetReq;
  int fuzzyControlGraphWidgetReq;
  int use_contours;
  int currentClassValue;
  int segColor;//zero-based red,green,blue respectively
  
  char saveSnapshot;
  int counter;
  char configured;
  
#if USE_IMAGE_MANUAL_CONTROL
  IplImage * ManualControlImage;
  
#endif // USE_IMAGE_MANUAL_CONTROL
  
} gui_t;
 
gui_t *get_gui();

void saveWorkspace();
 
void init_gui(int argc, char **argv);

void writeOnTextBox(char*);

void PrintOnGui(const char * theString);

C_RESULT gtkToOcv(uint8_t* data, IplImage * dst); 
#endif
