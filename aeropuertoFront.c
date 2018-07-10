#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "airportBack.h"

#define	SALTO_LINEA			'\n'
#define	DELIM_TOKEN			";"
#define	SALTEA_LINEA(c,a)	while((c=fgetc(a))!=SALTO_LINEA)
#define	RECORRE_TOKENS		(strtok(NULL, DELIM_TOKEN))
#define	CHAR_MAX			100

enum COLS_VUELOS {FECHA=0, HORA, CLASE, CLASIFICACION, TIPO_MOV, OACI_ORIG, OACI_DEST, AEROLINEA, AERONAVE, AERO_APC}
enum AEROPUERTOS {LOCAL=0, OACI, IATA, TIPO, DENOM, COORD, LATITUD, LONGITUD, ELEV, UOM, REF, DIST, DIRECC, COND, CONTROL, REG, FIR, USO, TRAFICO, SNA, CONCESIONADO, PROV, INHAB}

/*Lee línea del archivo en la que está posicionado el puntero linea*/
char * leerLinea(char linea[CHAR_MAX], char * path){
	FILE * archivo;
	archivo = fopen(path, "r");
	//Salteamos la primer linea (Encabezados de las columnas)
	char c;
	SALTEA_LINEA(c = fgetc(archivo)); //Devuelve el siguiente char y avanza la posición del puntero al archivo
	//Copiamos la línea	
	linea = fgets(linea, CHAR_MAX, archivo);

	return linea;
}//CHEKED

void datosAeropuerto(char OACI[OACI_MAX], char codigoLocal[LOCAL_MAX], char IATA[IATA_MAX], char * descripcion, char trafico){
	char lineaAero[CHAR_MAX] = {0};
	lineaAero = moverPtrColN(leerLinea(lineaAero, "./aeropuertos_detalle.csv"), COD_OACI);
	while((strcmp(lineaAero, OACILoc1))!=0){
		char c;
		SALTEA_LINEA(c, lineaAero);
		moverPtrColN(lineaAero, COD_OACI);
	}	
	//Si llegamos hasta acá quiere decir que estamos en la línea que coincide con el OACILoc1
	lineaAero = moverPtrColN(lineaAero, LOCAL); //Volvemos a la columna 1
	//Guardamos el primer segmento hasta ; (Primer columna)
	codigoLocal[LOCAL_MAX] = strtok(lineaAero, DELIM_TOKEN);		
	//Guardamos el resto de los tokens en las variables con la macro RECORRE_TOKENS
	OACI[OACI_MAX] = RECORRE_TOKENS;
	IATA[IATA_MAX] = RECORRE_TOKENS;
	//Salteamos columnas innecesarias
	lineaAero = moverPtrColN(lineaAero, DENOM);
	descripcion = RECORRE_TOKENS;
	lineaAero = moverPtrColN(lineaAero, TRAFICO);
	trafico = BOOLEAN_TOKEN("Internacional");
	fclose(lineaAero);

	return;
}
