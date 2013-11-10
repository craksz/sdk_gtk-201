#include <string.h>
#include "stdlib.h"
#include "stdio.h"

/*#include <string>
#include <fstream>
#include <iostream> // for std::cout*/

typedef struct {
	float 	theInt;
	char 	theString[100];
}var_t;
  
typedef var_t theCell[10];



char* substring(const char* str, size_t begin, size_t len);

int saveVars(const char *theFile, var_t *vars,int mode);

int loadVars(const char *theFile, var_t *vars);
