#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


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


// ===================== VACUNAS =====================
// variables o funciones globales...


// ============ TIPO DE AGENTE: CICLICO (Adrian) ==============

// 
pthread_mutex_t mensaje;

void* ciclico(void* arg){
	int tipo = 1; // Ciclico

	//Generar una posicion aleatorio
	int x = 0;
	int y = 0;
	int bandera = 0;
	while(!bandera){
		x = (rand() % ((filas - 1) - 0 + 1));
		y = (rand() % ((columnas - 1) - 0 + 1));
		if(mat[x][y][0] != -1){ // SI no es una pared...
			bandera = 1;
		}
	}

	

	pthread_mutex_lock(&mensaje);
	printf("Coordenada: %i, %i\n", posI, posJ);
	pthread_mutex_unlock(&mensaje);


}



int main (int argc, char **argv)
{
	pthread_mutex_init(&mensaje, NULL);

	configuracionMapa();

	srand(time(0));

	pthread_t ids[5];
	for(int i = 0; i<5; i++){
		pthread_create(&ids[i], NULL, ciclico, NULL);
	}

	for(int i = 0; i<5; i++){
		pthread_join(ids[i], NULL);
	}

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

	return 0;	
}


// Funcion que lee el archivo de configuracion del mapa ("configMapa.txt")
// y crea la matriz en base a los valores leidos.  
void configuracionMapa(){

	char singleLine[150];
	FILE * fPointer = fopen("configMapa.txt", "r");

	fgets(singleLine, 150, fPointer);

	// Se lee la cantidad de filas y columnas
	sscanf(singleLine, "%i %i\n", &columnas, &filas);

	filas++;
	columnas++;

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
