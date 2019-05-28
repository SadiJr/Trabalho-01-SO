#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>

#define REP    0
#define CAL    1

#define DELAY_REP 200000
#define DELAY_CAL 600


//Variables globlas
 int x = 10, y = 10;
 int max_y = 0, max_x = 0;
 int x_seg = 0, y_seg = 0;
 int dir_x = 1;
 int dir_y = 1;
 int x_paleta, y_paleta;
 int rep_pil = CAL;
 char key;


pthread_t id_fil_moupilota;
pthread_t id_fil_representa;
 
int error_fil1, error_fil2;

void inicialitzar_entorn();
void crear_marc();
void moure_pilota();

void *fil_moupilota();
void *fil_representa();

int main(){
    inicialitzar_entorn();
    error_fil1 = pthread_create(&id_fil_moupilota, NULL, fil_moupilota, NULL);
    error_fil2 = pthread_create(&id_fil_representa, NULL, fil_representa, NULL);
    if ( error_fil1 != 0 && error_fil2 != 0 )
    {
        clear();
        move(LINES/2,COLS/2);
        printw("ERROR creant fils d'execució.");
        return -1;   
    }
    while( key != 'q' && key != 'Q' )
    {
        key = getch();
        if (key == 'a' || key == 'A') { x_paleta = x_paleta - 2; }
        if (key == 'd' || key == 'D') { x_paleta = x_paleta + 2; }
        if (key == 'w' || key == 'W') { y_paleta = y_paleta - 2; }
        if (key == 's' || key == 'S') { y_paleta = y_paleta + 2; }
    }
 

    endwin();
    kill(&id_fil_moupilota);
    kill(&id_fil_representa);

    return 0;
}

void inicialitzar_entorn(){
    initscr();
    noecho();
    curs_set(FALSE);
    x_paleta = COLS / 2;
    y_paleta = LINES - 2;
}


void *fil_moupilota(){
    while(1) {
        while (rep_pil == REP );
        moure_pilota();
        rep_pil = REP;
    }
}

void *fil_representa(){
    while(1) {
        while (rep_pil == CAL );
        clear();
        getmaxyx(stdscr, max_y, max_x);
    //Representr marc
        crear_marc();
    //Pintar pilota
    mvprintw(y, x, "O");
    //Pintar paleta
    mvprintw(y_paleta, x_paleta, "==========");
        refresh();
        usleep(DELAY_REP);
        rep_pil = CAL;
    }
}


void moure_pilota(){
     x_seg = x + dir_x;
     y_seg = y + dir_y;
      
      //Si xoca contra laterals rebota
     if (x_seg >= max_x || x_seg < 0 ){dir_x*= -1;}else{x+= dir_x;}
      //Si xoca contra la paleta, rebota 
     if ( ( y_seg < 0 ) || 
              ( (y_seg >= y_paleta) && ( x_seg >= x_paleta ) && (x_seg <= (x_paleta + 10)) )
           )     {dir_y*= -1;}else{y+= dir_y;}
//     if (y_seg >= max_y || y_seg <= 0 || y_seg >= y_paleta){dir_y*= -1;}else{y+= dir_y;}
    
}

void crear_marc()
{
    int i;
        for(i=0;i<LINES;i++)
    {
            move(i,0);
            printw(":");
            move(i,COLS - 1);
            printw(":");
        }
 
    for(i=0;i<COLS;i++)
    {
        move(0,i);
        printw(".");
        move(LINES -1,i);
        printw(".");
    }
}
