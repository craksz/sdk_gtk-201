#include "../Sources/UI/fuzzyControl.h"
//#include "cv.h"
//#include "highgui.h"

extern vControl X;
extern vControl Z;
extern vControl Y;
extern vControl Yaw;
	

	vControl * vcp[4];
	

void graphResponses(){
	
}

void main(void){
	
	vcp[0]=&Yaw;
	vcp[1]=&X;
	vcp[2]=&Y;
	vcp[3]=&Z;
	
	inControl(varX);
	inControl(varY);
	inControl(varZ);
	inControl(varYaw);
	vControlInit();
	
	int i,j;
	
	int max=1500;
	int puntos=1;
	for(i=0;i<max*puntos;i+=10){
		vControlSetVin(varZ,i/puntos);
		fuzzyControl();
		printf("%f %f\n",(float)i/puntos,vControlGetVout(varZ));
	}
	
	
}
