
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include <semaphore.h>

#define MAX_THREADS 5

int contador_global = 0; // Global
/* The mutex lock */
pthread_mutex_t mutex;



void *incrementa(void *param) {
	for(int i = 0; i < 100; i++){
		pthread_mutex_lock(&mutex);
    	if (contador_global % 100 == 0)
       		contador_global += 2;
    	else
    		contador_global++; 
		pthread_mutex_unlock(&mutex); 
	} 
	pthread_exit(NULL);
}





int main(int argc, char *argv[]) {  
	int i;
   	pthread_t tid[MAX_THREADS];
   	pthread_mutex_init(&mutex, NULL);
   	for(i = 0; i < MAX_THREADS; i++) {
      	pthread_create(&tid[i],NULL,incrementa,NULL);
    }

   	for(i = 0; i < MAX_THREADS; i++) {
      	pthread_join(tid[i],NULL);
   	}
  	pthread_mutex_destroy(&mutex);
   	printf("contador_global = %i\n", contador_global);
   	printf("Exit the program\n");
   	exit(0);
}
