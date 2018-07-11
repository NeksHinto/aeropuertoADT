aeropuertoADT registro = nuevoRegistroAero();
	FILE * archivoVuelos;
	archivoVuelos = fopen("./eana1401-1802.csv", "r");
	char * lineaVuelos = malloc(CHAR_MAX);
	char * lineaVuelosAux = malloc(CHAR_MAX);
	//Guardo la primer linea del archivo vuelos
	strcpy(lineaVuelos, leerLinea(lineaVuelos, archivoVuelos));
	strcpy(lineaVuelosAux, lineaVuelos);
	do{
		if(validaLinea(lineaVuelosAux)){
			;
		}
		else{
			int fueraDeAnio=0;
			char * fecha = malloc(DIAS_MAX);
			strcpy(fecha, retornaToken(lineaVuelos, FECHA));
			int dia = fechaADia(fecha, &fueraDeAnio, argv[1]);
			validaFlag(dia, ERR_DIA);
			if(!fueraDeAnio){
				int flag = registraMovDia(registro, dia);
				validaFlag(flag, ERR_DIA);
				char clasificacion = (strcmp(retornaToken(lineaVuelos, CLASIFICACION), "Cabotaje")==0?0:1);
				int aterrizaje = (strcmp(retornaToken(lineaVuelos, TIPO_MOV), "Despegue")==0?0:1);
				//Tres pos: 1 local y el otro no, viceversa, o ambos locales (vuelos de cabotaje)
				if(!aterrizaje){
					strcpy(OACILocal, retornaToken(lineaVuelos, OACI_ORIG));
					strcpy(OACIAnon, retornaToken(lineaVuelos, OACI_DEST));
				else{
					strcpy(OACILocal, retornaToken(lineaVuelos, OACI_DEST));
					strcpy(OACIAnon, retornaToken(lineaVuelos, OACI_ORIG));
				}
				esDesconocido(OACIAnon);
				char * codigoLocal1 = malloc(LOCAL_MAX);
				char * IATA1 = malloc(IATA_MAX);
				char * descripcion1 = malloc(CHAR_MAX);
				char trafico1;
				datosAeropuerto(OACILocal, codigoLocal1, IATA1, descripcion1, &trafico1);
				int flag = agregarAeropuerto(OACILocal, codigoLocal1, descripcion1, trafico1);
				validaFlag(flag, ERR_AERO);
				flag = agregarMovimiento(regsitro, OACILocal, OACIAnon, clasificacion, aterrizaje);
				validaFlag(flag, ERR_MOV);
				if(clasificacion){
					char * codigoLocal2 = malloc(LOCAL_MAX);
					char * IATA2 = malloc(IATA_MAX);
					char * descripcion2 = malloc(CHAR_MAX);
					char trafico2;
					datosAeropuerto(OACIAnon, codigoLocal2, IATA2, descripcion2, &trafico2);
					int flag = agregarAeropuerto(OACIAnon, codigoLocal2, IATA2, descripcion2, trafico2);
					validaFlag(flag, ERR_AERO);
					flag = agregarMovimiento(regsitro, OACIAnon, OACILocal, clasificacion, aterrizaje);
					validaFlag(flag, ERR_MOV);
				}
			}
			lineaVuelos=fgets(lineaVuelos, CHAR_MAX, archivoVuelos);
		}while(!feof(archivoVuelos));
	}
	return 0;
