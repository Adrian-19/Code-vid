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

// Del archivo de config
float probMuerte = 0; // Probabilidad que muera.
int tiempoMuerte = 0; // Tiempo que tarda en morir
int tiempoCurar = 0; // Tiempo que se tarda en sanar
int reinfeccion = 0; // 0 = No hay reinfeccion | 1 = Si hay reinfeccion.


// ===================== AGENTES =====================
// variables o funciones globales...
char* estado;
int* velocMax;
int* velocMin;
void configuracionAgente();

// ===================== VACUNAS =====================
// variables o funciones globales...


// ============ TIPO DE AGENTE: CICLICO ==============

// 
pthread_mutex_t mensaje;
pthread_mutex_t modificar;
void calcForward(int* direccion, int* x, int* y);
int calcBackward(int direccion);
void* ciclico(void* arg){
	int tipo = 1; // Ciclico
	int miGrupo = *(int*) arg;
	char miEstado = estado[miGrupo];
	//Generar una posicion aleatoria
	int x = 0;
	int y = 0;
	int antX = 0;
	int antY = 0;
	int bandera = 0;
	while(!bandera){
		x = (rand() % ((filas - 1) - 0 + 1));
		y = (rand() % ((columnas - 1) - 0 + 1));
		if(mat[x][y][0] != -1){ // SI no es una pared...
			bandera = 1;
			if(miEstado == 'e'){
				antX = x;
				antY = y;
				pthread_mutex_lock(&modificar);
				mat[x][y][tipo]++;
				pthread_mutex_unlock(&modificar);
			}
		}
	}

	//Definiendo ruta...
	int pasos = 0;
	int forward[7];
	int backwards[7];
	int direccion;
	bandera = 0;
	while(pasos < 7){
		antX = x;
		antY = y;
		pthread_mutex_lock(&mensaje); // BORRAR MUTEX !!!
		direccion = (rand() % ((8 - 1) - 1 + 1)) + 1;
		calcForward(&direccion, &x, &y);
		if(x<0 || y<0 || x>=filas || y>=columnas){
			printf("hay una pared (%i,%i)\n", x, y);
			x = antX;
			y = antY;
			bandera = 1; // Hay una pared en medio de su camino.
			break;
		}
		printf("x: %i, y: %i\n",x, y); // BORRAR MUTEX !!!
		pthread_mutex_unlock(&mensaje);
		if(mat[x][y][0] == -1){
			x = antX;
			y = antY;
			bandera = 1; // Hay una pared en medio de su camino.
			break;
		}
		if(miEstado == 'e'){
			pthread_mutex_lock(&modificar);
			mat[antX][antY][tipo]--;
			mat[x][y][tipo]++;
			pthread_mutex_unlock(&modificar);
			forward[pasos] = direccion;
		}
		if(miEstado == 's'){
		}

		pasos++;
	}
	if(bandera != 1){ pasos--; }

	int pasosFinal = pasos;

	// Determinando la ruta hacia atras... 
	int backwardCount = 0;
	while(pasos >= 0){
		antX = x;
		antY = y;
		direccion = calcBackward(forward[pasos]);
		calcForward(&direccion, &x, &y);
		//printf("x: %i, y: %i\n",x, y); 
		if(miEstado == 'e'){
			pthread_mutex_lock(&modificar);
			mat[antX][antY][tipo]--;
			mat[x][y][tipo]++;
			pthread_mutex_unlock(&modificar);
			backwards[backwardCount] = direccion;
		}
		if(miEstado == 's'){
		}
		backwardCount++;
		pasos--;
	}
	backwardCount--;
	int backwardFinal = backwardCount;

	int steps = 0;
	int secondCount = 0;
	int morire = 0; // :(
	pasos = 0;
	backwardCount = 0;
	// while(!finEjecucion){
	// 	// REVISAR CENTRO DE VACUNACION

	// 	if(morire == 1){

	// 	}
	// 	// Si se sale de este ciclo, significa que ya ha pasado un segundo.
	// 	while(steps<=velocMax[miGrupo] || steps>=velocMin[miGrupo]){
	// 		pasos = 0;
	// 		backwardCount = 0;
	// 		// Walk forward
	// 		while(pasos<=pasosFinal){
	// 			antX = x;
	// 			antY = y;
	// 			direccion = forward[pasos];
	// 			calcForward(&direccion, &x, &y);
	// 			if(miEstado == 'e'){
	// 				pthread_mutex_lock(&modificar);
	// 				mat[antX][antY][tipo]--;
	// 				mat[x][y][tipo]++;
	// 				pthread_mutex_unlock(&modificar);
	// 				backwards[backwardCount] = direccion;
	// 			}
	// 			if(miEstado == 's'){
	// 				for(int i = 0; i<tamArray; i++){
	// 					if(mat[x][y][i] > 0){ // hay personas enfermas...
	// 						// calculando si me contagio con este tipo de agente de acuerdo al archivo de config...
	// 						// if(si me enfermo, detengo este loop porque es solamente para validar si me enfermo o no) {
	// 						// 	miEstado = 'e';
	// 						// 	morire = 1; // :(
	// 						// 	break;
	// 						// }
	// 						// si no, sigo buscando en las demas casillas...
	// 					}
	// 				}
	// 			}
	// 			pasos++;
	// 			steps++;
	// 		}
	// 	}
	// 	steps = 0;
	// }
}

