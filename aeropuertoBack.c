#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aeropuertoBack.h"

#define	DELIM_FECHA 	"/"
#define	RECORRE_FECHA	strtok(NULL, DELIM_TOKEN)

#define EXIT_OK		1
#define	EXIT_ERR	0

enum DIAS {LUN=0, MAR, MIER, JUE, VIE, SAB, DOM};
enum ERRORES {ERR_AERO=1, ERR_MOV, ERR_FECHA, ERR_DIA, ERR_OACI};

/*Crea una estructura que dará soporte al almacenamiento de aeropuertos y sus respectivos movimientos.*/
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
				
				return new;
			}
		}
	}
	primero->sig=agregarAeropuertoRec(primero->sig, OACI, codigoLocal, IATA, descripcion, trafico, &exito);

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
void clasificarMovimiento(tVuelo primero, char aterrizaje){
	if(aterrizaje)
		primero.aterrizajes++;
	else
		primero.despegues++;

	return;
}

static
tVuelo agregarMovimientoRec(tVuelo primero, char OACISec[], char clasificacion, int * exito, char aterrizaje){
	int c;
	if(primero==NULL || (c=strcmp(OACISec, primero.OACI)) < 0 ){
		void * aux = malloc(sizeof(tVuelo));
		if(aux!=NULL){
			tVuelo new = aux;
			strcpy(new.OACI, OACISec);
			new.clasificacion=(clasificacion=="Internacional"?1:0); //Si es 0 es porque es un vuelo de cabotaje
			clasificarMovimiento(primero, aterrizaje);
		}
		*exito=1;

		return new;
	}
	if(c==0){
		clasificarMovimiento(primero, aterrizaje);
		*exito=1;

		return primero;
	}
	primero->sig=agregarMovimientoRec(primero, aterrizaje);

	return primero;
}

/*Agrega un movimiento al aeropuerto local pasado como parámetro.
**Pensado para utilizar post-agregar un aeropuerto.
**Incrementa totalAterrizajes o totalDespegues.
**El flag se define en el main, leyendo el archivo:
**Si se trata de un aterrizaje, aterrizaje = 1, sino aterrizaje = 0.*/
int agregarMovimiento(aeropuertoADT a, char OACILocal[], char OACISec[], char clasificacion, char flag){
	int exito=0;
	recorrerAeropuertos(a);
	while(strcmp(OACILocal, a->iter->OACI)!=0 && haySigAeropuerto(a)){
		a->iter = sigAeropuerto(a);
	}
	if(flag)
		a->iter->totalAterrizajes++;
	else
		a->iter->totalDespegues++;

	a->iter->aeropuerto.movimientos->vuelos=agregarMovimientoRec(a->iter->aeropuerto.movimientos->vuelos, OACISec, clasificacion, &exito, char aterrizaje);

	return exito;
}

void freeAeropuerto(aeropuertoADT a){
	freeAeropuertoRec(a->primero);
	for(int i=0; i<DIAS_MAX; i++){
		if(a->vuelosSemanal[i]!=NULL)
			free(a->vuelosSemanal[i]);
	}
	free(a->vuelosSemanal);
	free(a);

	return;
}

static
void freeAeropuertoRec(tLista primero){
	if(primero==NULL)
		return;
	freeVuelosRec(primero->sig);
	free(primero->trafico);
	free(primero);
	return;
}

static
void freeVuelosRec(tVuelo * vuelos){
	if(vuelos==NULL)
		return;
	freeVuelosRec(vuelos->sig);
	free(vuelos);
	return;
}


void recorrerAeropuertos(aeropuertoADT a){
	a->iter = a->primero;
	return;
}

void recorrerMovsAeropuerto(tLista aeropuerto){
	aeropuerto->movimientos->iter = aeropuerto->movimientos->vuelos;//cambiar nombre de vuelos que esta en la struct tmovimiento para primero(mas claro)  
	return;
}

int haySigAeropuerto(aeropuertoADT a){
	return a->iter!=NULL;
}

int haySigMovimiento(tLista aeropuerto){	
	return aeropuerto->movimientos->iter!=NULL;
}

