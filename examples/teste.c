    #include <stdio.h>
#include <time.h>
#include <curses.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>

#define STEP 1 //value of the moving
#define M 2 // number of items
#define BUFFER_DIM 100000

void * control_func(void * arg);
void * item_func(void * arg);

typedef struct pos{//structure for the communication
        char c; // type of item
        int x; // x coord
        int y; // y coord
        int p; // object pos
        int pid; // pid of process
} pos;

typedef struct argomenti{ //structure to pass arguments
    int x; //x position
    int y; //y position
    int i; //index
} args;

//buffer and semaphores initializations
pos buffer[BUFFER_DIM]; 
int IN = 0; // occupied positions of the buffer
int OUT = 0; // avariable position of the buffer
sem_t presents, avariables; // variabili che indicano il valore del semaforo, ovvero la presenza o meno di job disponibili
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex to avoid race conditions

main(){
    initscr();      
    noecho();
    curs_set(0);
    srand(time(NULL));
    pthread_t item_id[M], control_id;
    int i;

    sem_init(&avariables, 0, BUFFER_DIM); //initialize semaphores
    sem_init(&presents, 0, 0);

    //start threads
    for(i=0;i<M;i++){
         pthread_create(&item_id[i], NULL, &item_func, &i);
    }
    pthread_create(&control_id, NULL, &control_func, NULL);
    pthread_join(control_id, NULL);

    endwin();
    return(0);
}

void * item_func(void * arg){
    pos pos_item;
    int i = *((int *)arg);
    int dx=STEP;

    pos_item.c='$';
    pos_item.p=i;
    pos_item.pid=pthread_self();
    pos_item.y=1;
    pos_item.x=rand()%80;

    sem_wait(&avariables);
    pthread_mutex_lock(&mutex);
    buffer[IN] = pos_item;
    IN=(IN+1)%BUFFER_DIM;
    sem_post(&presents); // increment of the semaphore
    pthread_mutex_unlock(&mutex);

    while(1){//code for the changes of the coords
        if(pos_item.x==80){//80 are the columns
            dx=-STEP;
            pos_item.x += dx;
            pos_item.y++;
        }else if(pos_item.x==0){
            dx=STEP;
            pos_item.x += dx;
            pos_item.y++;
        }else
            pos_item.x += dx;

        sem_wait(&avariables);
        pthread_mutex_lock(&mutex);
        buffer[IN] = pos_item;
        IN=(IN+1)%BUFFER_DIM;
        sem_post(&presents); // increment of the semaphore
        pthread_mutex_unlock(&mutex);

        usleep(100000);}
}

void * control_func(void * arg)
{
    pos item[M], read_value;
    int i,j;

    for(i=0;i<M;i++){
          item[i].x=-1;
    }

    do{
        sem_wait(&presents);
        pthread_mutex_lock(&mutex);
        read_value = buffer[OUT];
        OUT=(OUT+1)%BUFFER_DIM;
        sem_post(&avariables); // increment of the semaphore
        pthread_mutex_unlock(&mutex);

        switch(read_value.c) {
          case '$':
            if(item[read_value.p].x>=0)
                mvaddch(item[read_value.p].y,item[read_value.p].x,' ');  //delete the old position     
            item[read_value.p]=read_value; //refresh the position
            mvaddch(item[read_value.p].y,item[read_value.p].x,item[read_value.p].c); //print the item
            break;
        }
    refresh();
    }while(1);
}
