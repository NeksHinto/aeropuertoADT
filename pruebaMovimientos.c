#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
		
#define	CHAR_MAX			100
#define	OACI_MAX	4
#define	LOCAL_MAX	3
#define	IATA_MAX	3
#define	DIAS_MAX	7

#define CLASIFICACION	4
#define	OACI_ORIG		6	
#define	OACI_DEST		7

#define EXIT_OK		1
#define	EXIT_ERR	0

enum DIAS {LUN=0, MAR, MIER, JUE, VIE, SAB, DOM};
enum ERRORES {ERR_AERO=1, ERR_MOV, ERR_FECHA, ERR_DIA, ERR_OACI};


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

typedef struct aeropuertoCDT * aeropuertoADT;

aeropuertoADT nuevoRegistroAero(){
	aeropuertoADT a = malloc(sizeof(struct aeropuertoCDT));
	if(a!=NULL){
		a->primero=NULL;
		a->iter=NULL;
		for(int i=0; a->vuelosSemanal[i]!=0; i++){
			a->vuelosSemanal[i]=0;
		}
	}
	return a;
}//CHECKED

void recorrerAeropuertos(aeropuertoADT a){
	a->iter = a->primero;
	return;
}//CHECK

void recorrerMovsAeropuerto(tLista primero){
	primero->aeropuerto.movimientos.iter = primero->aeropuerto.movimientos.vuelos; 
	return;
}//CHECK

int haySigAeropuerto(aeropuertoADT a){
	return a->iter!=NULL;
}//CHECK

int haySigMovimiento(tLista primero){	
	return primero->aeropuerto.movimientos.iter!=NULL;
}//CHECK

tLista sigAeropuerto(aeropuertoADT a){
	tLista aux = NULL;
	if(haySigAeropuerto(a)){
		aux = a->iter;
		a->iter = a->iter->sig;
	}
	return aux;
}//CHECK

tVuelo * sigMovimiento(tLista primero){
	tVuelo * aux = NULL;
	if(haySigMovimiento(primero)){
		aux = primero->aeropuerto.movimientos.iter;
		primero->aeropuerto.movimientos.iter = primero->aeropuerto.movimientos.iter->sig;
	}
	return aux;
}//CHECK

static 
tLista agregarAeropuertoRec(tLista primero, char OACI[], char codigoLocal[], char IATA[], char * descripcion, char trafico, int * exito){
	int c;
	if(primero==NULL || (c=strcmp(OACI, primero->aeropuerto.OACI)) < 0){
		tLista new = malloc(sizeof(*new));
		if(new!=NULL){
			strcpy(new->aeropuerto.OACI, OACI);
			strcpy(new->aeropuerto.codigoLocal, codigoLocal);
			strcpy(new->aeropuerto.IATA, IATA);
			new->aeropuerto.descripcion = malloc(sizeof(strlen(descripcion)));
			if(new->aeropuerto.descripcion!=NULL){
				strcpy(new->aeropuerto.descripcion, descripcion);
				new->aeropuerto.trafico=trafico;
				//Inicializamos los movimientos del aeropuerto recién agregado
				new->aeropuerto.movimientos.totalDespegues=0;
				new->aeropuerto.movimientos.totalAterrizajes=0;
				new->aeropuerto.movimientos.vuelos=calloc(1, sizeof(tVuelo));
				
				*exito=1;

				return new;
			}
		}
	}
	primero->sig=agregarAeropuertoRec(primero->sig, OACI, codigoLocal, IATA, descripcion, trafico, exito);

	return primero;
}//CHECKED

/*Agrega un nuevo aeropuerto a la lista. 
**Si ya existía el aeropuerto, no hace nada.
**Si lo agrega correctamente retorna 1, caso contrario retorna 0.*/
int agregarAeropuerto(aeropuertoADT a, char OACI[], char codigoLocal[], char IATA[], char * descripcion, char trafico){
	int exito = 0;
	a->primero=agregarAeropuertoRec(a->primero, OACI, codigoLocal, IATA, descripcion, trafico, &exito);

	return exito;
}//CHECKED

