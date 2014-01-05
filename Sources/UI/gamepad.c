/**
 *  \brief    gamepad handling implementation
 *  \author   Sylvain Gaeremynck <sylvain.gaeremynck@parrot.fr>
 *  \version  1.0
 *  \date     04/06/2007
 *  \warning  Subject to completion
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#include <ardrone_api.h>
#include <ardrone_tool/UI/ardrone_input.h>
#include <VP_Os/vp_os_print.h>
#include <VP_Os/vp_os_types.h>
#include "gamepad.h"
#include "UI/gui.h"




typedef struct {
  int32_t bus;
  int32_t vendor;
  int32_t product;
  int32_t version;
  char    name[MAX_NAME_LENGTH];
  char    handlers[MAX_NAME_LENGTH];
} device_t;

extern char * theString;

extern int32_t MiscVar[];

static C_RESULT add_device(device_t* device, const int32_t id);

static C_RESULT parse_proc_input_devices(FILE* f, const int32_t id);


input_device_t fpad = {
  "fuzzyPad",
  open_fpad,
  update_fpad,
  close_fpad
};

///////////////////////////////
//  FuzzyGP input functions  //
//////////////////////////////

Manual manualControl;
IplImage* zvg;// = cvCreateImage(cvSize(300,500),IPL_DEPTH_8U,3); 
FILE *datosG;
	

Manual* getManual(){
	return &manualControl;
};
IplImage* getImage(){
	return zvg;
};

void clearManual(){
	manualControl.theta=0;
	manualControl.phi=0;
	manualControl.gaz=0;
	manualControl.yaw=0;
}


float getManualVariable(ManualVars index){
    switch(index){
        case MANUAL_PHI:
            return manualControl.phi;
        case MANUAL_THETA:
            return manualControl.theta;
        case MANUAL_GAZ:
            return manualControl.gaz;
        case MANUAL_YAW:
            return manualControl.yaw;
        default:
            return 0;
    }
}


void setManualVariable(ManualVars index, float value){
    
    switch(index){
        case MANUAL_PHI:
            manualControl.phi=value;
            return;
        case MANUAL_THETA:
            manualControl.theta=value;
            return;
        case MANUAL_GAZ:
            manualControl.gaz=value;
            return;
        case MANUAL_YAW:
            manualControl.yaw=value;
            return;
        default:
            return;
    }
}



// Función para graficar;
void fuzzyGraph( fGraph *gX, IplImage *graph ){

        if(gX==NULL){ printf("\nfuzzyGraph null pointer\n\n"); exit(-1); }
	if(graph==NULL){ printf("\nfuzzyGraph graph null pointer\n\n"); exit(-1); }
	
	//char filename[50];
	int i;

	CvFont font;
	cvInitFont( &font, CV_FONT_HERSHEY_COMPLEX_SMALL, 0.5, 0.5, 0.5, 0, 8);

	if( gX -> counter == 300 || gX -> counter == 0){
		if( gX -> counter == 300 ){
			gX->counter2++;
			/*if(gX->counter2<10)
				sprintf( filename, "./graph/graph0%d.BMP", gX->counter2 );
			else
				sprintf( filename, "./graph/graph%d.BMP", gX->counter2 );
			cvSaveImage( filename, graph, 0 );//*/
		}

		gX->counter = 0;
		cvSetZero( graph );
		
		i = 0;
		for( i = 1; i < 60; i++){
			cvLine( graph, cvPoint(0,i*10-1), cvPoint(599,i*10-1), CV_RGB(20,20,20), 0, 8, 0 );			
		};

		cvLine( graph, cvPoint(0,299), cvPoint(599,299), CV_RGB(255,255,255), 2, 8, 0 );
		cvLine( graph, cvPoint(299,0), cvPoint(299,599), CV_RGB(255,255,255), 2, 8, 0 );

		cvLine( graph, cvPoint(0,149), cvPoint(599,149), CV_RGB(0,0,255), 0, 8, 0 );
		cvLine( graph, cvPoint(300,449), cvPoint(599,449), CV_RGB(0,0,255), 0, 8, 0 );

		cvPutText( graph, "Control en X", cvPoint(4,10), &font, CV_RGB(0,255,0) );
		cvPutText( graph, "Control en Y", cvPoint(304,10), &font, CV_RGB(0,255,0) );
		cvPutText( graph, "Control en Z", cvPoint(4,310), &font, CV_RGB(0,255,0) );
		cvPutText( graph, "Control en Yaw", cvPoint(304,310), &font, CV_RGB(0,255,0) );

	};
	
	CvPoint ref;
	CvPoint pos;

////////////////	X

	pos.x = gX->counter;
	pos.y = cvRound( 149 - gX->x );

	cvCircle( graph, pos, 0, CV_RGB(255,255,0), 0, 8, 0 );
	
