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
gcc aeropuertoBack.c aeropuertoFront.c aeropuertoMAIN.c -o aeropuertoRegistros -Wall -pedantic -std=c99 -fsanitize=address

--------
Ejecutar
--------
./aeropuertoRegistros {año entre 2014 y 2018 inclusive}


--------------------
Diseño y comentarios
--------------------
1. Diseño
Lista de listas (Falta completar)

2. Estilo
Front vs Back (Falta completar)
Funcion moverStreamPtr: mejor estilo que arranque desde el principio o validar que este mas adelante que n y bai?

3. Posibles problemas e inquietudes

-Lectura de archivos
Si bien el enunciado decía que debíamos suponer que los datos pasados eran correctos por ser de una página oficial, por el archivo aeropuertos_detalle.csv tuvimos que hacer la excepción validando que la función fgetc empiece a leer cuando llegara a un caracter alfanumérico, ya que en las posiciones 1, 2 y 3 del archivo, devolvía cualquier cosa menos el caracter 'l'.
Por motivos de eficiencia (recorrer cadaa archivo solo una vez) decidimos primero leer aeropuertos.csv rellenar la lista y desp el otro archivo hacer el vector, y los vuelos (EMPROLIJAR)

Ejemplo de prueba:

int main()
{
	FILE * miArchivo = fopen("./aeropuertos_detalle.csv", "rt");
	if(miArchivo==NULL)
		printf("No se pudo abrir\n");
	else
	{
		char c = fgetc(miArchivo);
		long int pos=ftell(miArchivo);
		printf("%d %ld\n", c, pos); //Posición 1
		c = fgetc(miArchivo);
		pos=ftell(miArchivo);
		printf("%d %ld\n", c, pos); //Posición 2
		c = fgetc(miArchivo);
		pos=ftell(miArchivo);
		printf("%d %ld\n", c, pos); //Posición 3
		c = fgetc(miArchivo);
		pos=ftell(miArchivo);
		printf("%d %ld\n", c, pos); //Posición 4
	}
	fclose(miArchivo);

	return 0;
}

Output:
-17 1
-69 2
-65 3
108 4 //ASCII de la 'l' en la posición 4

-Query 2
Con el propósito de idear una estructura adaptable a futuro, consideramos importante el registro de las fechas y horarios de cada vuelo en cada nodo de tipo tVuelo de los aterrizajes y despegues de cada aeropuerto local. 

A continuación el ejemplo:

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

Como notamos que el archivo eana1401-1802.csv estaba ordenado cronológicamente, supusimos que el registro de estos datos conológicamente en un vector no sería problema, ya que no existirían casos donde se agregue al principio o en el medio, sino que se agrega siempre al final. La posición de cada vector corresponde a la fecha y horario del mismo vuelo.

Ejemplo de función:

/*Agrega vuelo a un aeropuerto ya registrado.
**Si el código OACI del aeropuerto de salida/llegada ya existe,
**se incrementa el contador de movimientos entre los dos aeropuertos y se agrega
**la fecha y horario del nuevo movimiento cronológicamente.*/
int agregaMovimientos(aeropuertoADT aeropuerto, const char OACI[], tVuelo vuelo);

Ejemplo de requerimiento en caso real:

Se desean los registros (fecha y horario) de todos los despegues realizados en el aeropuerto de Ezeiza ordenados cronológicamente.
