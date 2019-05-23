
//Bibliotecas padrão
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

//Constantes
#define MAX_THREADS 5

//Declaração de métodos.
cont(int initialValue, int interval);
init_threads();

//Variáveis globais
bool endTime = false;
bool playerWiner = false;

int main(void) {
	printf("Escolha uma dificuldade:\n");
	printf("1 - Fácil\n");
	printf("2 - Médio\n");
	printf("3 - Dificil\n");
	init_threads();
    return 1;
}

void init_threads() {
  
}

void runGame() {
	while(!endTime || !playerWiner) {

	}
}

void cont(int initialValue, int interval) {
    while(initialValue > 0) {
        initialValue--;
        sleep(interval);
    }
}
