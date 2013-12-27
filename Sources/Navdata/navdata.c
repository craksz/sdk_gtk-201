#include <ardrone_tool/Navdata/ardrone_navdata_client.h>

#include <Navdata/navdata.h>
#include <stdio.h>
#include "UI/gui.h"
#include "UI/gamepad.h"

#define upTo 0


char theString[500];
static int theCounter=0;

/* Initialization local variables before event loop  */
inline C_RESULT demo_navdata_client_init( void* data )
{
  return C_OK;
}

/* Receving navdata during the event loop */
inline C_RESULT demo_navdata_client_process( const navdata_unpacked_t* const navdata )
{
	const navdata_demo_t*nd = &navdata->navdata_demo;	
	
    vControl *buffControl;
    char controlString[]="\n\nControl %s:\n\t\t\t[Ref]%4.1f\n\t\t\t[Inicial]%4.1f\n\t\t\t[Vout]%1.5f";
    if (theCounter>=upTo){
        gdk_threads_enter();
        gui_t *gui = get_gui();
        if (gui!=NULL&&gui->configured==1){
            gui->batteryLevel=nd->vbat_flying_percentage;
            if(nd->ctrl_state!=0)
                sprintf(theString,"Status: OK! \nBattery:\t%imV\nAltitude\t%i\n",gui->batteryLevel,nd->altitude);
            else
                sprintf(theString,"Status: EMERGENCY! \nBattery:\t%i\nAltitude\t%i\n",gui->batteryLevel,nd->altitude);

            gtk_label_set_text((GtkLabel*)gui->labelL,theString);
            sprintf(theString,
                    "\n\nOrientation:\n\t\t\t[Theta]%4.1f\n\t\t\t[Phi]%4.1f\n\t\t\t[Psi] %4.1f\nSpeed:\n\t\t\t[vX]%4.3f\n\t\t\t[vY] %4.3f\n\t\t\t[vZPsi] %4.3f\n",
                    nd->theta/100.0,nd->phi/100.0,nd->psi/100.0,nd->vx,nd->vy,nd->vz);
            gtk_label_set_text((GtkLabel*)gui->labelR,theString);
            buffControl=getVControl(varX);
            sprintf(theString,
                    controlString,
				"x",vControlGetRef(varX),vControlGetVin(varX),vControlGetVout(varX));
            gtk_label_set_text((GtkLabel*)gui->labelX,theString);

            buffControl=getVControl(varY);
            sprintf(theString,
                    controlString,
    				"y",vControlGetRef(varY),vControlGetVin(varY),vControlGetVout(varY));
            gtk_label_set_text((GtkLabel*)gui->labelY,theString);

            buffControl=getVControl(varYaw);
            sprintf(theString,
                    controlString,
				"yaw",vControlGetRef(varYaw),vControlGetVin(varYaw),vControlGetVout(varYaw));
            gtk_label_set_text((GtkLabel*)gui->labelW,theString);

            buffControl=getVControl(varZ);
            sprintf(theString,
                    controlString,
				"z",vControlGetRef(varZ),vControlGetVin(varZ),vControlGetVout(varZ));
            gtk_label_set_text((GtkLabel*)gui->labelZ,theString);
            //*/
        }
        gdk_threads_leave();
        theCounter=0;
    }
    else 
        theCounter++;
    //buffControl=getVControl(4);
    //buffControl->vin=nd->altitude;
	
  return C_OK;
}

/* Relinquish the local resources after the event loop exit */
inline C_RESULT demo_navdata_client_release( void )
{
  return C_OK;
}

/* Registering to navdata client */
BEGIN_NAVDATA_HANDLER_TABLE
  NAVDATA_HANDLER_TABLE_ENTRY(demo_navdata_client_init, demo_navdata_client_process, demo_navdata_client_release, NULL)
END_NAVDATA_HANDLER_TABLE

