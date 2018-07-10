#ifndef	_AEROPUERTOFRONT_H_
#define	_AEROPUERTOFRONT_H_

/*Retorna línea del archivo en la que está posicionado el puntero linea, sino retorna mensaje de error*/
char * leerLinea(char linea[CHAR_MAX], char * path);

/*Lee del archivo "aeropuertos_detalle.csv" y copia los datos correspondientes en las variables pasadas como parámetro*/
void datosAeropuerto(char OACI[OACI_MAX], char codigoLocal[LOCAL_MAX], char IATA[IATA_MAX], char * descripcion, char * trafico);

#endif