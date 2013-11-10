#include "stdio.h"

typedef struct chingao{
	int num1;
	int num2;
} chingaoT;

void chinga(chingaoT *tamadre, int *tuputama){
	tamadre->num1=10;
	tamadre->num2=50;
	*tuputama=34;
}

int main(void){
	chingaoT tamadre;
	int tuputama=12;
	printf("%d\n",tuputama);
	printf("%d\n",tamadre.num1);
	printf("%d\n\n",tamadre.num2);
	chinga(&tamadre,&tuputama);
	printf("%d\n",tuputama);
	printf("%d\n",tamadre.num1);
	printf("%d\n\n",tamadre.num2);
	return 0;
}
