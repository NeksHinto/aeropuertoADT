#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aeropuertoBack.h"

#define	DELIM_TOKEN			";"
#define	SALTEA_LINEA(a) 	while(getc(a)!='\n')
#define	CHAR_MAX			100

enum COLS_VUELOS {FECHA=0, HORA, CLASE, CLASIFICACION, TIPO_MOV, OACI_ORIG, OACI_DEST, AEROLINEA, AERONAVE, AERO_APC};
enum AEROPUERTOS {LOCAL=0, OACI, IATA, TIPO, DENOM, COORD, LATITUD, LONGITUD, ELEV, UOM, REF, DIST, DIRECC, COND, CONTROL, REG, FIR, USO, TRAFICO, SNA, CONCESIONADO, PROV, INHAB};


char * leerLinea(char linea[], FILE * archivo){
	//Salteamos la primer linea (Encabezados de las columnas)
	SALTEA_LINEA(archivo); //Devuelve el siguiente char y avanza la posición del puntero al archivo
	//Copiamos la línea	
	linea = fgets(linea, CHAR_MAX, archivo);

	return linea;
}//CHECKED

/*Lee del archivo "aeropuertos_detalle.csv" y copia los datos correspondientes en las variables pasadas como parámetro*/
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
