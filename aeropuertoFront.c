#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "airportBack.h"

#define	LIMPIA_BUFFER	while(getchar()!='\n')
#define	CHAR_MAX		100
#define	DELIM			';'
#define	SALTO_LINEA		'\n'

/*Lee línea del archivo en la que está posicionado el puntero linea*/
char * leerLinea(char * linea, char * path){
	FILE * archivo;
	archivo = fopen(path, "r");
	validaArchivo(archivo);
	//Inicializamos un string donde guardaremos la línea a leer
	char linea[CHAR_MAX] = {0};
	//Salteamos la primer linea (Encabezados de las columnas)
	char c;
	while(c!=SALTO_LINEA){
		c = fgetc(archivo); //Devuelve el siguiente char y avanza la posición del puntero al archivo
	}
	//Copiamos la línea	
	linea = fgets(linea, CHAR_MAX, archivo);

	return linea;
}












