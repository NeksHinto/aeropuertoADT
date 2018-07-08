#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include "airportBack.h"

#define	OACI_MAX	4
#define	LOCAL_MAX	3
#define	IATA_MAX	3
#define	DIAS_MAX	7

#define EXIT_OK		1

enum DIAS {LUN=0, MAR, MIER, JUE, VIE, SAB, DOM};

typedef struct vuelo{
	char OACI[OACI_MAX]; //Código OACI del aeropuerto secundario (con el que se relaciona el principal, importado de eana1401_1802.csv)
	unsigned int aterrizajes; //Origen: aeropuerto principal
	unsigned int despegues; //Origen: aeropuerto secundario
	//char * aeropuertoArgentino // verdadero o falso
	struct vuelo * sig; 
}tVuelo;

typedef struct{
	unsigned int totalDespegues;
	unsigned int totalAterrizajes;
	tVuelo * vuelos;
}tMovimiento;

typedef struct{
	char OACI[OACI_MAX]; //Código OACI del aeropuerto principal (LOCAL, importado de aerop_detalle.csv)
	char codigoLocal[LOCAL_MAX];
	char IATA[IATA_MAX];
	char * descripcion;
	char * trafico;
	tMovimiento movimientos;
}tAeropuerto;

typedef struct nodo{
	tAeropuerto aeropuerto;
	struct nodo * sig;
}tNodo;

typedef tNodo * tLista;

struct aeropuertoCDT{
	tLista aeropuertosLocales;
	unsigned long int vuelosSemanal[DIAS_MAX];
};

typedef aeropuertoCDT * aeropuertoADT;

aeropuertoADT nuevoAeropuerto()
{
	return calloc(1, sizeof(struct aeropuertoCDT));
}

/*Agrega un nuevo aeropuerto a la lista. 
**Si ya existía el aeropuerto, se reemplazan los datos (se considera una actualización)
**Si lo agrega correctamente retorna 1, caso contrario retorna 0.*/
int agregaAeropuerto(aeropuertoADT aeropuerto, const char OACI[], const char codigoLocal[], const char IATA[], const char * descripcion, const char * trafico)
{
	int exito = 0;
	aeropuerto->listaAeropuerto=agregaAeropuertoRec(aeropuerto->listaAeropuerto, OACI, codigoLocal, IATA, descripcion, trafico, &exito);

	return exito;
}

static 
tLista agregaAeropuertoRec(tLista listaAeropuerto, const char OACI[], const char codigoLocal[], const char IATA[], const char * descripcion, const char * trafico, int * exito)
{
	int c;
	if(listaAeropuerto==NULL || (c=strcmp(OACI, listaAeropuerto->OACI)) < 0)
	{
		tLista new = malloc(sizeof(*new));
		if(new!=NULL)
		{
			new->OACI=OACI;
			new->codigoLocal=codigoLocal;
			new->IATA=IATA;
			new->descripcion=descripcion;
			new->trafico=trafico;
			new->movimientos=calloc(1, sizeof(*movimientos));//Valido?????????????????????

			*exito=1;

			return new;
		}
		else
		{
			perror("Error");
		}
	}
	if(c==0) //Actualiza
	{
		listaAeropuerto->codigoLocal=codigoLocal;
		listaAeropuerto->IATA=IATA;
		listaAeropuerto->descripcion=descripcion;
		listaAeropuerto->trafico=trafico;

		*exito=1;
	}
	listaAeropuerto->sig=agregaAeropuertoRec(listaAeropuerto->sig, OACI, codigoLocal, IATA, descripcion, trafico);

	return listaAeropuerto;
}