////////////////	Y

	pos.x = 300 + gX->counter;
	pos.y = cvRound( 149 - gX->y );

	cvCircle( graph, pos, 0, CV_RGB(255,255,0), 0, 8, 0 );

////////////////	Z

	ref.x = gX->counter;
	ref.y = cvRound( 599 - gX->ref/10 );

	pos.x = gX->counter;
	pos.y = cvRound( 599 - gX->z/10 );

	cvCircle( graph, ref, 0, CV_RGB( 200, 0, 0), 0, 8, 0);
	cvCircle( graph, pos, 0, CV_RGB(255,255,0), 0, 8, 0);

////////////////	Yaw

	pos.x = 300 + gX->counter;
	pos.y = cvRound( 449 - gX->w );
	//pos.y = cvRound( 449 - ( 360 * atan2( gX->y , gX->x ) )/(2*PI)/1.5 );

	cvCircle( graph, pos, 0, CV_RGB(255,255,0), 0, 8, 0 );

////////////////
	gX->counter++;
    
    gui_t *gui=get_gui();
    if(gui->fuzzyControlGraphWidgetReq==1){	
        GdkPixbuf * pixbuf;	
  		pixbuf = gdk_pixbuf_new_from_data((const guchar*)graph->imageData,
                    GDK_COLORSPACE_RGB,
                    FALSE,  
                    8,     
                    graph->width,    
                    graph->height,     
                    graph->widthStep, 
                    NULL,    
                    NULL);
                              
        gtk_image_set_from_pixbuf(GTK_IMAGE(gui->fuzzyImage), pixbuf);
    }//*/
}
	
C_RESULT open_fpad(void){
	
	//double dG[5];
	char filename[100];
	sprintf(filename,
					"gnuplot/grafica-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%2.2f,%2.2f,%2.2f-%d.dat",
					XGAIN1,XGAIN2,XGAIN3,YGAIN1,YGAIN2,YGAIN3,ZGAIN1,ZGAIN2,ZGAIN3,WGAIN1,WGAIN2,WGAIN3,switchTop);
	datosG = fopen( filename, "w");
	fprintf( datosG, "#	Proyecto: Control de un Quadrotor por VA\n");
	fprintf( datosG, "#	Datos para la gráfica en GNUplot.\n\n");
	fprintf( datosG, "#	Error X	    	Error Y	 	Altura		Alt. Ref	Yaw\n");	

	zvg = cvCreateImage(cvSize(600,600),IPL_DEPTH_8U,3); 
	
    vControlInit();
	//definicion
	return C_OK;
}

void updateTestModeRefs(){
    static int timer=0;
    static int theFlag=0;
    if(timer<heightTestModeTop){
        if(theFlag%2==0){
            vControlSetRef(varZ,heightTestModeH1);
        }
        else{
            vControlSetRef(varZ,heightTestModeH2);
        }
        fGraphUpdateRef(vControlGetRef(varZ));
        timer++;
        //printf("%d!\n",timer);
    }
    else {
        printf("CHANGE to %0.2f !!!\n\n",vControlGetRef(varZ));
        theFlag++;
        timer=0;
    }
}

bool_t manualControlCheckAndRun(){
    if(manualControl.phi!=0||manualControl.theta!=0||manualControl.gaz!=0||manualControl.yaw!=0){
        ardrone_tool_set_progressive_cmd( 1,manualControl.phi,manualControl.theta,manualControl.gaz,manualControl.yaw,0.0,0.0);
        //printf("%.1f %.1f %.1f %.1f \n",manualControl.phi,manualControl.theta,manualControl.gaz,manualControl.yaw);
        //ardrone_at_set_led_animation(FIRE,10,1);
        //gui_t *gui=get_gui();
        //gtk_label_set_text((GtkLabel*)gui->textBox,"manualControl called");

        return TRUE;
	}
    return FALSE;
}

C_RESULT update_fpad(void){

    if(heightTestMode==1){
        updateTestModeRefs();
	}
    else if(manualControlCheckAndRun()==TRUE) 
        return C_OK;
	vControlTask();
	return C_OK;
}

C_RESULT close_fpad(void){
	return C_OK;
}

void printControlsToGui(){
    gui_t * gui=get_gui();
    
    if(gui->configured==1&&gui!=NULL){
    	gtk_label_set_text((GtkLabel*)gui->labelX,"ecole!!!!!!!!!");
	}
		
}

