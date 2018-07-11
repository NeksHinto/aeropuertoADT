#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "aeropuertoBack.h"

#define	DELIM_FECHA 	"/"
#define	DELIM_TOKEN		";"
#define	RECORRE_FECHA	strtok(NULL, DELIM_TOKEN)
#define	RECORRE_TOKENS	strtok(NULL, DELIM_TOKEN)
#define TOKENS_VUELOS	7

#define EXIT_OK		1
#define	EXIT_ERR	0

enum DIAS {LUN=0, MAR, MIER, JUE, VIE, SAB, DOM};
enum ERRORES {ERR_AERO=1, ERR_MOV, ERR_FECHA, ERR_DIA, ERR_OACI};

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
}

void recorrerAeropuertos(aeropuertoADT a){
	a->iter = a->primero;
	return;
}

void recorrerMovsAeropuerto(tLista primero){
	primero->aeropuerto.movimientos.iter = primero->aeropuerto.movimientos.vuelos; 
	return;
}

int haySigAeropuerto(aeropuertoADT a){
	return a->iter!=NULL;
}

int haySigMovimiento(tLista primero){	
	return primero->aeropuerto.movimientos.iter!=NULL;
}

tLista sigAeropuerto(aeropuertoADT a){
	tLista aux = NULL;
	if(haySigAeropuerto(a)){
		aux = a->iter;
		a->iter = a->iter->sig;
	}
	return aux;
}

tVuelo * sigMovimiento(tLista primero){
	tVuelo * aux = NULL;
	if(haySigMovimiento(primero)){
		aux = primero->aeropuerto.movimientos.iter;
		primero->aeropuerto.movimientos.iter = primero->aeropuerto.movimientos.iter->sig;
	}
	return aux;
}

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
}