int copiaMovimientosAeropuerto(tLista listaDeAeropuertosLocales){
	FILE * archivoP; //puntero que apunta a un archivo 
	fd=fopen("movs_aeropuerto.csv", "w"); //crea un archivo nombre "movs_aeropuerto(...)" y recibe un string "w" que permite que se escriba en el archivo creado
	tLista l=listaDeAeropuertosLocales;


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Recorremos toda la lista de aeropuertos locales y unicamente si el aeropuerto tuvo movimiento copiamos
	los datos al archivo creado*////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	while(l!=NULL){
		if((l->aeropuerto->movimiento->totalDespegues !=0) || (l->aeropuerto->movimiento->totalAterrizajes !=0)) //NO debe contener aeropuertos con cero movimientos en el año pedido
			fprintf(fd, "%s;%s;%s;%d\n", "l->aeropuerto->OACI","l->aeropuerto->codigoLocal", "l->aeropuerto->descripcion", l->aeropuerto->totalDespegues + l->aeropuerto->totalAterrizajes);
		l=l->sig;
	}

	fclose(fd); //cierra el stream 
	return 1;
}

int movimientosAeropuerto(aeropuertoADT aeropuerto)
{
	return copiaMovimientosAeropuerto(aeropuerto->aeropuertosLocales);
}


int copiaMovimientosInternacionales(tLista listaDeAeropuertosLocales){
	int at, desp; 
	
	tVuelo *vueloP;
	               
	Tlista l=listaDeAeropuertosLocales;
	FILE * achivoP;//puntero que apunta a un archivo
	archivoP=fopen("movs_internacional.csv", "w");//creamos un archivo "movs_in(...)" y recibe "w" que permite la escritura en el archivo
	
	//Recorremos la lista de aeropuertos locales/principal
	while(l!=NULL){
		//Verificamos que el aeropuerto principal/local sea internacional
		if(l->aeropuerto->trafico==1){
			do{
				//Verificamos que el aeropuerto DESTINO no sea local/Argentino 
				if((vueloP=l->aeropuerto->movimientos->vuelos)->aeropuertoArgentino==0){
					fprintf(archivoP, "%s;%s;%d;%d;%d\n", l->aeropuerto->OACI, l->aeropuerto->IATA, desp=vueloP->aterrizajes, at=vueloP->despegues, desp+at);
		
				vueloP=vueloP->sig;
				}
			}while(vueloP!=NULL);
		}

		l=l->sig; 
	}
	fclose(archivoP);//cerramos el stream
	return 1;
}


int movimientosInternacionales(aeropuertoADT aero)
{
	return copiaMovimientosInternacionales(aero->aeropuertosLocales);
}

int vuelosPorDía()
{
	FILE *archivoP;
	archivoP=fopen("semanal.csv", "w");
		fprintf(archivoP, "LUNES: %d\n", aero->vuelosSemanal[LUN]);
		fprintf(archivoP, "MARTES: %d\n", aero->vuelosSemanal[MAR]);
		fprintf(archivoP, "MIERCOLES: %d\n", aero->vuelosSemanal[MIER]);
		fprintf(archivoP, "JUEVES: %d\n", aero->vuelosSemanal[JUE]);
		fprintf(archivoP, "VIERNES: %d\n", aero->vuelosSemanal[VIE]);
		fprintf(archivoP, "SABADO: %d\n", aero->vuelosSemanal[SAB]);
		fprintf(archivoP, "DOMINGO: %d\n", aero->vuelosSemanal[DOM]);

	fclose(archivoP);
	return 1;
}

int copiaDetallesVuelo(tLista listaDeAeropuertosLocales){
	tLista l= listaDeAeropuertosLocales;
	FILE *archivoP; //puntero a un archivo;
	archivoP=fopen("aerop_detalle.csv", "w");
	struct vuelo *vueloP; 

	while(l!=NULL){
		fprintf(archivoP, "%s;%s;%d;%d\n", l->aeropuerto->OACI, (vueloP= l->aeropuerto->movimiento->vuelo)->OACI, vueloP->aterrizajes; vueloP->despegues);
	l=l->sig;
	}

	fclose(archivoP);
	return 1;
}

int detalleVuelos(aeropuertoADT aero)
{
	return copiaDetallesVuelo(aero->aeropuertosLocales);
}

