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
	char clasificacion; //Flag: 1 -> El OACI del nodo corresponde al de un aeropuerto INTERNACIONAL, 0 sino (NACIONAL)
	unsigned int aterrizajes; //Origen: aeropuerto principal
	unsigned int despegues; //Origen: aeropuerto secundario
	struct vuelo * sig; 
}tVuelo;

typedef struct{
	unsigned int totalDespegues;
	unsigned int totalAterrizajes;
	tVuelo * vuelos;
	tVuelo * iter; //Para recorrer la lista de vuelos
}tMovimiento;

typedef struct{
	char OACI[OACI_MAX]; //Código OACI del aeropuerto principal (LOCAL, importado de aerop_detalle.csv)
	char codigoLocal[LOCAL_MAX];
	char IATA[IATA_MAX];
	char * descripcion;
	char trafico; //Flag: 1->Aeropuerto Local tiene tráfico INTERNAC, 0 sino (Únicamente vuelos de cabotaje)
	tMovimiento movimientos;
}tAeropuerto;

typedef struct nodo{
	tAeropuerto aeropuerto;
	struct nodo * sig;
}tNodo;

typedef tNodo * tLista;

struct aeropuertoCDT{
	tLista primero;
	tLista iter; //Para recorrer la lista de aeropuertos
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
int agregaAeropuerto(aeropuertoADT a, const char OACI[], const char codigoLocal[], const char IATA[], const char * descripcion, const char * trafico)
{
	int exito = 0;
	a->primero=agregaAeropuertoRec(a->primero, OACI, codigoLocal, IATA, descripcion, trafico, &exito);

	return exito;
}

static 
tLista agregaAeropuertoRec(tLista primero, const char OACI[], const char codigoLocal[], const char IATA[], const char * descripcion, const char * trafico, int * exito)
{
	int c;
	if(primero==NULL || (c=strcmp(OACI, primero->OACI)) < 0)
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
		primero->codigoLocal=codigoLocal;
		primero->IATA=IATA;
		primero->descripcion=descripcion;
		primero->trafico=trafico;

		*exito=1;
	}
	primero->sig=agregaAeropuertoRec(primero->sig, OACI, codigoLocal, IATA, descripcion, trafico);

	return primero;
}

void freeAeropuerto(aeropuertoADT a)
{
	freeAeropuertoRec(a->primero);
	for(int i=0; i<DIAS_MAX; i++)
	{
		if(a->vuelosSemanal[i]!=NULL)
			free(a->vuelosSemanal[i]);
	}
	free(a->vuelosSemanal);
	free(a);

	return;
}

static
void freeAeropuertoRec(tLista primero)
{
	if(primero==NULL)
		return;
	freeVuelosRec(primero->sig);
	free(primero->trafico);
	free(primero);
	return;
}

static
void freeVuelosRec(tVuelo * vuelos)
{
	if(vuelos==NULL)
		return;
	freeVuelosRec(vuelos->sig);
	free(vuelos);
	return;
}

void recorrerAeropuertos(aeropuertoADT a)
{
	a->iter = a->primero;
}

void recorrerMovsAeropuerto(tLista aeropuerto)
{
	aeropuerto->iter = aeropuerto->primero; 
}

int haySigAeropuerto(aeropuertoADT a)
{
	return a->iter!=NULL;
}

int haySigMovimiento(tLista aeropuerto)
{	
	return aeropuerto->iter!=NULL;
}

tLista sigAeropuerto(aeropuertoADT a)
{
	tMovimiento auxVuelos={0,0,NULL,NULL};
	tLista aux = {0,0,0,NULL,NULL, auxVuelos};
	if(haySigAeropuerto(a))
	{
		aux = a->iter;
		a->iter = a->iter->sig;
	}
	return aux;
}

tVuelo sigMovimiento(tLista aeropuerto)
{
	tVuelo aux = {0,0,0};
	if(haySigMovimiento(aeropuerto))
	{
		aux = aeropuerto->iter;
		aeropuerto->iter = aeropuerto->iter->sig;
	}
	return aux;
}