void vControlTask(){
    
    float zComp=1.0;
    
    fGraph * zg=getFGraph();

    fprintf( datosG, "%d\t%lf\t%lf\t%lf\t%lf\t%lf\n", zg->counter+300*zg->counter2, zg->x, zg->y, zg->z, zg->ref, zg->w );

    fuzzyGraph( zg, zvg );

    fuzzyControl();
            
    /*if(vControlGetVout(varZ)<0){
        zComp=0.125;
    }//*/
    if(vControlGetVout(varY)!=0||vControlGetVout(varX)!=0){
        ardrone_tool_set_progressive_cmd( 1,-(float)vControlGetVout(varX),-(float)vControlGetVout(varY),zComp*(float)vControlGetVout(varZ),0.0,0.0,0.0);
    }
    else{
        ardrone_tool_set_progressive_cmd( 0,0,0,zComp*(float)vControlGetVout(varZ),0,0,0);
    }
    
}

static int32_t make_id(device_t* device)
{
  return ( (device->vendor << 16) & 0xffff0000) | (device->product & 0xffff);
}

static C_RESULT add_device(device_t* device, const int32_t id_wanted)
{
  int32_t id = make_id(device);

  if( id_wanted == GAMEPAD_LOGICTECH_ID && id == id_wanted)
  {
		PRINT("Input device %s found\n", device->name);
    strncpy(gamepad.name, device->handlers, MAX_NAME_LENGTH);
    return C_OK;
	}

	if(id_wanted == RADIO_GP_ID && id==id_wanted)
	{
		PRINT("Input device %s found\n", device->name);
    strncpy(radioGP.name, device->handlers, MAX_NAME_LENGTH);
    return C_OK;
  }

    if(id_wanted == GAMEPAD_PLAYSTATION3_ID && id==id_wanted)
	{
		PRINT("PS3 : Input device %s found\n", device->name);
        strncpy(ps3pad.name, device->handlers, MAX_NAME_LENGTH);
        return C_OK;
  }

  return C_FAIL;
}





/** simple /proc/bus/input/devices generic LL(1) parser **/

#define KW_MAX_LEN 64

typedef enum {
  KW_BUS,
  KW_VENDOR,
  KW_PRODUCT,
  KW_VERSION,
  KW_NAME,
  KW_HANDLERS,
  KW_MAX
} keyword_t;

typedef enum {
  INT,
  STRING,
} value_type_t;

typedef struct {
  const char*   name;
  value_type_t  value_type;
  int32_t       value_offset;
} kw_tab_entry_t;

static int current_c;
static int next_c; // look ahead buffer

static device_t current_device;

static const int separators[] = { ' ',  ':', '=', '\"', '\n' };
static const int quote = '\"';
static const int eol = '\n';

static kw_tab_entry_t kw_tab[] = {
  [KW_BUS]      = {  "Bus",      INT,    offsetof(device_t, bus)       },
  [KW_VENDOR]   = {  "Vendor",   INT,    offsetof(device_t, vendor)    },
  [KW_PRODUCT]  = {  "Product",  INT,    offsetof(device_t, product)   },
  [KW_VERSION]  = {  "Version",  INT,    offsetof(device_t, version)   },
  [KW_NAME]     = {  "Name",     STRING, offsetof(device_t, name)      },
  [KW_HANDLERS] = {  "Handlers", STRING, offsetof(device_t, handlers)  }
};

static const char* handler_names[] = {
  "js0",
  "js1",
  "js2",
  "js3",
  0
};

static bool_t is_separator(int c)
{
  int32_t i;
  bool_t found = FALSE;

  for( i = 0; i < sizeof separators && !found; i++ )
  {
    found = ( c == separators[i] );
  }

  return found;
}

static bool_t is_quote(int c)
{
  return c == quote;
}

static bool_t is_eol(int c)
{
  return c == eol;
}

static C_RESULT fetch_char(FILE* f)
{
  C_RESULT res = C_FAIL;

  current_c = next_c;

  if( !feof(f) )
  {
    next_c = fgetc(f);
    res = C_OK;
  }

  // PRINT("current_c = %c, next_c = %c\n", current_c, next_c );

  return res;
}

static C_RESULT parse_string(FILE* f, char* str, int32_t maxlen)
{
  int32_t i = 0;
  bool_t is_quoted = is_quote(current_c);

  if( is_quoted )
  {
    while( SUCCEED(fetch_char(f)) && ! ( is_separator(current_c) && is_quote(current_c) ) )  {
      str[i] = current_c;
      i++;
    }
  }
  else
  {
    while( SUCCEED(fetch_char(f)) && !is_separator(current_c) )  {
      str[i] = current_c;
      i++;
    }
  }

  str[i] = '\0';
  // PRINT("parse_string: %s\n", str);

  return is_eol( current_c ) ? C_FAIL : C_OK;
}

