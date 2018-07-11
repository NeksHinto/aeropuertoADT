#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aeropuertoBack.h"

#define	SALTEA_LINEA(a) 	while(getc(a)!='\n')

#define	COD_LOCAL	0
#define COD_OACI	1
#define	COD_IATA	2
#define	DENOM		4
#define	TRAFICO 	18

#define	OACI_MAX	5
#define	LOCAL_MAX	4
#define	IATA_MAX	4
#define	CHAR_MAX	100

char * leerLinea(char linea[], FILE * archivo){
	SALTEA_LINEA(archivo); 
	linea = fgets(linea, CHAR_MAX, archivo);
	return linea;
}

void datosAeropuerto(char OACI[OACI_MAX], char codigoLocal[LOCAL_MAX], char IATA[IATA_MAX], char * descripcion, char trafico){
	FILE * archivoAero;
	archivoAero = fopen("./aeropuertos_detalle.csv", "r");
	char * lineaAero = malloc(CHAR_MAX);
	strcpy(lineaAero, leerLinea(lineaAero, archivoAero));
	char * aux = malloc(CHAR_MAX);
	aux = retornaToken(lineaAero, COD_OACI);
	while((strcmp(aux, OACILoc1))!=0){
		lineaAero = fgets(lineaAero, CHAR_MAX, archivoAero);
		strcpy(aux, retornaToken(lineaAero, COD_OACI));
	}	
	//Si llegamos hasta acá quiere decir que estamos en la línea que coincide con el OACILoc1
	strcpy(OACI, aux);
	strcpy(codigoLocal, retornaToken(lineaAero, COD_LOCAL));
	strcpy(IATA, retornaToken(lineaAero, COD_IATA));
	strcpy(descripcion, retornaToken(lineaAero, DENOM));
	trafico = (strcmp(retornaToken(lineaAero, TRAFICO), "Internacional"))==0?1:0;
	fclose(lineaAero);

	return;
}