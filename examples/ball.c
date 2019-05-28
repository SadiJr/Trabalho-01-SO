#include <stdio.h>
#include <curses.h>
#include <sys/time.h>
#include <signal.h>

void drawStart(WINDOW *win);
void drawScreen(WINDOW *win, int ballyold, int ballxold, int bally, int ballx);
void setAlarm(long milleseconds);
void onAlarm(int sig);
void cleanUp(int sig);
void moveBall(int *ballxold, int *ballyold, int *ballx, int *bally, 
	      int *ballx_vel, int *bally_vel);
void moveCursor(WINDOW *win);

#define MIN_X		11
#define MAX_X		69
#define MIN_Y		5
#define MAX_Y 		19
#define DELAY		200

int main(int argc, char *argv[]) {
   WINDOW *main_window;
   int ballxold, ballyold;	/* previous ball location */
   int ballx, bally;		/* current ball location */
   int ballx_vel, bally_vel;	/* ball velocity */

   /* initialize curses stuff */  
   initscr();
   main_window = newwin(0,0,0,0);
   leaveok(main_window,TRUE);
   curs_set(0);			/* turn off cursor */

   /* set up interrupt handlers */
   signal(SIGALRM, onAlarm);
   signal(SIGINT, cleanUp);	/* Ctrl-C to exit */

   /* initialize ball */
   ballx = (MAX_X - MIN_X) / 2 + MIN_X;
   bally = (MAX_Y - MIN_Y) / 2 + MIN_Y;
   ballxold = ballx;
   ballyold = bally;
   ballx_vel = -1;
   bally_vel = +1;

   /* draw starting box */
   drawStart(main_window);

   /* loop until exit on Ctrl-C */
   while (1) { 
     setAlarm(DELAY);
     pause();			/* wait here until alarm goes off */
     moveBall(&ballxold, &ballyold, &ballx, &bally, &ballx_vel, &bally_vel);
     drawScreen(main_window, ballxold, ballyold, ballx, bally);
   }
}

/* move the ball based on x,y velocity and walls */
void moveBall(int *ballxold, int *ballyold, int *ballx, int *bally, 
	      int *ballx_vel, int *bally_vel) {

  /* move the ball */
  *ballxold = *ballx;
  *ballyold = *bally;
  *ballx = *ballx + *ballx_vel;
  *bally = *bally + *bally_vel; 

  /* hit side wall? */
  if (*ballx == MIN_X || *ballx == MAX_X)
    *ballx_vel = *ballx_vel * -1; 
  
  /* hit top/bottom wall? */
  if (*bally == MAX_Y || *bally == MIN_Y)
    *bally_vel = *bally_vel * -1; 
}

/* draw the screen */
void drawScreen(WINDOW *win, int ballxold, int ballyold, int ballx, int bally){
  mvwaddch(win, ballyold, ballxold,' ');
  mvwaddch(win, bally, ballx, '*');
  wrefresh(win);
}

/* draw the starting box */
void drawStart(WINDOW *win) {
  int i;

  /* horizontal bars */
  for (i=MIN_X-1; i<=MAX_X+1; i++) {
    mvwaddch(win, MIN_Y-1, i, '-');
    mvwaddch(win, MAX_Y+1, i, '-');
  }

  /* vertical bars */
  for (i=MIN_Y-1; i<=MAX_Y+1; i++) {
    mvwaddch(win, i, MIN_X-1, '|');
    mvwaddch(win, i, MAX_X+1, '|');
  }    

  wrefresh(win);
}

/* set the alarm to go off in the specified number of msecs */
void setAlarm(long milleseconds) {
  struct itimerval length;

  length.it_interval.tv_sec = 0;
  length.it_interval.tv_usec = 0;

  length.it_value.tv_sec = milleseconds/1000;
  length.it_value.tv_usec = (milleseconds % 1000) * 1000;

  if (setitimer(ITIMER_REAL,&length,NULL) < 0)
    perror("setitimer");
}

/* reset signal handler (not needed on every system) */
void onAlarm(int sig) {
  signal(SIGALRM, onAlarm);
}

/* close curses and exit */
void cleanUp(int sig) {
  endwin();
  exit(0);
}
