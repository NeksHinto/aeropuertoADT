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

int movAeropuerto(tLista listaDeAeropuertosLocales){
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
	return movAeropuerto(aeropuerto->aeropuertosLocales);
}


int movInternacional(tLista listaDeAeropuertosLocales){
	int aterrizajes, despegues; 
	Tlista l=listaDeAeropuertosLocales;
	FILE * achivoP;//puntero que apunta a un archivo
	fd=fopen("movs_internacional.csv", "w");//creamos un archivo "movs_in(...)" y recibe "w" que permite la escritura en el archivo
	
	//Recorremos la lista 
	while(l!=NULL){
		//////////////////////////////////////////////////////////////////////////////
		//Que son los MOVIMIENTOS INTERNACIONALESS???
		//////////////////////////////////////////////////////////////////////////////

		//Verificamos que el aeropuerto sea internacional 
		if(l->aeropuerto->trafico==1){

			//Verificamos que hubo movimiento del aeropuerto en ese año
			if((despegues= l->aeropuerto->movimientos->totalDespegues !=0) || (aterrizajes=l->aeropuerto->movimientos->totalAterrizajes != 0)){
				fprintf(archivoP, "%s;%s;%d;%d;%d\n", l->aeropuerto->OACI, l->aeropuerto->IATA, despegues, aterrizajes, despegues+aterrizajes);
			}
		}

		l=l->sig; 
	}
	fclose(archivoP);//cerramos el stream
	return 1;
}


int movimientosInternacionales(aeropuertoADT aero)
{
	return movInternacional(aero->aeropuertosLocales);
}

int vuelosPorDía()
{
	FILE * vuelos = fopen("semanal.csv", "wt");

}

int detalleVuelos()
{
	FILE * detalleLocales = fopen("aerop_detalle.csv", "wt");
}