tAeropuerto* sigAeropuerto(aeropuertoADT a){
	
	tAeropuerto* aux;
	aux= a->iter->aeropuerto;
	a->iter = a->iter->sig;
	
	return aux;
}

tVuelo * sigMovimiento(tLista aeropuerto){
	tVuelo * aux;
	aux = aeropuerto->movimientos->iter;
	aeropuerto->movimientos->iter = aeropuerto->movimientos->iter->sig;
	
	return aux;
}

/*Devuelve la posición en el vector que coincide con el día de la semana correspondiente
**a la fecha pasada como parámetro*/
int fechaADia(char * fecha, int * fueraDeRango, int anio){
	int d = atoi(strtok(fecha, DELIM_FECHA));
	int m = atoi(RECORRE_FECHA);
	int a = atoi(RECORRE_TOKENS);

	if(a!=anio)
		*fueraDeRango=1;

	int dia = (d+=m<3?a--:a-2,23*m/9+d+4+4/a-a/100+a/400)%7;

	return dia-1;
}//CHECKED

int registrarMovDia(aeropuertoADT a, int dia){
	switch(dia){
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

/*Si se trata de un aeropuerto desconocido, lo pisa con "####"*/
void esDesconocido(char OACI[5]){
	int i;
	for(i=2; OACI[i]!=0 && isalpha(OACI[i]); i++)
		;
	if(i!=4)
		strcpy(OACI, "####");
	printf("%d\n", i);
	return;
}//CHECKED

///////////////////////////////////////////////
/*QUERY 1*/
///////////////////////////////////////////////
///////////////////////////////////////////////
int movimientosAeropuerto(aeropuertoADT a, FILE *archivoP){
    tAeropuerto* aeroLocal; //este puntero es para que reciba la direccion que retorna la funcion next (del aeropuerto local siguiente de la lista)
    recorrerAeropuertos(a);//El iterador apunta al primer nodo de la lista

    do{
    	aeroLocal=sigAeropuerto(a);//retorna la direccion del aeropuerto que apunta el puntero iterador y hace con que el iterador apunte al proximo aeropuerto

    	/*Asumo que solo hay aeropuertos con movimientos en la lista, por lo tanto no verifico que tenga 
    	movimientos o no, los guardo directamente hasta completar el final de la lista de aeropuertos*/
    	fprintf(archivoP, "%s;%s;%s;%d\n", aeroLocal->OACI, aeroLocal->codigoLocal, aeroLocal->descripcion, (aeroLocal->movimientos->totalDespegues+aeroLocal->movimientos->totalAterrizajes));
    }while(haySigAeropuerto(a));
    fclose(archivoP);
    return EXIT_OK;	
}


///////////////////////////////////////////////
/*QUERY 2*/
///////////////////////////////////////////////
///////////////////////////////////////////////
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
     				fprintf(archivoP,"%s;%s;%d;%d;%d\n", aeroP->OACI, aeroP->IATA, desp=vueloP->aterrizaje, at=vueloP->despegues,desp+at );
     			}
     		}while(haySigMovimiento(a->iter));
		}
	}while(haySigAeropuerto(a));
	fclose(archivoP);//cerramos el stream
	return EXIT_OK;
}


///////////////////////////////////////////////
/*QUERY 3*/
///////////////////////////////////////////////
///////////////////////////////////////////////
int vuelosPorDía(aeropuertoADT * a, FILE * archivoP){
	fprintf(archivoP, "LUNES: %d\n", a->vuelosSemanal[LUN]);
	fprintf(archivoP, "MARTES: %d\n", a->vuelosSemanal[MAR]);
	fprintf(archivoP, "MIERCOLES: %d\n", a->vuelosSemanal[MIER]);
	fprintf(archivoP, "JUEVES: %d\n", a->vuelosSemanal[JUE]);
	fprintf(archivoP, "VIERNES: %d\n", a->vuelosSemanal[VIE]);
	fprintf(archivoP, "SABADO: %d\n", a->vuelosSemanal[SAB]);
	fprintf(archivoP, "DOMINGO: %d\n", a->vuelosSemanal[DOM]);
	fclose(archivoP);//cerramos el stream
	return EXIT_OK;
}


