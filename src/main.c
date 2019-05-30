#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <pthread.h>


#define MAX_THREADS 5
#define LINES 11
#define COLLUMNS 11
#define CURSOR  1
#define THREAD  2
#define BLANK   3

void main_menu();
void start_game();
void init_positions();
void move_cursor();
void verify_killed_threads();
void init_table();
void refresh_table(int old_x, int old_y, int new_x, int new_y, bool thread);
void clear_line(int x);
void* create_timer();
void move_thread(void* id);
bool verify_free_position(int new_x, int new_y);
void create_cursor();

bool winner, time_out, game_running;
unsigned int max_time;
unsigned long speed;
int killed_threads = 0;

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
pthread_t c;

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
    clear_line(0); clear_line(1);
    clear_line(2); clear_line(3);
    clear_line(4);

    init_positions();
    init_table();
    create_cursor();

    
    pthread_create(&c, NULL, (void *)move_cursor, NULL);
    
    for(int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, move_thread, (void *)i);
    }
    pthread_create(&cron, NULL, create_timer(), NULL);

    // pthread_join(&c, NULL);
    pthread_join(&cron, NULL);
    for(int i = 0; i < MAX_THREADS; i++) {
        pthread_join(&threads[i], NULL);
    }
}

void init_positions() {
    for(int i = 0; i < MAX_THREADS + 1; i++) {
        positions[i].alive = true;
    }
}

void move_cursor() {
    int key;
    do {
        key = getch();
        switch (key) {
        case KEY_UP:
        case 'w':
        case 'W':
            if ((cursor.y > 0)) {
                refresh_table(cursor.x, cursor.y, cursor.x, cursor.y -1, false);
                cursor.y = cursor.y - 1;
            }
            break;
        case KEY_DOWN:
        case 's':
        case 'S':
            if ((cursor.y < LINES - 1)) {
                refresh_table(cursor.x, cursor.y, cursor.x, cursor.y +1, false);
            cursor.y = cursor.y + 1;
            }
            break;
        case KEY_LEFT:
        case 'a':
        case 'A':
            if ((cursor.x > 0)) {
                refresh_table(cursor.x, cursor.y, cursor.x -1, cursor.y, false);
            cursor.x = cursor.x - 1;
            }
            break;
        case KEY_RIGHT:
        case 'd':
        case 'D':
            if ((cursor.x < COLLUMNS - 1)) {
                refresh_table(cursor.x, cursor.y, cursor.x +1, cursor.y, false);
                cursor.x = cursor.x + 1;
            }
            break;
        }
        verify_killed_threads();
  }while ((key != 'q') && (key != 'Q'));
}

void verify_killed_threads() {
    for(int i = 0; i < MAX_THREADS; i++) {
        if(positions[i].x == cursor.x && positions[i].y == cursor.y) {
            killed_threads++;
            positions[i].alive = false;
            if(killed_threads == 5) {
                winner = true;
            }
        }
    }
}

void init_table() {
    for(int i = 0; i < LINES; i++) {
        for(int j = 0; j < COLLUMNS; j++) {
            attron(COLOR_PAIR(BLANK));
            mvaddch(j, i, ' ');
            attroff(COLOR_PAIR(BLANK));
        }
    }
}

void refresh_table(int old_x, int old_y, int new_x, int new_y, bool thread) {
    attron(COLOR_PAIR(BLANK));
    mvaddch(old_x, old_y, ' ');
    attroff(COLOR_PAIR(BLANK));

    if(thread) {
        attron(COLOR_PAIR(THREAD));
        mvaddch(new_x, new_y, THREAD);
        attroff(COLOR_PAIR(THREAD));
    } else {
        move(new_x, new_y);
        attron(COLOR_PAIR(CURSOR));
        mvaddch(new_x, new_y, BLANK);
        attroff(COLOR_PAIR(CURSOR));
    }
    refresh();
}

void clear_line(int x) {
    move(x, 0);          // move o cursor pro começo da linha
    clrtoeol();          // deleta a linha
}

void* create_timer() {
    for(int i = 0; i <= max_time; i++) {
        clear_line(15); 
        mvprintw(15, 0, "Tempo restante: %i", max_time - i);
        sleep(1);
        refresh();
    }
    time_out = true;
    game_running = false;
}

void move_thread(void* arg) {
    int id = (int) arg;
    while (game_running && !time_out && !winner && positions[id].alive) {
        pthread_mutex_lock(&mutex);
        int new_x, new_y;
        do {
            new_x = rand() % LINES;
            new_y = rand() % COLLUMNS;
        } while(!verify_free_position);
        int old_x = positions[id].x;
        int old_y = positions[id].y;
        positions[id].x = new_x;
        positions[id].y = new_y;
        refresh_table(old_x, old_y, new_x, new_y, true);
        pthread_mutex_unlock(&mutex);
        sleep(speed);
    }
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
    refresh_table(0,0,6,6,false);
}