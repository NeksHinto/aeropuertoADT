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
gcc aeropuertoBack.c aeropuertoFront.c aeropuertoMAIN.c -o aeropuertoRegistros -Wall -pedantic -std=c99 -fsanitize=address -lm

--------
Ejecutar
--------
./aeropuertoRegistros {año entre 2014 y 2018 inclusive}
 
