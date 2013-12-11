#ifndef GUI_H_
#define GUI_H_

//#include "ttiofiles.h"
#include "ImgProc/ImgProc.h"
#include "cv.h"
#include <gtk/gtk.h>
#include <VP_Os/vp_os_types.h>

#define step 0.2


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
  
  int ihm_is_initialized;
  int triggerOutput;
  
  int batteryLevel;
  
  int popup;
  int fpopup;
  int fpreq;
  int preq;
  int use_contours;
  int currentClassValue;
  int segColor;//zero-based red,green,blue respectively
  
  char saveSnapshot;
  int counter;
  char configured;
  
} gui_t;
 
gui_t *get_gui();

void saveWorkspace();
 
void init_gui(int argc, char **argv);

void writeOnTextBox(char*);

C_RESULT gtkToOcv(uint8_t* data, IplImage * dst); 
#endif