/*Agrega un nuevo aeropuerto a la lista. 
**Si ya existía el aeropuerto, no hace nada.
**Si lo agrega correctamente retorna 1, caso contrario retorna 0.*/
int agregarAeropuerto(aeropuertoADT a, char OACI[], char codigoLocal[], char IATA[], char * descripcion, char trafico){
	int exito = 0;
	a->primero=agregarAeropuertoRec(a->primero, OACI, codigoLocal, IATA, descripcion, trafico, &exito);

	return exito;
}

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
	if( primero==NULL || (c=strcmp(OACISec, primero->OACISec)) < 0 ){
		tVuelo * aux = malloc(sizeof(*aux));
		if(aux!=NULL){
			tVuelo * new = aux;
			strcpy(new->OACISec, OACISec);
			new->clasificacion=clasificacion; //Si es 0 es porque es un vuelo de cabotaje
			new->aterrizajes=0;
			new->despegues=0;
			clasificarMovimiento(new, aterrizaje);
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

int fechaADia(char * fecha, int * fueraDeRango, int anio){
	int d = atoi(strtok(fecha, DELIM_FECHA));
	int m = atoi(RECORRE_FECHA);
	int a = atoi(RECORRE_TOKENS);

	if(a!=anio)
		*fueraDeRango=1;

	int dia = (d+=m<3?a--:a-2,23*m/9+d+4+4/a-a/100+a/400)%7;

	return dia-1;
}

int registrarMovDia(aeropuertoADT a, int dia){
	enum DIAS d = dia;
	switch(d){
		case LUN: a->vuelosSemanal[LUN-1]++;break;
		case MAR: a->vuelosSemanal[MAR-1]++;break;
		case MIER: a->vuelosSemanal[MIER-1]++;break;
		case JUE: a->vuelosSemanal[JUE-1]++;break;
		case VIE: a->vuelosSemanal[VIE-1]++;break;
		case SAB: a->vuelosSemanal[SAB-1]++;break;
		case DOM: a->vuelosSemanal[DOM-1]++;break;
		default: return EXIT_ERR;
	}
	return EXIT_OK;
}

void esDesconocido(char OACI[]){
	int i;
	for(i=2; OACI[i]!=0 && isalpha(OACI[i]); i++)
		;
	if(i!=4)
		strcpy(OACI, "####");
	printf("%d\n", i);
	return;
}

/*QUERY1*/
static
int movimientosAeropuerto(aeropuertoADT a, FILE *archivoP){
    tAeropuerto* aeroLocal; //este puntero es para que reciba la direccion que retorna la funcion next (del aeropuerto local siguiente de la lista)
    recorrerAeropuertos(a);//El iterador apunta al primer nodo de la lista

    do{
    	aeroLocal=sigAeropuerto(a);//retorna la direccion del aeropuerto que apunta el puntero iterador y hace con que el iterador apunte al proximo aeropuerto

    	/*Asumo que solo hay aeropuertos con movimientos en la lista, por lo tanto no verifico que tenga 
    	movimientos o no, los guardo directamente hasta completar el final de la lista de aeropuertos*/
    	fprintf(archivoP, "%s;%s;%s;%d\n", aeroLocal->OACI, aeroLocal->codigoLocal, aeroLocal->descripcion, (aeroLocal->movimientos.totalDespegues+aeroLocal->movimientos.totalAterrizajes));
    }while(haySigAeropuerto(a));
    fclose(archivoP);
    return EXIT_OK;	
}


/*QUERY2*/
static
int movimientosInternacionales( aeropuertoADT a, FILE * archivoP){
	int at, desp;
	recorrerAeropuertos(a);
	tAeropuerto * aeroP;//puntero para recorrer la lista de aeropuertos principales/locales
	tVuelo * vueloP;// puntero para recorrer la lista de movimientos de un determinado aeropuerto principal/local

	do{
		aeroP=sigAeropuerto(a);//aeroP apunta al aeropuerto que apunta el iterador y el iterador pasa a apuntar al siguiente aeropuerto principal/local de la lista de aeropuertos principales

		//verificamos que el aeropuerto principal sea internacional
		if(aeroP->trafico==1){
			recorrerMovsAeropuerto(a->iter);
			/*inicializamos el iterados de la lista de movimientos del aeropuerto aeroP internacional 
			principal*/
     		do{
     			vueloP=sigMovimiento(a->iter);
     			/*vueloP apunta al nodo de la lista de vuelos que apuntaba el iter de movs. y el iter apunta
     			al proximo vuelo de la lista de movs.*/

     			//verificamos que el aeropuerto DESTINO sea internacional
     			if(vueloP->clasificacion==1){
     				fprintf(archivoP,"%s;%s;%d;%d;%d\n", aeroP->OACI, aeroP->IATA, desp=vueloP->aterrizajes, at=vueloP->despegues,desp+at );
     			}
     		}while(haySigMovimiento(a->iter));
		}
	}while(haySigAeropuerto(a));
	fclose(archivoP);//cerramos el stream
	return EXIT_OK;
}


/*QUERY3*/
static
int vuelosPorDia(aeropuertoADT * a, FILE * archivoP){
	fprintf(archivoP, "LUNES: %d\n", a.vuelosSemanal[LUN]);
	fprintf(archivoP, "MARTES: %d\n", a.vuelosSemanal[MAR]);
	fprintf(archivoP, "MIERCOLES: %d\n", a.vuelosSemanal[MIER]);
	fprintf(archivoP, "JUEVES: %d\n", a.vuelosSemanal[JUE]);
	fprintf(archivoP, "VIERNES: %d\n", a.vuelosSemanal[VIE]);
	fprintf(archivoP, "SABADO: %d\n", a.vuelosSemanal[SAB]);
	fprintf(archivoP, "DOMINGO: %d\n", a.vuelosSemanal[DOM]);
	fclose(archivoP);//cerramos el stream
	return EXIT_OK;
}


/*QUERY4*/
static
int detallesVuelo(aeropuertoADT a, FILE *archivoP){
	recorrerAeropuertos(a);
	recorrerMovsAeropuerto(a->iter);
	tLista aeroP;//para recorrer lista de aeropuertos principales
	tVuelo * vueloP;//para recorrer lista de movimientos de cada aeropuerto principal


	do{
		aeroP=sigAeropuerto(a);
		/*todos los aeropuertos tienen movimiento, no hace falta verificar*/
		do{
			vueloP=sigMovimiento(a->iter);
			fprintf(archivoP, "%s;%s;%d;%d\n", aeroP->aeropuerto.OACI, vueloP->OACISec, vueloP->aterrizajes, vueloP->despegues);
		}while(haySigMovimiento(a->iter));
	}while(haySigAeropuerto(a));
	fclose(archivoP);
	return EXIT_OK;
}

int crearArchivos(aeropuertoADT a, char * pathQuery1, char * pathQuery2, char * pathQuery3, char * pathQuery4){
	//Creamos los archivos
	FILE * archQ1;
	archQ1 = fopen(pathQuery1, "w");
	FILE * archQ2;
	archQ2 = fopen(pathQuery2, "w");
	FILE * archQ3;
	archQ3 = fopen(pathQuery3, "w");
	FILE * archQ4;
	archQ4 = fopen(pathQuery4, "w");
	//Copiamos los datos del TAD
	movimientosAeropuerto(aeropuertoADT a, archQ1);
	movimientosInternacionales(aeropuertoADT a, archQ2);
	vuelosPorDia(aeropuertoADT a, archQ3);
	detalleVuelos(aeropuertoADT a, archQ4);

	return EXIT_OK;
}

void validaArchivo(FILE * archivo)
{
	if(archivo==NULL)
	{
		fprintf(stderr, "No se pudo abrir el archivo.\n");
		return EXIT_ERR;
	}
	return;
}

static
char * strError(int tipoError)
{
	enum ERRORES error = tipoError;
	switch(error)
	{
		case ERR_AERO:	return "No se pudo agregar el aeropuerto.\n"; break;
		case ERR_MOV: 	return "No se pudo agregar el movimiento.\n"; break;
		case ERR_FECHA: return "No se pudo registrar el día de la fecha pasada como parámetro.\n"; break;
		case ERR_DIA: 	return "No se pudo registrar el movimiento en el día correspondiente.\n"; break;
		case ERR_OACI:	return "No se encontró el OACI.\n"; break;
		case ERR_ARCH:	return "No hay espacio suficiente para crear los archivos.\n"; break;
		default: 		return "Error.\n"; break;
	}
}

void validaFlag(int flag, int tipoError)
{
	if(!flag){
		char * error=strError(tipoError);
		fprintf(stderr, "%s\n", error);
	}
	return;
}

/*Retorna 1 si debe saltear la línea, 0 sino*/
int validaLinea(char * linea){
	char * aux;
	int cont = TOKENS_VUELOS;
	int saltar = 0;
	while(cont>0 && !saltar){
		__strtok_r(linea, DELIM_TOKEN, &aux);
		if(esNA(linea))
			saltar = 1;	
		strcpy(linea, aux);	
		cont--;
	}
	return saltar;
}

char * retornaToken(char * linea, int col){
	char * aux;
	char * originalStr = malloc(CHAR_MAX);
	strcpy(originalStr, linea);
	printf("%s\n", originalStr);
	while(col>0){
		__strtok_r(originalStr, DELIM_TOKEN, &aux);
		printf("%s\n", originalStr);
		printf("%s\n", aux);
		strcpy(originalStr, aux);	
		col--;
	}
	originalStr = strtok(originalStr, DELIM_TOKEN);
	printf("%s\n", originalStr);
	return originalStr;
}