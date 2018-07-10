#ifndef	_AEROPUERTOBACK_H_
#define	_AEROPUERTOBACK_H_

typedef struct aeropuertoCDT * aeropuertoADT;

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

aeropuertoADT nuevoRegistroAero()

/*Agrega un nuevo aeropuerto a la lista. 
**Si ya existía el aeropuerto, no hace nada.
**Si lo agrega correctamente retorna 1, caso contrario retorna 0.*/
int agregarAeropuerto(aeropuertoADT a, char OACI[], char codigoLocal[], char IATA[], char * descripcion, char trafico);

/*Agrega un movimiento al aeropuerto local pasado como parámetro.
**Pensado para utilizar post-agregar un aeropuerto.
**Incrementa totalAterrizajes o totalDespegues.
**El flag se define en el main, leyendo el archivo:
**Si se trata de un aterrizaje, aterrizaje = 1, sino aterrizaje = 0.*/
int agregarMovimiento(aeropuertoADT a, char OACILocal[], char OACISec[], char clasificacion, char flag);

/*Libera todos los recursos reservados por el TAD*/
void freeAeropuerto(aeropuertoADT a);

/*
**Funciones de iteración para poder recorrer todos los aeropuertos locales del registro
**y los movimientos de cada uno de estos (ambos en orden alfabético).
**recorrerAeropuertos() inicializa el iterador en el primer aeropuerto por orden alfabético.
**recorrerMovsAeropuerto() inicializa el iterador en el primer movimiento del aeropuerto en cuestión.
**sigAeropuerto() retorna el aeropuerto apuntado por el iterador y lo hace apuntar al siguiente aeropuerto.
**	Si no hay más aeropuertos en el registro, retorna una estructura con todos sus campos que 
**	no son punteros en cero y los punteros en NULL.
**sigMovimiento() retorna el movimiento apuntado por el iterador y lo hace apuntar al siguiente movimiento 
**	del aeropuerto en cuestión.
**	Si no hay más movimientos en el aeropuerto, retorna una estructura con todos sus campos que no son punteros
**	en cero y los punteros en NULL.
**haySigAeropuerto() retorna distinto de cero si el iterador apunta a un aeropuerto válido, cero sino.
**haySigMovimiento() retorna distinto de cero si el iterador apunta a un movimiento válido, cero sino.
*/
void recorrerAeropuertos(aeropuertoADT a);
void recorrerMovsAeropuerto(tLista aeropuerto);
int haySigAeropuerto(aeropuertoADT a);
int haySigMovimiento(tLista aeropuerto);	
tLista sigAeropuerto(aeropuertoADT a);
tVuelo sigMovimiento(tLista aeropuerto);

/*Devuelve la posición en el vector que coincide con el día de la semana correspondiente
**a la fecha pasada como parámetro*/
int fechaADia(char * fecha, int * fueraDeRango, int anio);

int registrarMovDia(aeropuertoADT a, int dia);

/*Si se trata de un aeropuerto desconocido, lo pisa con "####"*/
void esDesconocido(char OACI[5]);

/*Reinicia el puntero al inicio de la línea y lo mueve hasta el token (columna) n*/
char * moverPtrColN(char * s, int n);

/*Valida si el token no es N/A*/
char * esLineaValida(char * linea);

/*
**Funciones de errores
**validaArchivo() retorna mensaje de error si el puntero al stream es NULL, sino no hace nada.
**validaFlag() retorna mensaje de error correspondiente al tipo de error pasado como parámetro, sino no hace nada.
*/
void validaArchivo(FILE * archivo);
void validaFlag(int flag, int tipoError);


#endif