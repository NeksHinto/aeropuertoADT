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
#define	RECORRE_TOKENS		(strtok(NULL, DELIM_TOKEN))
#define	BOOLEAN_TOKEN(s)	((RECORRE_TOKENS==(s))?1:0)
#define	esNA(s)			(strcmp(s, "N/A")==0)
#define	FECHA_MAX		9


int main(int argc, char *argv[])
{
	if(argc==2 && RANGO(argv[1]))
	{
		//CREAMOS EL TAD
		aeropuertoADT registro = nuevoRegistroAero();
		if(registro==NULL){
			fprintf(stderr, "Error: No se pudo crear el nuevo registro.\n");
			return EXIT_ERR;
		}
		char lineaVuelos[CHAR_MAX] = {0};
		lineaVuelos = leerLinea(lineaVuelos, "./eana1401-1802.csv");
		int fueraDeAnio=0;
		do{
		//Buscamos línea donde ni la clasificación, ni destino y origen OACI's no sean N/A
		lineaVuelos = esLineaValida(lineaVuelos);
		//Guardamos el primer segmento hasta ; (Primer columna->FECHA)
		lineaVuelos = moverPtrColN(lineaVuelos, FECHA);
		char fecha[FECHA_MAX] = strtok(lineaVuelos, DELIM_TOKEN);
		int dia = fechaADia(fecha, &fueraDeAnio, argv[1]);
		validaFlag(dia, ERR_FECHA);
		if(!fueraDeAnio){
			//Incrementamos el movimiento en la posición del día correspondiente
			flag = registrarMovDia(registro, dia);
			validaFlag(flag, ERR_DIA);
			//Guardamos el resto de los tokens en las variables con la macro RECORRE_TOKENS
			lineaVuelos = moverPtrColN(lineaVuelos, CLASIFICACION);
			char clasificacion = BOOLEAN_TOKEN("Internacional");
			lineaVuelos = RECORRE_TOKENS;
			int aterrizaje = BOOLEAN_TOKEN("Aterrizaje");
			//Para guardar el OACI necesito ver cuál es local y cuál internacional
			char OACILocal[OACI_MAX];
			char OACIAnon[OACI_MAX];
			if(!aterrizaje){
				OACILocal = RECORRE_TOKENS; //OrigenOACI es el local
				OACIAnon = RECORRE_TOKENS;
			}
			else{
				OACIAnon = RECORRE_TOKENS;
				OACILocal = RECORRE_TOKENS; //DestinoOACI es el local
			}//Sino, me da igual porque ambos son locales
			
			//COPIAMOS LOS DATOS DEL ARCHIVO aeropuertos_detalle.csv
			char codigoLocal1[LOCAL_MAX];
			char IATA1[IATA_MAX];
			char * descripcion1;
			char trafico1;
			datosAeropuerto(OACILocal, codigoLocal1, IATA1, descripcion1, &trafico1);
			//Guardamos los datos del aeropuerto local en el TAD
			int flag = agregarAeropuerto(OACILocal, codigoLocal1, IATA1, descripcion1, trafico1);
			validaFlag(flag, ERR_AERO);
			//Guardamos los datos del Movimiento local-anonimo en el TAD
			flag = agregarMovimiento(regsitro, OACILocal, OACIAnon, clasificacion, aterrizaje);
			validaFlag(flag, ERR_MOV);

			if(clasificacion){ //Si el vuelo es de CABOTAJE -> agregamos OACIAnon como otro local
				char codigoLocal2[LOCAL_MAX];
				char IATA2[IATA_MAX];
				char * descripcion2;
				char trafico2;
				datosAeropuerto(OACIAnon, codigoLocal2, IATA2, descripcion2, &trafico2);
				int flag = agregarAeropuerto(OACIAnon, codigoLocal2, IATA2, descripcion2, trafico2);
				validaFlag(flag, ERR_AERO);
				flag = agregarMovimiento(regsitro, OACIAnon, OACILocal, clasificacion, aterrizaje);
				validaFlag(flag, ERR_MOV);
			}
			//Pasamos a leer la línea siguiente
			lineaVuelos = leerLinea(lineaVuelos, "./eana1401-1802.csv");
		}
		else
			return 0; //Terminó de crear los archivos para el año pedido
		}while(feof(lineaVuelos)==0);

		return 0;
	}
	else{
		fprintf(stderr, "Vuelva a ejecutar el programa ingresando un año entre el 2014 y 2018 inclusive.\n");
		return EXIT_ERR;
	}
}
