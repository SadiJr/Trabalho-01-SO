#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>

#define MAX_THREADS 5
#define LINES 11
#define COLLUMNS 11
#define CURSOR  1
#define THREAD  2
#define BLANK   3

void main_menu();
void start_game();
void init_positions();
void *move_cursor();
void verify_killed_threads();
void init_table();
void refresh_table(int x, int y, int i);
void clear_line(int x);
void *create_timer();
void *move_thread(void *id);
bool verify_free_position(int new_x, int new_y);
void create_cursor();

bool winner, time_out, game_running;
bool first_game = true;
unsigned int max_time;
unsigned long speed;
int killed_threads;
int record;

typedef struct Ball {
    int x;
    int y;
    bool alive;
} ball;

ball positions[MAX_THREADS];
ball cursor;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t threads[MAX_THREADS];
pthread_t cron;
//TODO renomear esse caralho,
pthread_t cu;

int main() {
    srand(time(NULL));
    initscr();
	cbreak();
	keypad(stdscr, TRUE);
    noecho();
    
    if (has_colors() == FALSE) {
        endwin();
        printf("Seu terminal nao suporta cor\n");
        exit(1);
    }

    start_color();
    init_pair(CURSOR, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(THREAD, COLOR_RED, COLOR_RED);
    init_pair(BLANK, COLOR_BLUE, COLOR_BLUE);

    main_menu();
    endwin();
    exit(0);
}

void main_menu() {
    if(!first_game) {
        pthread_mutex_unlock(&mutex);
        clear();
        refresh();

        if(winner) {
            mvprintw(0, 0, "Parabens, voce ganhou!");
            mvprintw(1, 0, "Matou todas as threads em %i segundos!", record);
        } else if (time_out) {
            mvprintw(0, 0, "Parabens, voce perdeu!");
            mvprintw(1, 0, "Matou ao todo %i threads em %i segundos!", killed_threads, max_time);
        }

        mvprintw(3, 0, "Deseja iniciar um novo jogo?");
        mvprintw(4, 0, "(S/s) Sim");
        mvprintw(5, 0, "(N/n) Nao");
        switch (getch()) {
            case 'S':
            case 's':
                break;
            
            case 'N':
            case 'n':
                pthread_mutex_destroy(&mutex);
                endwin();
                exit(0);
                break;
            
            default:
                main_menu();
                break;
        }
    }
    clear();
    mvprintw(0, 0, "Escolha uma dificuldade:");
    mvprintw(1, 0, "(1) Facil");
    mvprintw(2, 0, "(2) Medio");
    mvprintw(3, 0, "(3) Dificil");
    mvprintw(4, 0, "(q) Sair");

    switch (getch()) {
    case '1':
        max_time = 120;
        speed = 2.0;
        start_game();
        break;
    
    case '2':
        max_time = 60;
        speed = 1.0;
        start_game();
        break;
    case '3':
        max_time = 30;
        speed = 0.5;
        start_game();
        break;

    case 'q':
    case 'Q':
        pthread_mutex_destroy(&mutex);
        endwin();
        exit(0);
    default:
        main_menu();
        break;
    }
}

void start_game() {
    game_running = true;
    winner = false;
    time_out = false;
    killed_threads = 0;
    clear_line(0); clear_line(1);
    clear_line(2); clear_line(3);
    clear_line(4);

    init_positions();
    init_table();
    create_cursor();

    pthread_create(&cu, NULL, move_cursor, NULL);
    
    for(int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, move_thread, (void *) (intptr_t)i);
    }
    pthread_create(&cron, NULL, create_timer, NULL);

    pthread_join(cu, NULL);
    pthread_join(cron, NULL);
    for(int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}

void init_positions() {
    for(int i = 0; i < MAX_THREADS; i++) {
        positions[i].alive = true;
        positions[i].x = 0;
        positions[i].y = 0;
    }
}

void *move_cursor() {
    int key;
    do {
        key = getch();
        pthread_mutex_lock(&mutex);
        int x = cursor.x;
        int y = cursor.y;
        
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
            if ((cursor.x < COLLUMNS - 1)) {
                cursor.x = cursor.x + 1;
            }
            break;
        }
        verify_killed_threads();
        refresh_table(x, y, -1);
        pthread_mutex_unlock(&mutex);
  }while ((key != 'q') && (key != 'Q'));
  game_running = false;
  pthread_cancel(cron);
  pthread_exit(0);
}

