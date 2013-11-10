#include "../Sources/ttiofiles/ttiofiles.c"
#define writeMode 1
//test code for ttiofiles

int main (void) {
	const char *theFile = "input.txt";
	var_t theVars[30];
	int total;
int i;
	
	if (writeMode==1){
		strcpy(theVars[0].theString,"yeahhhh");
		theVars[0].theInt=12;
		strcpy(theVars[1].theString,"fuck yeah");
		theVars[1].theInt=15;
		strcpy(theVars[2].theString,"ecole");
		theVars[2].theInt=0;
		strcpy(theVars[3].theString,"yeahhhh");
		theVars[3].theInt=12;
		strcpy(theVars[4].theString,"fuck yeah");
		theVars[4].theInt=15.1503;
		strcpy(theVars[5].theString,"ecole");
		theVars[5].theInt=150578;
		strcpy(theVars[6].theString,"yeahhhh");
		theVars[6].theInt=12;
		strcpy(theVars[7].theString,"fuck yeah");
		theVars[7].theInt=15;
		strcpy(theVars[8].theString,"ecole");
		theVars[8].theInt=150578;
	
		strcpy(theVars[9].theString,"EOF");
		saveVars(theFile,theVars,0);
	}
	
	total=loadVars(theFile,theVars);
	for(i=0;i<total;i++){
		printf("%s : %0.4f \n",theVars[i].theString,theVars[i].theInt);
	
	}//*/
	return 0;
};
