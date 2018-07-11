#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

#define	SALTO_LINEA			'\n'
#define	SALTEA_LINEA(c,a)	while((c=fgetc(a))!=SALTO_LINEA)
#define	CHAR_MAX			100
#define	DELIM_FECHA 		"/"
#define	DELIM_TOKEN			";"
#define	RECORRE_FECHA		(strtok(NULL, DELIM_FECHA))
#define	RECORRE_TOKENS		(strtok(NULL, DELIM_TOKEN))

#define	OACI_MAX	5
#define	LOCAL_MAX	4
#define	IATA_MAX	4
#define	DIAS_MAX	8

#define EXIT_OK		1
#define	EXIT_ERR	0
#define ERR_ARCH    3


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
	if(c>0){
		primero->sig=agregarAeropuertoRec(primero->sig, OACI, codigoLocal, IATA, descripcion, trafico, exito);
	}
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


void recorrerAeropuertos(aeropuertoADT a){
	a->iter = a->primero;
	return;
}//CHECK


void recorrerMovsAeropuerto(tLista a){
	a->aeropuerto.movimientos.iter = a->aeropuerto.movimientos.vuelos;//cambiar nombre de vuelos que esta en la struct tmovimiento para primero(mas claro)  
	return;
}

int haySigAeropuerto(aeropuertoADT a){
	return a->iter!=NULL;
}//CHECK



int haySigMovimiento(tLista a){	
	return a->aeropuerto.movimientos.iter!=NULL;
}



tLista sigAeropuerto(aeropuertoADT a){
	
	tLista aux;
	aux= a->iter;
	a->iter = a->iter->sig;
	
	return aux;
}//CHECK


tVuelo * sigMovimiento(tLista a){
	tVuelo * aux;
	aux = a->aeropuerto.movimientos.iter;
	a->aeropuerto.movimientos.iter = a->aeropuerto.movimientos.iter->sig;
	
	return aux;
}



////////////////////////////////////////////////
//////////////////////////////////////////////////
////QUERY 1
///////////////////////////////////////////////

int movimientosAeropuerto(aeropuertoADT a, FILE *archivoP){

    int cantDeElementos;//guardo la cantidad de elementos que se escriben el archivo(si es negativo es porque no quedo mas memoria)
    tLista aeroLocal; //este puntero es para que reciba la direccion que retorna la funcion next (del aeropuerto local siguiente de la lista)
    recorrerAeropuertos(a);//El iterador apunta al primer nodo de la lista
    
    do{
    	aeroLocal=sigAeropuerto(a);//retorna la direccion del aeropuerto que apunta el puntero iterador y hace con que el iterador apunte al proximo aeropuerto

    	/*Asumo que solo hay aeropuertos con movimientos en la lista, por lo tanto no verifico que tenga 
    	movimientos o no, los guardo directamente hasta completar el final de la lista de aeropuertos*/
    	cantDeElementos=fprintf(archivoP, "%s;%s;%s;%d\n", aeroLocal->aeropuerto.OACI, aeroLocal->aeropuerto.codigoLocal, aeroLocal->aeropuerto.descripcion, (aeroLocal->aeropuerto.movimientos.totalDespegues+aeroLocal->aeropuerto.movimientos.totalAterrizajes));
    	
    	//si la cant de elementos es menor que cero, no hay mas espacio para escribir en el archivo, devolvemos error
    	if(cantDeElementos<0)
    		return EXIT_ERR;

    }while(haySigAeropuerto(a));
   
    fclose(archivoP);

    return EXIT_OK;	
}//CHECK




static
void clasificarMovimiento(tVuelo * primero, char aterrizaje){
	if(aterrizaje)
		primero->aterrizajes++;
	else
		primero->despegues++;

	return;
}

static
int agregarMovimiento2(tLista aero, char OACIDestino[], char clasificacion, char aterrizaje){
	int c;
	int exito=0;
	recorrerMovsAeropuerto(aero);
	tVuelo*next;
	tVuelo*anterior=aero->aeropuerto.movimientos.vuelos;

	do{
		next=sigMovimiento(aero);
		if(next!=anterior)
			anterior=anterior->sig;

		if((next==NULL)||(c=strcmp(OACIDestino, next->OACI))<0){
			void *aux=malloc(sizeof(tVuelo));
			if(aux!=NULL){
				tVuelo*new=aux;
				strcpy(new->OACI, OACIDestino);
				new->clasificacion=clasificacion;
				clasificarMovimiento(new, aterrizaje);
				new->sig=next;
				anterior->sig=new;
				exito=1;
			}
		}else if(c==0){
			clasificarMovimiento(next, aterrizaje);
			exito=1;
		}
	}while(haySigMovimiento(aero));

	return exito;
}