void verify_killed_threads() {
    for(int i = 0; i < MAX_THREADS; i++) {
        if(positions[i].alive && positions[i].x == cursor.x && positions[i].y == cursor.y) {
            killed_threads++;
            positions[i].alive = false;
            // mvprintw(20, 0, "mATANDO THREAD %i", i);
            if(killed_threads == 5) {
                // mvprintw(20, 0, "Matou ao todo %i threads", killed_threads);
                pthread_cancel(cron);
                first_game = false;
                winner = true;
                main_menu();
                pthread_exit(0);
            }
        }
    }
}

void init_table() {
    clear();
    for(int i = 0; i < LINES; i++) {
        for(int j = 0; j < COLLUMNS; j++) {
            attron(COLOR_PAIR(BLANK));
            mvaddch(j, i, ' ');
            attroff(COLOR_PAIR(BLANK));
        }
    }
    refresh();
}

void refresh_table(int x, int y, int i) {
    // attron(COLOR_PAIR(BLANK));
    // mvaddch(old_x, old_y, ' ');
    // attroff(COLOR_PAIR(BLANK));

    // if(thread) {
    //     attron(COLOR_PAIR(THREAD));
    //     mvaddch(new_x, new_y, THREAD);
    //     attroff(COLOR_PAIR(THREAD));
    // } else {
    //     move(new_x, new_y);
    //     attron(COLOR_PAIR(CURSOR));
    //     mvaddch(new_x, new_y, BLANK);
    //     attroff(COLOR_PAIR(CURSOR));
    // }
    // refresh();
    // int x, y, i;

    /* redesenha tabuleiro "limpo" */
    attron(COLOR_PAIR(BLANK));
    mvaddch(y, x, ' ');
    attroff(COLOR_PAIR(BLANK));
    
    // for (x = 0; x < COLLUMNS; x++) 
    //     for (y = 0; y < LINES; y++){
    //     attron(COLOR_PAIR(BLANK));
    //     mvaddch(y, x, ' ');
    //     attroff(COLOR_PAIR(BLANK));
    // }

    /* poe os tokens no tabuleiro */
    if(i > -1) {
        if(positions[i].alive) {
            attron(COLOR_PAIR(THREAD));
            mvaddch(positions[i].y, positions[i].x, ' ');
            attroff(COLOR_PAIR(THREAD));
        }
    }

    // for (i = 0; i < MAX_THREADS; i++) {
    //     if(positions[i].alive) {
    //         attron(COLOR_PAIR(THREAD));
    //         mvaddch(positions[i].y, positions[i].x, ' ');
    //         attroff(COLOR_PAIR(THREAD));
    //     }
    // }
    /* poe o cursor no tabuleiro */

    move(y, x);
    attron(COLOR_PAIR(CURSOR));
    mvaddch(cursor.y, cursor.x, ' ');
    attroff(COLOR_PAIR(CURSOR));
    refresh();
}

void clear_line(int x) {
    move(x, 0);          // move o cursor pro começo da linha
    clrtoeol();          // deleta a linha
}

void *create_timer() {
    record = max_time;
    for(int i = 0; i <= max_time; i++) {
        clear_line(15); 
        mvprintw(15, 0, "Tempo restante: %i", max_time - i);
        refresh();
        record--;
        sleep(1);
    }
    first_game = false;
    time_out = true;
    game_running = false;
    pthread_cancel(cu);
    refresh();
    main_menu();
    pthread_exit(0);
}

void *move_thread(void *arg) {
    int id = (intptr_t)arg;
    // printf("Moving thread with id %i, and pid %i", id, pthread_self());
    // printf("Bool variables: game_running %i, time-out %i, winner %i and alive %i", game_running, time_out, winner, positions[id].alive);
    while (game_running && !time_out && !winner && positions[id].alive) {
        pthread_mutex_lock(&mutex);
        // printf("Mutex!");
        int new_x, new_y;
        do {
            new_x = rand() % LINES;
            new_y = rand() % COLLUMNS;
        } while(!verify_free_position(new_x, new_y));
        int old_x = positions[id].x;
        int old_y = positions[id].y;
        positions[id].x = new_x;
        positions[id].y = new_y;
        // printf("Refreshing screen with values = %i %i %i %i", old_x, old_y, new_x, new_y);
        refresh_table(old_x, old_y, id);
        pthread_mutex_unlock(&mutex);
        sleep(speed);
    }
    // mvprintw(20, 0, "mATANDO THREAD %i", id);
    pthread_exit(0);
}

bool verify_free_position(int new_x, int new_y) {
    for(int i = 0; i <= MAX_THREADS; i++) {
        if(positions[i].x == new_x && positions[i].y == new_y 
            || cursor.x == new_x && cursor.y == new_y) {
            return false;
        }
    }
    return true;
}

void create_cursor() {
    cursor.x = 6;
    cursor.y = 6;
}