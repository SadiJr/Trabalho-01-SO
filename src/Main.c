#include <curses.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define LINES 11
#define COLLUMNS 11
#define MAX_THREADS 6

void create_window();
void refresh_window();
void create_threads();
void move_threads();
void check_time();

void print() {
    int count;
	int y = 5, x = 5, w = 4, h = 2;
    mvprintw(0, 0, "Escolha uma dificuldade:");
    mvprintw(1, 0, "(1) Facil");
    mvprintw(2, 0, "(2) Medio");
    mvprintw(3, 0, "(3) Dificil");
	board(stdscr, y, x, 11, 11, w, h);
	for(count = 1; count <= 11; ++count)
	{	int tempy = y + (count - 1) * h + h / 2;
		int tempx = x + (11 - 1) * w + w / 2;
		mvaddch(tempy, tempx, ' ');
	}
	refresh();
	mvprintw(30, 0, "(q) Sair");
    char choice;
    
    while(1) {
        choice = getch();

        switch (choice) {
        case '1':
            
            break;
        
        case '2':
            break;
        
        case '3':
            break;
        
        case 'q':
        case 'Q':
            endwin();
            exit(0);
            break;
        default:
            break;
        }
    }
    clear();
}

void board(WINDOW *win, int starty, int startx, int lines, int cols, 
	   int tile_width, int tile_height)
{	int endy, endx, i, j;
	
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

int main() {
    initscr();
	cbreak();
	keypad(stdscr, TRUE);
    noecho();
    print();

    endwin();
    exit(0);
}