#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include "airportFront.h"
#include "airportBack.h"

int main(int argc, char *argv[])
{
	if(argc==2 && (strcmp(argv[1], "2014")==0 || strcmp(argv[1], "2015")==0 || strcmp(argv[1], "2016")==0 || strcmp(argv[1], "2017")==0 || strcmp(argv[1], "2018")==0))
	{
		//Do it 
	}
	else
	{
		fprintf(stderr, "Vuelva a ejecutar el programa ingresando un año entre el 2014 y 2018 inclusive.\n");
		exit(EXIT_FAILURE);	
	}

	return 0;
}