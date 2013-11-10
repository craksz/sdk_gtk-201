#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#include "../Sources/ttiofiles/ttiofiles.c"

#define GNUPLOT_PATH "/usr/bin/gnuplot"
#define BUILD_PATH "/home/alexis/ARDrone_SDK_Version_1_8_20110726/Examples/Linux/sdk_gtk/Build"
#define datos "grafica"

int main (int argc, const char * argv[]) {

	FILE *gpG;

	var_t theVars[30];
	char *filename[40];
	int i = 0;
	for(i = 0; i < 40; i++)
		filename[i] = "0";
		
	
	//printf("asdfasdfad\n");
	int total=loadVars("./gnuplot/list",theVars);
	printf("%d\n",total);
	for(i=0;i<total;i++){
		//strcpy(filename[i],theVars[i].theString);
		filename[i]=theVars[i].theString;
		//printf("%s : %0.4f : %s\n",theVars[i].theString,theVars[i].theInt,filename[i]);
	
	}
	

	//crea la tubería
	gpG = popen(GNUPLOT_PATH, "w");
	// comprueba si se ha creado
	if(gpG == NULL){
		fprintf(stderr, "No existe %s.", GNUPLOT_PATH);
		exit(EXIT_FAILURE);
	}
	// establece la dirección del script
	fprintf(gpG, " cd '%s/gnuplot'\n",BUILD_PATH );
	
	//printf("$$$$$$$\n");
	i = 0;
	while( filename[i] != "0" ){
		//printf("%s\n",filename[i]);
		fprintf(gpG, " call 'scriptG.p' '%s' \n", filename[i]);
		i++;
	}
	//printf("%%%%%%\n");
	fflush(gpG); // flush para el buffer
	pclose(gpG);

	return 0;
}
