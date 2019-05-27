#include <curses.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define MAX_THREAD 5
#define LINES 5
#define COLLUMNS 5
#define EMPTY     ' '
#define CURSOR_PAIR   1
#define TOKEN_PAIR    2
#define EMPTY_PAIR    3
#define TRUE 1
#define FALSE 0

//Assinaturas de métodos
void init_threads_and_cursor();
void init_table(void);
void verify_killed_ball();
void create_threads();
void *move_thread(int index);
void create_timer(void);
void refresh_table(void);


//Estrutura de uma token e do cursor.
typedef struct ball {
    int x;
    int y;
    bool alive;
} ball;


ball cursor;
ball balls[MAX_THREAD];
bool time_out, winner;
int max_time = 60;
int speed = 2;
int killed = 0;

//Mutexs
int table[LINES][COLLUMNS];;
pthread_mutex_t mutex;
int main() {
    int key;
    srand(time(NULL));

    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();

    /* inicializa colors */

    if (has_colors() == FALSE) {
        endwin();
        printf("Seu terminal nao suporta cor\n");
        exit(1);
    }

    start_color();
    
    /* inicializa pares caracter-fundo do caracter */

    pthread_mutex_init(&mutex, NULL);

    init_pair(CURSOR_PAIR, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(TOKEN_PAIR, COLOR_RED, COLOR_RED);
    init_pair(EMPTY_PAIR, COLOR_BLUE, COLOR_BLUE);
    clear();
    init_table();
    init_threads_and_cursor();
    create_threads();
    do {   
        key = getch();

        switch (key) {   
            case KEY_UP:
            case 'w':
            case 'W':
            if ((cursor.y > 0)) {
                cursor.y = cursor.y - 1;
            }
            break;

            case KEY_DOWN:
            case 's':
            case 'S':
            if ((cursor.y < LINES - 1)) {
                cursor.y = cursor.y + 1;
            }
            break;

            case KEY_LEFT:
            case 'a':
            case 'A':
            if ((cursor.x > 0)) {
                cursor.x = cursor.x - 1;
            }
            break;
            
            case KEY_RIGHT:
            case 'd':
            case 'D':
            if ((cursor.x < COLS - 1)) {
                cursor.x = cursor.x + 1;
            }
            break;
        }
        verify_killed_ball();
        refresh_table();
    }while ((key != 'q') && (key != 'Q') || !time_out || !winner);
    endwin();
    pthread_mutex_destroy(&mutex);

    if(time_out) {
        printf("Voce perdeu! Nao conseguir matar as threads a tempo!");
    } else if(winner) {
        printf("Ce eh o bixao memo hein doido");
    }
    exit(0);
}

void init_threads_and_cursor(void) {
    pthread_mutex_lock(&mutex);
    
    table[6][6]<-0;
    cursor.x = 6;
    cursor.y = 6;

    for(int i = 0; i < MAX_THREAD; i++) {
        int x, y = 0;
        do {
            x = rand()%(COLLUMNS);
            y = rand()%(LINES);
        } while ((table[x][y] != -1) || ((x == cursor.x) && (y == cursor.y)));
        balls[i].x = x;
        balls[i].y = y;
        balls[i].alive = TRUE;
    }
    pthread_mutex_unlock(&mutex);
}


void init_table(void) {
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < LINES; i++) {
        for (int j = 0; j < COLLUMNS; j++) {
            table[i][j] = -1;
        }
    }
    refresh();
    pthread_mutex_unlock(&mutex);
}

void verify_killed_ball(void) {
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < MAX_THREAD; i++) {
        if(balls[i].x == cursor.x && balls[i].y == cursor.y) {
            balls[i].alive = FALSE;
            killed++;
        }
    }
    if(killed == 5) {
        winner = TRUE;
    }
    pthread_mutex_unlock(&mutex);
}

void create_threads() {
    for(int i = 0; i < MAX_THREAD; i++) {
        pthread_create(NULL, NULL, move_thread(i), (void *) &balls[i]);
    }

    for (int i = 0; i < MAX_THREAD; i++) 
        pthread_join(&balls[i], NULL);
}

void *move_thread(int index) {
    while(!winner || !time_out || balls[index].alive) {
        int x, y = 0;
        pthread_mutex_lock(&mutex);
        do {
            x = rand()%(COLLUMNS);
            y = rand()%(LINES);
        } while ((table[x][y] != -1) || ((x == cursor.x) && (y == cursor.y)));
    
        table[balls[index].x][balls[index].y] = -1;
        table[x][y]=index;
        balls[index].x = x;
        balls[index].y = y;
        refresh_table();
        pthread_mutex_unlock(&mutex);
        sleep(speed);
    }
}

void create_timer(void) {
    for(int i = 0; i < max_time; i++) {
        sleep(1);
    }
    time_out = TRUE;
}


void refresh_table(void) {
    int x, y, i;
    for (x = 0; x < COLLUMNS; x++) {
        for (y = 0; y < LINES; y++){
            attron(COLOR_PAIR(EMPTY_PAIR));
            mvaddch(y, x, EMPTY);
            attroff(COLOR_PAIR(EMPTY_PAIR));
        }
    }

    /* poe os tokens no tabuleiro */

    for (i = 0; i < MAX_THREAD; i++) {
        if(balls[i].alive) {
            attron(COLOR_PAIR(TOKEN_PAIR));
            mvaddch(balls[i].y, balls[i].x, EMPTY);
            attroff(COLOR_PAIR(TOKEN_PAIR));
        }
    }
    /* poe o cursor no tabuleiro */
    move(y, x);
    refresh();
    attron(COLOR_PAIR(CURSOR_PAIR));
    mvaddch(cursor.y, cursor.x, EMPTY);
    attroff(COLOR_PAIR(CURSOR_PAIR));
}