///////////////////////////////////////////////
/*QUERY 1*/
///////////////////////////////////////////////
///////////////////////////////////////////////
int movimientosAeropuerto(aeropuertoADT a)
{
	return copiaMovimientosAeropuerto(a->aeropuerto);
}

int copiaMovimientosAeropuerto(tLista aeropuertos){
	FILE * archivoP; //puntero que apunta a un archivo 
	archivoP = fopen("movs_aeropuerto.csv", "w"); //crea un archivo nombre "movs_aeropuerto(...)" y recibe un string "w" que permite que se escriba en el archivo creado
	
	tLista l = aeropuertos;
	
	/*Recorremos toda la lista de aeropuertos locales y unicamente si el aeropuerto tuvo movimiento copiamos
	los datos al archivo creado*/
	while(l!=NULL){
		if((l->aeropuerto->movimiento->totalDespegues!=0) || (l->aeropuerto->movimiento->totalAterrizajes!=0)) //NO debe contener aeropuertos con cero movimientos en el año pedido
			fprintf(fd, "%s;%s;%s;%d\n", l->aeropuerto->OACI,l->aeropuerto->codigoLocal, l->aeropuerto->descripcion,(l->aeropuerto->movimientos->totalDespegues  +  l->aeropuerto->movimientos->totalAterrizajes));
		l=l->sig;
	}

	fclose(archivoP); //cierra el stream 
	return EXIT_OK;
}

///////////////////////////////////////////////
/*QUERY 2*/
///////////////////////////////////////////////
///////////////////////////////////////////////
int movimientosInternacionales(aeropuertoADT a){
	return copiaMovimientosInternacionales(a->aeropuerto);
}

int copiaMovimientosInternacionales(tLista aeropuertos){
	FILE * achivoP;//puntero que apunta a un archivo
	archivoP = fopen("movs_internacional.csv", "w");//creamos un archivo "movs_in(...)" y recibe "w" que permite la escritura en el archivo
	
	int at, desp; 
	
	//creamos un puntero a un struct vuelo para: acceder a los datos de cada vuelo de la lista de vuelos de un aeropuerto internacional
	tVuelo * vueloP; 
	tLista l = aeropuertos;
	
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
	return EXIT_OK;
}


///////////////////////////////////////////////
/*QUERY 3*/
///////////////////////////////////////////////
///////////////////////////////////////////////
int vuelosPorDía(){
	FILE * archivoP;
	archivoP = fopen("semanal.csv", "w");
	
	fprintf(archivoP, "LUNES: %d\n", aero->vuelosSemanal[LUN]);
	fprintf(archivoP, "MARTES: %d\n", aero->vuelosSemanal[MAR]);
	fprintf(archivoP, "MIERCOLES: %d\n", aero->vuelosSemanal[MIER]);
	fprintf(archivoP, "JUEVES: %d\n", aero->vuelosSemanal[JUE]);
	fprintf(archivoP, "VIERNES: %d\n", aero->vuelosSemanal[VIE]);
	fprintf(archivoP, "SABADO: %d\n", aero->vuelosSemanal[SAB]);
	fprintf(archivoP, "DOMINGO: %d\n", aero->vuelosSemanal[DOM]);

	fclose(archivoP);//cerramos el stream
	return EXIT_OK;
}


///////////////////////////////////////////////
/*QUERY 4*/
///////////////////////////////////////////////
///////////////////////////////////////////////
int detalleVuelos(aeropuertoADT aero)
{
	return copiaDetallesVuelo(aero->aeropuertosLocales);
}

int copiaDetallesVuelo(tLista aeropuertos){
	FILE *archivoP; //puntero a un archivo;
	archivoP = fopen("aerop_detalle.csv", "w"); //creamos archivo
	
	tLista l = aeropuertos;
	tVuelo * vueloP;

	while(l!=NULL){
		if(vueloP = l->aeropuerto->movimiento->vuelo !=NULL){
			do{
				fprintf(archivoP, "%s;%s;%d;%d\n", l->aeropuerto->OACI, vueloP->OACI, vueloP->aterrizajes; vueloP->despegues);
				vueloP=vueloP->sig;
			}while(vueloP!=NULL);
		}	
	l=l->sig;
	}

	fclose(archivoP);
	return EXIT_OK;
}
