#ifndef	_AEROPUERTOFRONT_H_
#define	_AEROPUERTOFRONT_H_

/*Retorna primer línea de donde apunta archivo salteandose la línea de encabezados*/
char * leerLinea(char linea[], FILE * archivo);

/*Lee del archivo "aeropuertos_detalle.csv" y copia los datos correspondientes en las variables pasadas como parámetro*/
void datosAeropuerto(char OACI[OACI_MAX], char codigoLocal[LOCAL_MAX], char IATA[IATA_MAX], char * descripcion, char * trafico);

#endif