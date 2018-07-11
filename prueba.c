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

char * leerLinea(char linea[CHAR_MAX], char * path){
	FILE * archivo;
	archivo = fopen(path, "r");
	//Salteamos la primer linea (Encabezados de las columnas)
	char c;
	SALTEA_LINEA(c, archivo); //Devuelve el siguiente char y avanza la posición del puntero al archivo
	//Copiamos la línea	
	linea = fgets(linea, CHAR_MAX, archivo);
	fclose(archivo);

	return linea;
}

void modificaStr(char * s){
	int i;
	for(i = 0; s[i]!=0; i++)
		s[i]=toupper(s[i]);
	s[i]=0;
	printf("%s\n", s);
}

void esDesconocido(char OACI[5]){
	int i;
	for(i=2; OACI[i]!=0 && isalpha(OACI[i]); i++)
		;
	if(i!=4)
		strcpy(OACI, "####");
	printf("%d\n", i);
	return;
}//CHECKED

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
}//CHECKEDstatic 
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
		tVuelo * aux = malloc(sizeof(tVuelo));
		if(aux!=NULL){
			tVuelo * new = aux;
			strcpy(new->OACI, OACISec);
			new->clasificacion = clasificacion; //Si es 0 es porque es un vuelo de cabotaje
			clasificarMovimiento(primero, aterrizaje);

			return new;
		}
		*exito=1;

		return aux;
	}
	if(c==0){
		clasificarMovimiento(primero, aterrizaje);
		*exito=1;

		return primero;
	}
	primero->sig=agregarMovimientoRec(primero, OACISec, clasificacion, exito, aterrizaje);

	return primero;
}

int agregarMovimiento(aeropuertoADT a, char OACILocal[], char OACISec[], char clasificacion, char flag){
	int exito=0;
	recorrerAeropuertos(a);
	while(strcmp(OACILocal, a->iter.OACI)!=0 && haySigAeropuerto(a)){
		a->iter = sigAeropuerto(a);
	}
	if(flag)
		a->iter->totalAterrizajes++;
	else
		a->iter->totalDespegues++;

	a->iter->aeropuerto.movimientos->vuelos=agregarMovimientoRec(a->iter->aeropuerto.movimientos->vuelos, OACISec, clasificacion, &exito, aterrizaje);

	return exito;
}

int main(/*int argc, char *argv[]*/)
{
	/*if(argc==2 && (strcmp(argv[1], "2014")==0 || strcmp(argv[1], "2015")==0 || strcmp(argv[1], "2016")==0 || strcmp(argv[1], "2017")==0 || strcmp(argv[1], "2018")==0))
	{
		printf("GUT\n");
	}
	else
	{
		fprintf(stderr, "Vuelva a ejecutar el programa ingresando un año entre el 2014 y 2018 inclusive.\n");
		exit(EXIT_FAILURE);	
	}*/

	/*FILE * miArchivo = fopen("./aeropuertos_detalle.csv", "r");
	if(miArchivo==NULL)
		printf("No se pudo abrir\n");
	else
	{
		char c = fgetc(miArchivo);
		while(!isalpha(c))
			c = fgetc(miArchivo);
		long int pos=ftell(miArchivo);
		printf("%c %ld\n", c, pos);
	}
	fclose(miArchivo);*/

	/*char linea[CHAR_MAX] ={0};
	char * a = leerLinea(linea, "./eana1401-1802.csv");*/
	/*a = moverPtrColN(a, 3);
	printf("%s\n", a);*/
	/*char * b = leerLinea(linea, "./aeropuertos_detalle.csv");
	printf("%s\n", b);*/
	/*char str[] = "Buenas;comp;anadan;294;chicos";
	char * save;
	char *a, *b;
	a = __strtok_r(str, DELIM_TOKEN, &save);
	b = __strtok_r(NULL, DELIM_TOKEN, &save);
	printf("%s\n", save);
	printf("%s\n", a);
	printf("%s\n", b);*/

	/*char s[5] = "AR-1234";
	esDesconocido(s);
	printf("%s\n", s);*/

	aeropuertoADT a = nuevoRegistroAero();
	char OACI[OACI_MAX] = "SAEZ";
	char codigoLocal[LOCAL_MAX] = "ABC";
	char IATA[IATA_MAX]="DEF";
	char * descripcion = "EZEIZA";
	char trafico = 0;
	int flag = agregarAeropuerto(a, OACI, codigoLocal, IATA, descripcion, trafico);
	printf("%s\n", a->primero->aeropuerto.OACI);
	printf("%s\n", a->primero->aeropuerto.codigoLocal);
	printf("%s\n", a->primero->aeropuerto.IATA);
	printf("%s\n", a->primero->aeropuerto.descripcion);
	printf("%d\n", a->primero->aeropuerto.trafico);
	printf("%d\n", flag);

	char OACISec[OACI_MAX]="BESP";
	char clasificacion = 1;
	char aterrizaje = 0;
	flag = agregarMovimiento(regsitro, OACI, OACISec, clasificacion, aterrizaje);
	printf("%s\n", a->primero->aeropuerto.OACI);
	printf("%s\n", a->primero->aeropuerto.codigoLocal);
	printf("%s\n", a->primero->aeropuerto.IATA);
	printf("%s\n", a->primero->aeropuerto.descripcion);
	printf("%d\n", a->primero->aeropuerto.trafico);
	printf("%d\n", flag);

	return 0;
}

