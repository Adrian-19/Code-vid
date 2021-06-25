#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// ============ VARIABLES Y FUNCIONES GLOBALES ==============

int senal = 0; // Cambia de valor cuando ha pasado un segundo.
int finEjecucion = 0; // Si es 1 , significa que ya el programa debe finalizar por que ha llegado al tiempo limite
void* runTime(void* arg); // Este hilo toma el tiempo de ejecucion. 
void * cambioSegundo(void* arg); // Este hilo cambia la variable global senal cada segundo.

// ============ CONFIGURACION DE LA MATRIZ (MAPA) ==============

int*** mat;
int filas;
int columnas;
int tamArray = 4; // Es fijo porque son 4 tipos de agente.
// Posiciones de agentes: 0 = Rectos | 1 = Ciclicos | 2 = Aleatorios | 3 = Estaticos

void configuracionMapa();

// ===================== CANTIDADES =====================

int cantEnfermos = 0;
int cantSanos = 0;
int cantVacunados = 0;

// ===================== CODE-VID =====================
// variables o funciones globales...


// ===================== AGENTES =====================
// variables o funciones globales...
char* estado;
int* velocMax;
int* velocMin;
void configuracionAgente();

// ===================== VACUNAS =====================
// variables o funciones globales...


// ============ TIPO DE AGENTE: CICLICO (Adrian) ==============

// 
pthread_mutex_t mensaje;

void* ciclico(void* arg){
	int tipo = 1; // Ciclico
	int miGrupo = *(int*) arg;

	// pthread_mutex_lock(&mensaje);
	// printf("Soy un hilo del grupo %i y mis datos son los siguientes: \n", miGrupo);
	// printf("Velocidad maxima: %i\n", velocMax[miGrupo]);
	// printf("Velocidad minima: %i\n", velocMin[miGrupo]);
	// printf("Estado: %c\n", estado[miGrupo]);
	// pthread_mutex_unlock(&mensaje);

	//Generar una posicion aleatoria
	int x = 0;
	int y = 0;
	int bandera = 0;
	while(!bandera){
		x = (rand() % ((filas - 1) - 0 + 1));
		y = (rand() % ((columnas - 1) - 0 + 1));
		if(mat[x][y][0] != -1){ // SI no es una pared...
			bandera = 1;
			if(estado[miGrupo] == 'e'){
				mat[x][y][tipo]++;
			}
		}
	}

	// //Definiendo ruta...
	// int pasos = 0;
	// while(pasos <= 7){

	// 	pasos++;
	// }
}


void * cambioSegundo(void* arg){
	
	while(!finEjecucion){
		sleep(1);
		if(!senal){ senal = 1; }
		else{senal = 0;}	
	}
}

// Este hilo toma el tiempo de ejecucion.
void* runTime(void* arg){
	int timeLimit = *(int*) arg;
	sleep(timeLimit);
	finEjecucion = 1;
}

// =========================================================================================================================
int main (int argc, char **argv)
{
	int *tiempo =  malloc(sizeof(*tiempo));
	*tiempo = atoi(argv[1]);

	pthread_mutex_init(&mensaje, NULL);
	
	configuracionMapa();
	configuracionAgente();

	srand(time(0));


	// Se crea un hilo que vaya tomando el tiempo de ejecucion.
	pthread_t timeID;
	pthread_create(&timeID, NULL, runTime, tiempo);

	// Hilo que modifica la variable global "senal" cada segundo.
	pthread_t segundosID;
	pthread_create(&segundosID, NULL, cambioSegundo, NULL);

	pthread_join(timeID, NULL);
	pthread_join(segundosID, NULL);

	pthread_mutex_destroy(&mensaje);

	// Importante mantener esto al final del main porque
	// borra la memoria de la matriz dinamica.
	// Se libera la memoria de la matriz
	for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++) {
            free(mat[i][j]);
        }
        free(mat[i]);
    }

    free(mat);
    free(estado);
    free(velocMin);
    free(velocMax);

	return 0;	
}
// =========================================================================================================================

