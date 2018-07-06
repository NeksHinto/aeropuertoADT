------------------
Archivos incluidos
------------------
aeropuertoFront.c
aeropuertoFront.h
aeropuertoBack.c
aeropuertoBack.h
aeropuertoMAIN.c
README.txt (este archivo)


---------------------
Generar el ejecutable
---------------------



--------------------
Diseño y comentarios
--------------------
1. Estilo


2. Posibles problemas e inquietudes

-Query 2
Con el propósito de idear una estructura adaptable a futuro, consideramos importante el registro
de las fechas y horarios de cada vuelo en cada nodo de tipo tVuelo de los aterrizajes y despegues 
de cada aeropuerto local. A continuación el ejemplo:

typedef struct{
	unsigned char hora;
	unsigned char min;
}tHora;

typedef struct {
	unsigned char dia;
	unsigned char mes;
	unsigned short anio;
}tFecha;

typedef struct vuelo{
	char OACI[4];
	unsigned int cont;
	> tFecha * fechas; //Vector de estructuras de tipo tFecha
	> tHora * horarios; //Vector de estructuras de tipo tHora
	struct vuelo * sig; 
}tVuelo; 

Como notamos que el archivo eana1401-1802.csv estaba ordenado cronológicamente, supusimos que el registro
de estos datos conológicamente en un vector no sería problema, ya que no existirían casos donde se agregue al
principio o en el medio, sino que se agrega siempre al final. La posición de cada vector corresponde a la fecha y horario del mismo vuelo.

Ejemplo de función:

/*Agrega vuelo a un aeropuerto ya registrado.
**Si el código OACI del aeropuerto de salida/llegada ya existe,
**se incrementa el contador de movimientos entre los dos aeropuertos y se agrega
**la fecha y horario del nuevo movimiento cronológicamente.*/
int agregaMovimientos(aeropuertoADT aeropuerto, const char OACI[], tVuelo vuelo);

Ejemplo de requerimiento en caso real:

Se desean los registros (fecha y horario) de todos los despegues realizados en el aeropuerto de Ezeiza
ordenados cronológicamente.

-Query 4
Surgieron dificultades con el armado de la estructura para el Query 4 ya que
