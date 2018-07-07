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

#define	BLOQUE		12

#define EXIT_OK		1

enum DIAS {LUN=0, MAR, MIER, JUE, VIE, SAB, DOM};

typedef struct vuelo{
	char OACI[OACI_MAX]; //Código OACI del aeropuerto secundario (con el que se relaciona el principal, importado de eana1401_1802.csv)
	unsigned int aterrizajes; //Origen: aeropuerto principal
	unsigned int despegues; //Origen: aeropuerto secundario
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
	unsigned int cantAeropuertosLocales;
	unsigned long int vuelosPorSemana[DIAS_MAX];
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

int movimientosAeropuerto()
{
	FILE * movsAerop = fopen("movs_aeropuerto.csv", "wt");
}


int movimientosInternacionales()
{
	FILE * movsInt = fopen("movs_internacional.csv", "wt");
}

int vuelosPorDía()
{
	FILE * vuelos = fopen("semanal.csv", "wt");

}

int detalleVuelos()
{
	FILE * detalleLocales = fopen("aerop_detalle.csv", "wt");
}