// Funcion que lee el archivo de configuracion del mapa ("configMapa.txt")
// y crea la matriz en base a los valores leidos.  
void configuracionMapa(){

	char singleLine[150];
	FILE * fPointer = fopen("configMapa.txt", "r");

	fgets(singleLine, 150, fPointer);

	// Se lee la cantidad de filas y columnas
	sscanf(singleLine, "%i %i\n", &columnas, &filas);

	// Creacion de la matriz dinamica
	mat = (int***)malloc(filas * sizeof(int**));
	if(mat == NULL){
		printf("Out of memory!\n");
		exit(0);
	}

	for(int i = 0; i < filas; i++){
		mat[i] = (int**) malloc(columnas * sizeof(int*));
		if(mat[i] == NULL){
			printf("Out of memory!\n");
			exit(0);
		}
		for(int j = 0; j<columnas; j++){
			mat[i][j] = (int*)malloc(tamArray*sizeof(int));
			if(mat[i][j] == NULL){
				printf("Out of memory!\n");
				exit(0);
			}
		}
	}

	// FALTA POR IMPLEMENTAR ...
	// Se podria poner la regla de que si uno de los campos del 
	// array es -1, significa que es una pared

	// Se lee la cantidad de paredes
	// fgets(singleLine, 150, fPointer);
	// int cantPared = 0;
	// int x1, y1, x2, y2;
	// scanf(singleLine, "%i", &cantPared);	
	// for(int i = 0; i < cantPared; i++){
	// 	scanf(singleLine, "%i %i %i %i", &x1, &y1, &x2, &y2);
		
	// }

	fclose(fPointer);
}

void configuracionAgente(){
	char singleLine[150];
	FILE * fPointer = fopen("configAgente.txt", "r");

	fgets(singleLine, 150, fPointer);

	int cantidadGrupos = 0;
	//Se lee la cantidad de grupos
	sscanf(singleLine, "%i", &cantidadGrupos);

	estado = (char*) malloc(cantidadGrupos * (sizeof(char)));
	velocMin = (int*) malloc(cantidadGrupos * (sizeof(int)));
	velocMax = (int*) malloc(cantidadGrupos * (sizeof(int)));
	if(estado == NULL || velocMin == NULL || velocMax == NULL){
		printf("Out of memory!\n");
		exit(0);
	}

	int cantAgentes = 0;
	int tipoAgente = 0;
	int vMin = 0;
	int vMax = 0; 
	char estadoConf = 'l';
	for(int i = 0; i < cantidadGrupos; i++){
		fgets(singleLine, 150, fPointer);
		sscanf(singleLine, "%i", &cantAgentes);
		fgets(singleLine, 150, fPointer);
		sscanf(singleLine, "%i", &tipoAgente);
		fgets(singleLine, 150, fPointer); 
		sscanf(singleLine, "%i %i", &vMin, &vMax);
		fgets(singleLine, 150, fPointer);
		sscanf(singleLine, "%c", &estadoConf);
		estado[i] = estadoConf;
		velocMin[i] = vMin;
		velocMax[i] = vMax;

		int * arg = malloc(sizeof(*arg));
		*arg = i;
		switch(tipoAgente){
			case 1: // Rectos
			break; 

			case 2: // Ciclicos
				// generar un hilo que genere otros hilos.
				for(int j = 0; j < cantAgentes; j++){
					pthread_t myId;
					pthread_create(&myId, NULL, ciclico, arg);
				}
			break;

			case 3: // Aleatorios 
			break;

			case 4: // Estaticos
			break;
		}
	}
	fclose(fPointer);
}
	// ejemplo para imprimir la matriz entera

	// printf("tamArray es: %i\n", tamArray);
	// for(int a = 0; a<filas; a++){
	// 	for(int b = 0; b<columnas; b++){
	// 		printf("%i, %i es: \n", a, b);
	// 		for(int c = 0; c<tamArray; c++){
	// 			printf("%i (%i)\n", mat[a][b][c], c);
	// 		}
	// 	}
	// }
