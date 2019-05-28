#include <curses.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define LINES 11
#define COLLUMNS 11
#define MAX_THREADS 6

#define CURSOR_PAIR   1
#define THREAD_PAIR    2

void start_game();
void create_timer();
void clear_line(int x);

unsigned short int max_time;
unsigned long speed;
bool gameRunning;
bool time_out;
bool winner;

void print() {
    clear();
    int count;
	int y = 5, x = 5, w = 4, h = 2;
    mvprintw(0, 0, "Escolha uma dificuldade:");
    mvprintw(1, 0, "(1) Facil");
    mvprintw(2, 0, "(2) Medio");
    mvprintw(3, 0, "(3) Dificil");
	board(stdscr, y, x, LINES, LINES, w, h);
	for(count = 1; count <= LINES; ++count)
	{	int tempy = y + (count - 1) * h + h / 2;
		int tempx = x + (LINES - 1) * w + w / 2;
		mvaddch(tempy, tempx, ' ');
	}
	refresh();
	mvprintw(30, 0, "(q) Sair");
    char choice;
    
    while(gameRunning) {
        refresh();
        choice = getch();

        switch (choice) {
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
            max_time = 5;
            speed = 0.5;
            start_game();
            break;
        
        case 'q':
        case 'Q':
            endwin();
            exit(0);
            break;
        default:
            mvprintw(33, 0, "Opcao invalida! Tente novamente");
            break;
        }
    }
    clear();
}

void board(WINDOW *win, int starty, int startx, int lines, int cols, 
	   int tile_width, int tile_height) {	
    int endy, endx, i, j;
	endy = starty + lines * tile_height;
	endx = startx + cols  * tile_width;
	
	for(j = starty; j <= endy; j += tile_height)
		for(i = startx; i <= endx; ++i)
			mvwaddch(win, j, i, ACS_HLINE);
	for(i = startx; i <= endx; i += tile_width)
		for(j = starty; j <= endy; ++j)
			mvwaddch(win, j, i, ACS_VLINE);
	mvwaddch(win, starty, startx, ACS_ULCORNER);
	mvwaddch(win, endy, startx, ACS_LLCORNER);
	mvwaddch(win, starty, endx, ACS_URCORNER);
	mvwaddch(win, 	endy, endx, ACS_LRCORNER);
	for(j = starty + tile_height; j <= endy - tile_height; j += tile_height)
	{	mvwaddch(win, j, startx, ACS_LTEE);
		mvwaddch(win, j, endx, ACS_RTEE);	
		for(i = startx + tile_width; i <= endx - tile_width; i += tile_width)
			mvwaddch(win, j, i, ACS_PLUS);
	}
	for(i = startx + tile_width; i <= endx - tile_width; i += tile_width)
	{	mvwaddch(win, starty, i, ACS_TTEE);
		mvwaddch(win, endy, i, ACS_BTEE);
	}
	wrefresh(win);
}

void start_game() {
    clear_line(0);
    clear_line(1);
    clear_line(2);
    clear_line(3);
    // attron(COLOR_PAIR(CURSOR_PAIR));
    // mvaddch(13.5, 14, CURSOR_PAIR);
    // mvaddch(13.5, 15, CURSOR_PAIR);
    // mvaddch(13.5, 16, CURSOR_PAIR);
    // mvaddch(14, 1, CURSOR_PAIR);
    // attroff(COLOR_PAIR(CURSOR_PAIR));
    create_timer();
}

void create_timer() {
    for(int i = 0; i <= max_time; i++) {
        clear_line(30); 
        mvprintw(30, 0, "Tempo restante: %i", max_time - i);
        sleep(1);
        refresh();
    }
    time_out = true;
    gameRunning = false;
}

void clear_line(int x) {
    move(x, 0);          // move o cursor pro começo da linha
    clrtoeol();          // deleta a linha
}

int main() {
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
    init_pair(CURSOR_PAIR, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(THREAD_PAIR, COLOR_RED, COLOR_RED);
    
    bool run = true;
    
    while(run) {
        gameRunning = true;
        winner = false;
        time_out = false;
        print();

	    mvprintw(0, 0, "O jogo acabou! O resultado final foi:");
        if(winner) {
            mvprintw(1, 0, "Parabens! Voce venceu!");
        } else {
            mvprintw(1, 0, "Parabens! Voce perdeu!");
        }

        mvprintw(5, 0, "Deseja jogar novamente?");
        mvprintw(6, 0, "Sim (S)");
        mvprintw(7, 0, "Nao (qualquer tecla)");

        switch (getch()) {    
            case 's':
            case 'S':
                break;            

            default:
                run = false;
                break;
        }
    }
    endwin();
    exit(0);
}