///////////////////////////////////////////////
/*QUERY 4*/
///////////////////////////////////////////////
///////////////////////////////////////////////
int detallesVuelo(aeropuertoADT a, FILE *archivoP){
	recorrerAeropuertos(a);
	recorrerMovsAeropuerto(a->iter);
	tAeropuerto *aeroP;//para recorrer lista de aeropuertos principales
	tVuelo * vueloP;//para recorrer lista de movimientos de cada aeropuerto principal


	do{
		aeroP=sigAeropuerto(a);
		/*todos los aeropuertos tienen movimiento, no hace falta verificar*/
		do{
			vueloP=sigMovimiento(a->iter);
			fprintf(archivoP, "%s;%s;%d;%d\n", aeroP->OACI, vueloP->OACI, vueloP->aterrizajes, vueloP->despegues);
		}while(haySigMovimiento(a->iter));
	}while(haySigAeropuerto(a->iter));
	fclose(archivoP);
	return EXIT_OK;
}

//FUNCIÓN PRINCIPAL QUE JUNTA TODAS LAS QUERIES
int crearArchivos(aeropuertoADT a, char * pathQuery1, char * pathQuery2, char * pathQuery3, char * pathQuery4){
	//Creamos los archivos
	FILE * archQ1;
	archQ1 = fopen("movs_aeropuerto.csv", "w"); //crea un archivo nombre "movs_aeropuerto(...)" y recibe un string "w" que permite que se escriba en el archivo creado
	FILE * archQ2;
	archQ2 = fopen("movs_internacional.csv", "w");
	FILE * archQ3;
	archQ3 = fopen("semanal.csv", "w");
	FILE * archQ4;
	archQ4 = fopen("aerop_detalle.csv", "w");
	//Copiamos los datos del TAD
	movimientosAeropuerto(aeropuertoADT a, archQ1);
	movimientosInternacionales(aeropuertoADT a, archQ2);
	vuelosPorDía(aeropuertoADT a, archQ3);
	detalleVuelos(aeropuertoADT a, archQ4);

	return EXIT_OK;
}

/*Reinicia el puntero al inicio de la línea y lo mueve hasta el token (columna) n*/
char * moverPtrColN(char * s, int n)
{
	int i=0;
	char * aux = strtok(s, DELIM_TOKEN);
	while(i<n){
		aux = RECORRE_TOKENS;
		i++;
	}
	return aux;
}//CHECKED

/*Valida si no es N/A*/
char * esLineaValida(char * linea){
	char * auxClasif = moverPtrColN(linea, CLASIFICACION);
	char * auxOrig = moverPtrColN(linea, OACI_ORIG);
	char * auxDest = moverPtrColN(linea, OACI_DEST);
	while(esNA(auxClasif) && esNA(auxOrig) && esNA(auxDest)){
		char c;
		SALTEA_LINEA(c, auxClasif);
		auxClasif = moverPtrColN(auxClasif, CLASIFICACION);
		auxOrig = moverPtrColN(auxClasif, OACI_ORIG);
		auxDest = moverPtrColN(auxClasif, OACI_DEST);
	}
	return auxClasif;
}

void validaArchivo(FILE * archivo)
{
	if(archivo==NULL)
	{
		fprintf(stderr, "No se pudo abrir el archivo.\n");
		return EXIT_ERR;
	}
	return;
}//CHECKED

static
char * strError(int tipoError)
{
	switch(tipoError)
	{
		case ERR_AERO:	return "No se pudo agregar el aeropuerto.\n"; break;
		case ERR_MOV: 	return "No se pudo agregar el movimiento.\n"; break;
		case ERR_FECHA: return "No se pudo registrar el día de la fecha pasada como parámetro.\n"; break;
		case ERR_DIA: 	return "No se pudo registrar el movimiento en el día correspondiente.\n"; break;
		case ERR_OACI:	return "No se encontró el OACI.\n"; break;
	}
}

void validaFlag(int flag, int tipoError)
{
	if(!flag){
		char * strError=strError(strError, tipoError);
		fprintf(stderr, "%s\n", strError);
		return EXIT_ERR;
	}
	return;
}
