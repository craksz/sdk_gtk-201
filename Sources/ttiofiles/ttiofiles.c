#include "ttiofiles.h"


char* substring(const char* str, size_t begin, size_t len)
{

  if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len))
    return 0;

  return strndup(str + begin, len);
} 

int saveVars(const char *theFile, var_t *vars,int mode){
	if(theFile==NULL){ printf("\nsaveVars null pointer\n\n"); exit(-1); }
	if(vars==NULL){ printf("\nsaveVars vars null pointer\n\n"); exit(-1); }
	
	int n=0;
	char theString[500]="";
	FILE *fileObj;
	
	switch(mode){
	case 1://re-write
		fileObj=fopen(theFile,"a");
		break;
	default:
		fileObj=fopen(theFile,"w");
		break;
	}
		//printf("\n%d\n",strcmp(vars[n].theString,"yeahhhhh"));
	while (strcmp(vars[n].theString,"EOF")!=0){
		//printf("%d\t",n);
		sprintf(theString,"%s%s=%0.4f\n",theString,vars[n].theString,vars[n].theInt);
		//printf("%s\n",theString);
		n++;
	}
  if (fileObj!=NULL){
    fputs (theString,fileObj);
    fclose (fileObj);
  }
	//std::fstream f (theFile,std::ios::out);
	/*theFile=fopen("somefile.dat","a");
	
	if (!f.is_open()){
     std::cout << "File not found: " << theFile << std::endl;
     return -1;
  }
  
  while (vars[n].theInt){
  	f << vars[n].theString << "=" << vars[n].theInt << std::endl;
  	n++;
  }//*/
	//f.close();
	return 0;
};

int loadVars(const char *theFile, var_t *vars){

  //long int theInt;
    if(vars==NULL){ printf("\nloadVars null pointer\n\n"); exit(-1); }
	
	char theData[600];
	char *tempData;
	size_t strp,strp2;
	int n=0;
	FILE *fileObj;
	
	fileObj=fopen(theFile,"r");
        if (fileObj==NULL) { printf ("loadVars File not found"); exit(-1); }
	while(fgets(theData, 300, fileObj) != NULL){
	
	 if(strlen(theData)>3){
		//printf("\n\n%s%d\n",theData,n);
	 strp=strcspn(theData,"=")+1;
	 strp2=strcspn(theData,"\n");
	 
	 tempData=substring(theData,strp,strp2-strp);
	 //printf("chingau!!\n");
	 sscanf (tempData, "%f", &vars[n].theInt);
	 tempData=substring(theData,0,strp-1);
	 strcpy(vars[n].theString,tempData);
	 
	 //printf ("%s==%f\n", vars[n].theString, vars[n].theInt);
	 n++;
	 }
	 else {
	 	continue;
	 	
	 }
  }
  if (n==0){
  	printf("no seg values\n\n");
  	exit(0);
  }
  
  fclose(fileObj);
  return n;//*/
	
};
