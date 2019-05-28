
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include <sched.h>
#define N 5

int contador_global = 0; // Global
/* The mutex lock */
pthread_mutex_t garfo[N];
pthread_mutex_t mutex;



void *filosofo(void *param) {
	int i = 0, f;
	pthread_mutex_lock(&mutex);
	f = *((int*) param);
	pthread_mutex_unlock(&mutex);
	printf("Filosofo %i sentou na mesa\n", f);
	while(i++<10){
		printf("Filosofo %d pensando...\n", f);
		sleep(1);
		pthread_mutex_lock(&garfo[f%N]);
        sched_yield(); // Cede o processador e vai para o final da fila ready
		pthread_mutex_lock(&garfo[(f+1)%N]);
		printf("Filosofo %d comendo...\n", f);
		sleep(1);
		pthread_mutex_unlock(&garfo[(f+1)%N]);
		pthread_mutex_unlock(&garfo[f%N]);
		printf("Filosofo %d cochilando...\n", f);
		sleep(1);
	}
	
    return 0;
}





int main(int argc, char *argv[]) {  
	int i;
   	pthread_t tid[N];
   	pthread_mutex_init(&mutex, NULL);
   	for(i = 0; i < N; i++)
   		pthread_mutex_init(&garfo[i], NULL);
	for(i = 0; i < N; i++){
    	pthread_create(&tid[0],NULL,filosofo, (void *) &i);
    	printf("Nasce o filosofo %i\n", i);
	}
    

   	for(i = 0; i < N; i++) {
      	pthread_join(tid[i],NULL);
      	printf("Filosofo %d morreu...\n", i);
   }
   	exit(0);
}