static
void clasificarMovimiento(tVuelo * primero, char aterrizaje){
	if(aterrizaje)
		primero->aterrizajes++;
	else
		primero->despegues++;

	return;
}

static
tVuelo * agregarMovimientoRec(tVuelo * primero, char OACISec[], char clasificacion, int * exito, char aterrizaje){
	int c;
	if(primero==NULL || (c=strcmp(OACISec, primero->OACI)) < 0 ){
		void * aux = malloc(sizeof(tVuelo));
		if(aux!=NULL){
			tVuelo * new = aux;
			strcpy(new->OACI, OACISec);
			new->clasificacion=clasificacion; //Si es 0 es porque es un vuelo de cabotaje
			clasificarMovimiento(primero, aterrizaje);
			*exito=1;
			return new;
		}
		return aux;
	}
	if(c==0){
		clasificarMovimiento(primero, aterrizaje);
		*exito=1;

		return primero;
	}
	primero->sig=agregarMovimientoRec(primero->sig, OACISec, clasificacion, exito, aterrizaje);

	return primero;
}

/*Agrega un movimiento al aeropuerto local pasado como parámetro.
**Pensado para utilizar post-agregar un aeropuerto.
**Incrementa totalAterrizajes o totalDespegues.
**El flag se define en el main, leyendo el archivo:
**Si se trata de un aterrizaje, aterrizaje = 1, sino aterrizaje = 0.*/
int agregarMovimiento(aeropuertoADT a, char OACILocal[], char OACISec[], char clasificacion, char aterrizaje){
	int exito=0;
	recorrerAeropuertos(a);
	while(strcmp(OACILocal, a->iter->aeropuerto.OACI)!=0 && haySigAeropuerto(a)){
		a->iter = sigAeropuerto(a);
	}
	if(aterrizaje)
		a->iter->aeropuerto.movimientos.totalAterrizajes++;
	else
		a->iter->aeropuerto.movimientos.totalDespegues++;

	a->iter->aeropuerto.movimientos.vuelos=agregarMovimientoRec(a->iter->aeropuerto.movimientos.vuelos, OACISec, clasificacion, &exito, aterrizaje);

	return exito;
}


int main(){
	aeropuertoADT a = nuevoRegistroAero();
	char OACI[OACI_MAX] = "SAEZ";
	char codigoLocal[LOCAL_MAX] = "ABC";
	char IATA[IATA_MAX]="DEF";
	char * descripcion = "EZEIZA";
	char trafico = 0;
	int flag = agregarAeropuerto(a, OACI, codigoLocal, IATA, descripcion, trafico);
	printf("OACI aeropuerto local: %s\n", a->primero->aeropuerto.OACI);
	printf("Codigo Local: %s\n", a->primero->aeropuerto.codigoLocal);
	printf("IATA: %s\n", a->primero->aeropuerto.IATA);
	printf("Nombre: %s\n", a->primero->aeropuerto.descripcion);
	printf("Trafico: %d\n", a->primero->aeropuerto.trafico);
	
	//Agrego movimiento en SAEZ
	char OACISec[OACI_MAX]="BESP";
	char clasificacion = 1;
	char aterrizaje = 0;
	flag = agregarMovimiento(a, OACI, OACISec, clasificacion, aterrizaje);
	printf("OACI aeropuerto de la sublista: %s\n", a->primero->aeropuerto.movimientos.vuelos->OACI);
	printf("Clasificacion: %d\n", a->primero->aeropuerto.movimientos.vuelos->clasificacion);
	printf("Aterrrizajes: %d\n", a->primero->aeropuerto.movimientos.vuelos->aterrizajes);
	printf("Despegues: %d\n", a->primero->aeropuerto.movimientos.vuelos->despegues);
	printf("Flag: %d\n", flag);

	return 0;
}