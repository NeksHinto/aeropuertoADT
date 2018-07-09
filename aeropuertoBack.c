#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include "airportFront.h"
#include "airportBack.h"

#define RANGO(c)			(strcmp(c, "2014")==0 || strcmp(c, "2015")==0 || strcmp(c, "2016")==0 || strcmp(c, "2017")==0 || strcmp(c, "2018")==0)	
#define VALIDA(c)			(isalpha(c) && c!=NULL)
#define	RECORRE_TOKENS		(strtok(NULL, DELIM))
#define	BOOLEAN_TOKEN(s)	((RECORRE_TOKENS==(s))?1:0)
#define	FECHA_MAX			9
#define	SALTO1				1
#define	SALTO2				14
#define	SALTO3				2			

int main(int argc, char *argv[])
{
	if(argc==2 && RANGO(argv[1]))
	{
		//CREAMOS EL TAD
		aeropuertoADT registro = nuevoRegistroAero();
		if(registro==NULL){
			fprintf(stderr, "Error: No se pudo crear el nuevo registro.\n");
			exit(EXIT_FAILURE);
		}

		//PASAMOS LOS DATOS DEL ARCHIVO aeropuertos_detalle.csv
		char * linea = leerLinea(linea, "./aeropuertos_detalle.csv");
		
		do{		//MODULARIZAR ESTE DO-WHILE
		//Guardamos el primer segmento hasta ; (Primer columna)
		char codigoLocal[LOCAL_MAX] = strtok(linea, DELIM);
		
		//Guardamos el resto de los tokens en las variables con la macro RECORRE_TOKENS
		char OACI[OACI_MAX] = RECORRE_TOKENS;
		char IATA[IATA_MAX] = RECORRE_TOKENS;
		//Salteamos columnas innecesarias
		moverPtrNCols(linea, SALTO1);
		char * descripcion = RECORRE_TOKENS;
		moverPtrNCols(linea, SALTO2);
		char * trafico = BOOLEAN_TOKEN("Internacional");
		
		//Guardamos los datos del aeropuerto en el TAD
		int flag = agregarAeropuerto(OACI, codigoLocal, IATA, descripcion, trafico);
		validaFlag(flag, ERR_AERO);
		//Pasamos a leer la línea siguiente
		linea = leerLinea(linea, "./aeropuertos_detalle.csv");
		}while(linea!=EOF);

		//PASAMOS LOS DATOS DEL ARCHIVO DE LOS VUELOS
		linea = leerLinea(linea, "./eana1401-1802.csv");
		do{		//MODULARIZAR ESTE DO-WHILE
		//Guardamos el primer segmento hasta ; (Primer columna->FECHA)
		char fecha[FECHA_MAX] = strtok(linea, DELIM);
		int dia = fechaADia(fecha);
		validaFlag(dia, ERR_FECHA);
		//Incrementamos el movimiento en la posición del día correspondiente
		flag = registrarMovDia(dia);
		validaFlag(flag, ERR_DIA);
		//Guardamos el resto de los tokens en las variables con la macro RECORRE_TOKENS
		moverPtrNCols(linea, SALTO3);
		char OACI[OACI_MAX] = RECORRE_TOKENS;
		char clasificacion = BOOLEAN_TOKEN("Internacional");
		int aterrODespeg = BOOLEAN_TOKEN("Aterrizaje");

		//Guardamos los datos del Movimiento en el TAD
		flag = agregarMovimiento(regsitro->primero, OACI, clasificacion, aterrODespeg);
		validaFlag(dia, ERR_MOV);
		
		//Pasamos a leer la línea siguiente
		linea = leerLinea(linea, "./aeropuertos_detalle.csv");
		}while(linea!=EOF);
	}
	else{
		fprintf(stderr, "Vuelva a ejecutar el programa ingresando un año entre el 2014 y 2018 inclusive.\n");
		return EXIT_FAILURE;
	}

	return 0;
}