/*Agrega un movimiento al aeropuerto local pasado como parámetro.
**Pensado para utilizar post-agregar un aeropuerto.
**Incrementa totalAterrizajes o totalDespegues.
**El flag se define en el main, leyendo el archivo:
**Si se trata de un aterrizaje, aterrizaje = 1, sino aterrizaje = 0.*/

int agregarMovimiento(aeropuertoADT a, char OACILocal[], char OACISec[], char clasificacion, char aterrizaje){
	int exito=0;
	recorrerAeropuertos(a);
	tLista aux;
	while(strcmp(OACILocal, a->iter->aeropuerto.OACI)!=0 && haySigAeropuerto(a)){
		aux = sigAeropuerto(a);
	}
	if(aterrizaje)
		a->iter->aeropuerto.movimientos.totalAterrizajes++;
	else
		a->iter->aeropuerto.movimientos.totalDespegues++;

	exito=agregarMovimiento2(aux, OACISec, clasificacion, aterrizaje);

	return exito;
}
static
char * strError(int tipoError)
{
	switch(tipoError)
	{
		/*
		case ERR_AERO:	return "No se pudo agregar el aeropuerto.\n"; break;
		case ERR_MOV: 	return "No se pudo agregar el movimiento.\n"; break;
		case ERR_FECHA: return "No se pudo registrar el día de la fecha pasada como parámetro.\n"; break;
		case ERR_DIA: 	return "No se pudo registrar el movimiento en el día correspondiente.\n"; break;
		case ERR_OACI:	return "No se encontró el OACI.\n"; break;
		*/
		case ERR_ARCH: return "No hay espacio, no se copio\n";break;
	}
}
/*
int crearArchivos(aeropuertoADT a, char * pathQuery1){
	//Creamos los archivos

	int error1;// error2;// error3, error4;
	FILE * archQ1;
	archQ1 = fopen(pathQuery1, "w"); //crea un archivo nombre "movs_aeropuerto(...)" y recibe un string "w" que permite que se escriba en el archivo creado
	//FILE * archQ2;
	//archQ2 = fopen(pathQuery2, "w");
	
	/*
	FILE * archQ3;
	archQ3 = fopen("semanal.csv", "w");
	FILE * archQ4;
	archQ4 = fopen("aerop_detalle.csv", "w");
/*	
	//*///Copiamos los datos del TAD

	//error1= movimientosAeropuerto( a, archQ1);
	//error2= movimientosInternacionales(aeropuertoADT a, archQ2);
	//vuelosPorDía(aeropuertoADT a, archQ3);
	//detalleVuelos(aeropuertoADT a, archQ4);
	//if((error1==1)/*||(error2==1)/*||(error3==1)||(error4==1)*/){
//		printf("%s\n", strError(ERR_ARCH));
//		return EXIT_ERR;
//	}


//	return EXIT_OK;
//}//CHECK



int main()
{
	aeropuertoADT a = nuevoRegistroAero();
	char OACI[OACI_MAX] = "SAEZ";
	char codigoLocal[LOCAL_MAX] = "ABC";
	char IATA[IATA_MAX]="DEF";
	char * descripcion = "EZEIZA";
	char trafico = 0;
	int flag = agregarAeropuerto(a, OACI, codigoLocal, IATA, descripcion, trafico);
	/*printf("%s\n", a->primero->aeropuerto.OACI);
	printf("%s\n", a->primero->aeropuerto.codigoLocal);
	printf("%s\n", a->primero->aeropuerto.IATA);
	printf("%s\n", a->primero->aeropuerto.descripcion);
	printf("%d\n", a->primero->aeropuerto.trafico);
	printf("El flag de aeropuertos= %d\n", flag);
	*/
	
	
	char OACISec[OACI_MAX]="BESP";
	char clasificacion = 1;
	char aterrizaje = 0;
	
	flag = agregarMovimiento(a, OACI, OACISec, clasificacion, aterrizaje);
	//printf("%s\n", a->primero->aeropuerto.OACI);
	//printf("%s\n", a->primero->aeropuerto.codigoLocal);
	//printf("%s\n", a->primero->aeropuerto.IATA);
	//printf("%s\n", a->primero->aeropuerto.descripcion);
	//printf("%d\n", a->primero->aeropuerto.trafico);
	printf("El flag de movimientos= %d\n", flag);
    

    //crearArchivos(a, "movs_aeropuerto.csv"/*,  "movs_internacional.csv"*/);

	return 0;
}
