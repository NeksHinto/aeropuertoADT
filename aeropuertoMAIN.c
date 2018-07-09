#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include "airportFront.h"
#include "airportBack.h"

#define RANGO(c)		(strcmp(c, "2014")==0 || strcmp(c, "2015")==0 || strcmp(c, "2016")==0 || strcmp(c, "2017")==0 || strcmp(c, "2018")==0)	
#define VALIDA(c)		(isalpha(c) && c!=NULL)
#define	RECORRE_TOKENS		(strtok(NULL, DELIM))	
#define	FECHA_MAX		9

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
		char IATA[IATA_MAX] = RECORRE_TOKENS;//MAL, ME TENGO QUE SALTEAR COLUMNAS(Lo veo mañana)!!!!!!!!!!!!!!!!!!!!!!
		char * descripcion = RECORRE_TOKENS;
		char * trafico = ((RECORRE_TOKENS=="Internacional")?1:0); //HACER MACRO(SE REPITE MAS ABAJO)
		
		//Guardamos los datos del aeropuerto en el TAD
		int flag = agregarAeropuerto(OACI, codigoLocal, IATA, descripcion, trafico);
			if(!flag){
				fprintf(stderr, "No se pudo agregar el aeropuerto.\n");
				return EXIT_FAILURE;
			}

		linea = leerLinea(linea, "./aeropuertos_detalle.csv");
		}while(linea!=EOF);

		//PASAMOS LOS DATOS DEL ARCHIVO DE LOS VUELOS
		linea = leerLinea(linea, "./eana1401-1802.csv");
		do{		//MODULARIZAR ESTE DO-WHILE
		//Guardamos el primer segmento hasta ; (Primer columna->FECHA)
		char fecha[FECHA_MAX] = strtok(linea, DELIM);
		int dia = fechaADia(fecha);//(BACK)
			//HACER UN SWITCH (FCION AUX) CON LOS DIAS DE LA SEMANA (BACK)
		//Guardamos el resto de los tokens en las variables con la macro RECORRE_TOKENS
		char OACI[OACI_MAX] = RECORRE_TOKENS; //Chequear que esto funque
		char clasificacion = ((RECORRE_TOKENS=="Internacional")?1:0);
		int aterrODespeg = ((RECORRE_TOKENS=="Aterrizaje")?1:0);
		/*Ver cómo hacer esto
		unsigned int despegues;
		unsigned int aterrizajes;*/
		
		//Guardamos los datos del aeropuerto en el TAD
		//clasificacion -> Si es de cabotaje o inter
		//aterrODespeg -> Flag para incrementar aterrizajes o despegues
		int flag = agregarMovimiento(registo->primero->vuelos, OACI, clasificacion, aterrizaje);
		if(!flag){
			fprintf(stderr, "No se pudo agregar el movimiento.\n");
			return EXIT_FAILURE;
		}

		linea = leerLinea(linea, "./aeropuertos_detalle.csv");
		}while(linea!=EOF);
	}
	else{
		fprintf(stderr, "Vuelva a ejecutar el programa ingresando un año entre el 2014 y 2018 inclusive.\n");
		return EXIT_FAILURE;
	}

	return 0;
}
