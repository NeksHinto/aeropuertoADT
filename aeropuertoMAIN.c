#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include "aeropuertoFront.h"
#include "aeropuertoBack.h"

#define RANGO(c)			(strcmp(c, "2014")==0 || strcmp(c, "2015")==0 || strcmp(c, "2016")==0 || strcmp(c, "2017")==0 || strcmp(c, "2018")==0)	
#define	RECORRE_TOKENS		(strtok(NULL, DELIM_TOKEN))
#define	BOOLEAN_TOKEN(s)	((RECORRE_TOKENS==(s))?1:0)
#define	esNA(s)				(strcmp(s, "N/A")==0)
#define	FECHA_MAX			9
#define	CHAR_MAX			100

enum ERRORES {ERR_AERO=1, ERR_MOV, ERR_FECHA, ERR_DIA, ERR_OACI};


int main(int argc, char *argv[])
{
	if(argc==2 && RANGO(argv[1])){
		int flag;
		aeropuertoADT registro = nuevoRegistroAero();
		FILE * archivoVuelos;
		archivoVuelos = fopen("./eana1401-1802.csv", "r");
		char * lineaVuelos = malloc(CHAR_MAX);
		char * lineaVuelosAux = malloc(CHAR_MAX);
		//Guardo la primer linea del archivo vuelos
		strcpy(lineaVuelos, leerLinea(lineaVuelos, archivoVuelos));
		strcpy(lineaVuelosAux, lineaVuelos);
		do{
			if(validaLinea(lineaVuelosAux)){
				;
			}
			else{
				int fueraDeAnio=0;
				char * fecha = malloc(DIAS_MAX);
				strcpy(fecha, retornaToken(lineaVuelos, FECHA));
				int dia = fechaADia(fecha, &fueraDeAnio, argv[1]);
				validaFlag(dia, ERR_DIA);
				if(!fueraDeAnio){
					flag = registraMovDia(registro, dia);
					validaFlag(flag, ERR_DIA);
					char clasificacion = (strcmp(retornaToken(lineaVuelos, CLASIFICACION), "Cabotaje"))==0?0:1;
					int aterrizaje = (strcmp(retornaToken(lineaVuelos, TIPO_MOV), "Despegue"))==0?0:1;
					if(!aterrizaje){
						strcpy(OACILocal, retornaToken(lineaVuelos, OACI_ORIG));
						strcpy(OACIAnon, retornaToken(lineaVuelos, OACI_DEST));
					else{
						strcpy(OACILocal, retornaToken(lineaVuelos, OACI_DEST));
						strcpy(OACIAnon, retornaToken(lineaVuelos, OACI_ORIG));
					}
					esDesconocido(OACIAnon);
					char * codigoLocal1 = malloc(LOCAL_MAX);
					char * IATA1 = malloc(IATA_MAX);
					char * descripcion1 = malloc(CHAR_MAX);
					char trafico1;
					datosAeropuerto(OACILocal, codigoLocal1, IATA1, descripcion1, &trafico1);
					int flag = agregarAeropuerto(OACILocal, codigoLocal1, descripcion1, trafico1);
					validaFlag(flag, ERR_AERO);
					flag = agregarMovimiento(regsitro, OACILocal, OACIAnon, clasificacion, aterrizaje);
					validaFlag(flag, ERR_MOV);
					if(clasificacion){
						char * codigoLocal2 = malloc(LOCAL_MAX);
						char * IATA2 = malloc(IATA_MAX);
						char * descripcion2 = malloc(CHAR_MAX);
						char trafico2;
						datosAeropuerto(OACIAnon, codigoLocal2, IATA2, descripcion2, &trafico2);
						int flag = agregarAeropuerto(OACIAnon, codigoLocal2, IATA2, descripcion2, trafico2);
						validaFlag(flag, ERR_AERO);
						flag = agregarMovimiento(regsitro, OACIAnon, OACILocal, clasificacion, aterrizaje);
						validaFlag(flag, ERR_MOV);
					}
				}
				lineaVuelos=fgets(lineaVuelos, CHAR_MAX, archivoVuelos);
			}while(!feof(archivoVuelos));
			fclose(archivoVuelos);

			flag = creaArchivos(registro, "movs_aeropuerto.csv", "movs_internacional.csv", "semanal.csv", "aerop_detalle.csv");
			validaFlag(flag, ERR_ARCH);
		}	
		return 0;
	}
	else{
		fprintf(stderr, "Vuelva a ejecutar el programa ingresando un año entre el 2014 y 2018 inclusive.\n");
		return EXIT_ERR;
	}
}