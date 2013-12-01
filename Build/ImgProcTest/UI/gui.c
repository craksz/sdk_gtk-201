#include <stdlib.h>
#include "gui.h"
#include "string.h"

#include "ImgProc/ImgProc.h"
#include "ttiofiles/ttiofiles.h"
#include "gamepad.h"

gui_t *gui = NULL; 

gui_t *get_gui()
{
  return gui; 
}


void init_gui(int argc, char **argv)
{
  gui = malloc(sizeof (gui_t));
  
  gui->use_contours=1;
  gui->segColor=0;
  gui->currentClassValue=9;
}
/*void saveWorkspace(){
		
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
}//*/