void calcForward(int* direccion, int* x, int* y){
	switch(*direccion){
		case 1: *x = *x - 1;
		break;
		case 2: *x = *x - 1; *y = *y + 1;
		break;
		case 3: *y = *y + 1;
		break;
		case 4: *x = *x + 1; *y = *y + 1;
		break;
		case 5: *x = *x + 1;
		break;
		case 6: *x = *x + 1; *y = *y - 1;
		break;
		case 7: *y = *y - 1;
		break;
		case 8: *x = *x - 1; *y = *y - 1;
		break;
	}
}

int calcBackward(int direccion){
	switch(direccion){
		case 1: return 5;
		case 2: return 6;
		case 3: return 7;
		case 4: return 8;
		case 5: return 1;
		case 6: return 2;
		case 7: return 3;
		case 8: return 4;
	}
}

// ============ TIPO DE AGENTE: ALEATORIO ==============
void* aleatorio(void* arg){
    int tipo = 2; // aleatorio
    int miGrupo = *(int*) arg;

    //Generar una posicion aleatoria
    int x = 0;
    int y = 0;
    char miEstado = estado[miGrupo];
    int bandera = 0;
    while(!bandera){ 
        x = (rand() % ((filas - 1) - 0 + 1));
        y = (rand() % ((columnas - 1) - 0 + 1));
        if(mat[x][y][0] != -1){ // SI no es una pared...
            bandera = 1;
            if(miEstado == 'e'){
                mat[x][y][tipo]++;
            }
            else{
                mat[x][y][4]++;
            }
        }
    }
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
	pthread_mutex_init(&modificar, NULL);

	srand(time(0));

	configuracionMapa();
	configuracionAgente();

	// Se crea un hilo que vaya tomando el tiempo de ejecucion.
	pthread_t timeID;
	pthread_create(&timeID, NULL, runTime, tiempo);

	// Hilo que modifica la variable global "senal" cada segundo.
	pthread_t segundosID;
	pthread_create(&segundosID, NULL, cambioSegundo, NULL);

	pthread_join(timeID, NULL);
	pthread_join(segundosID, NULL);

	// for(int i = 0; i<filas; i++){
	// 	for(int j = 0; j<columnas; j++)
	// 		printf("%i ",mat[i][j][1]);
	// 	printf("\n");
	// }

	pthread_mutex_destroy(&mensaje);
	pthread_mutex_destroy(&modificar);

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


	
	// pthread_mutex_lock(&mensaje);
	// printf("Soy un hilo del grupo %i y mis datos son los siguientes: \n", miGrupo);
	// printf("Velocidad maxima: %i\n", velocMax[miGrupo]);
	// printf("Velocidad minima: %i\n", velocMin[miGrupo]);
	// printf("Estado: %c\n", estado[miGrupo]);
	// pthread_mutex_unlock(&mensaje);