static C_RESULT parse_int(FILE* f, int32_t* i)
{
  C_RESULT res = C_OK;
  int value;

  *i = 0;

  while( !is_separator(next_c) && SUCCEED(fetch_char(f)) && res == C_OK )  {
    value = current_c - '0';

    if (value > 9 || value < 0)
    {
      value = current_c - 'a' + 10;
      res = (value > 0xF || value < 0xa) ? C_FAIL : C_OK;
    }

    *i *= 16;
    *i += value;
  }

  return res;
}

static C_RESULT skip_line(FILE* f)
{
  while( !is_eol(next_c) && SUCCEED(fetch_char(f)) );

  return C_OK;
}

static C_RESULT match_keyword( const char* keyword, keyword_t* kw )
{
  int32_t i;
  C_RESULT res = C_FAIL;

  for( i = 0; i < KW_MAX && res != C_OK; i++ )
  {
    res = ( strcmp( keyword, kw_tab[i].name ) == 0 ) ? C_OK : C_FAIL;
  }

  *kw = i-1;

  return res;
}

static C_RESULT match_handler( void )
{
  int32_t i = 0;
  bool_t found = FALSE;

  while( !found && handler_names[i] != 0 )
  {
    found = strcmp( (char*)((char*)&current_device + kw_tab[KW_HANDLERS].value_offset), handler_names[i] ) == 0;

    i ++;
  }

	if(found)
	{
		strcpy(current_device.handlers, handler_names[i-1]);
	}

  return found ? C_OK : C_FAIL;
}

static C_RESULT parse_keyword( FILE* f, keyword_t kw )
{
  C_RESULT res = C_OK;

  while( is_separator(next_c) && SUCCEED(fetch_char(f)) );

  switch( kw_tab[kw].value_type ) {
    case INT:
      parse_int( f, (int32_t*)((char*)&current_device + kw_tab[kw].value_offset) );
      //PRINT("%s = %x\n", kw_tab[kw].name, *(int32_t*)((char*)&current_device + kw_tab[kw].value_offset) );
      break;

    case STRING:
      parse_string( f, (char*)((char*)&current_device + kw_tab[kw].value_offset), KW_MAX_LEN );
      //PRINT("%s = %s\n", kw_tab[kw].name, (char*)((char*)&current_device + kw_tab[kw].value_offset) );
      break;

    default:
      res = C_FAIL;
      break;
  }

  return res;
}

static C_RESULT parse_I(FILE* f)
{
  char keyword[KW_MAX_LEN];

  while( SUCCEED(fetch_char(f)) && is_separator(next_c) );

  while( !is_eol(next_c) ) {
    keyword_t kw;

    parse_string( f, keyword, KW_MAX_LEN );
    if( SUCCEED( match_keyword( keyword, &kw ) ) )
    {
      parse_keyword( f, kw );
    }
  }

  return C_OK;
}

static C_RESULT parse_N(FILE* f)
{
  char keyword[KW_MAX_LEN];

  while( SUCCEED(fetch_char(f)) && is_separator(next_c) );

  while( !is_eol(next_c) ) {
    keyword_t kw;

    parse_string( f, keyword, KW_MAX_LEN );
    if( SUCCEED( match_keyword( keyword, &kw ) ) )
    {
      parse_keyword( f, kw );
    }
  }


  return C_OK;
}

static C_RESULT parse_H(FILE* f)
{
  C_RESULT res = C_FAIL;
  char keyword[KW_MAX_LEN];

  while( SUCCEED(fetch_char(f)) && is_separator(next_c) );

  while( !is_eol(next_c) ) {
    parse_string( f, keyword, KW_MAX_LEN );
    if( strcmp( keyword, kw_tab[KW_HANDLERS].name ) == 0 )
    {
      while( FAILED(res) && SUCCEED( parse_string(f,
                                                  (char*)((char*)&current_device + kw_tab[KW_HANDLERS].value_offset),
                                                  KW_MAX_LEN ) ) )
      {
        res = match_handler();
      }
    }
  }

  return res;
}

static C_RESULT end_device(const int32_t id)
{
  C_RESULT res = C_FAIL;
  res=add_device(&current_device, id);
  vp_os_memset( &current_device, 0, sizeof(device_t) );

  return res;
}

static C_RESULT parse_proc_input_devices(FILE* f, const int32_t id)
{
  C_RESULT res = C_FAIL;

  next_c = '\0';
  vp_os_memset( &current_device, 0, sizeof(device_t) );

  while( res != C_OK && SUCCEED( fetch_char(f) ) )
  {
    switch( next_c )
    {
      case 'I': parse_I(f); break;
      case 'N': parse_N(f); break;
      case 'H': if( SUCCEED( parse_H(f) ) ) res = end_device(id); break;
      case 'P':
      case 'S':
      case 'B':
      default: skip_line(f); break;
    }
  }

  return res;
}
