#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_THREADS 10

void work(void){
  printf("Nova thread criada. TID = %u\n",  pthread_self());
  pthread_exit(NULL);
}
 
void main(){
  pthread_t worker[MAX_THREADS];
  int i;  
   
  for(i=0; i < MAX_THREADS; i++){
	int ret = pthread_create(&worker[i],NULL,(void *)work,NULL);  // Cria Thread
  	if(ret<0){ 
    		printf("Criacao de thread falhou\n");  
    		pthread_exit(NULL);
  	}
   }
   for(i=0; i < MAX_THREADS; i++)
  	pthread_join(worker[i],NULL);  // Pai espera filho terminar 
   pthread_exit(NULL);
}


