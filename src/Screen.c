#include <unistd.h>
#include <ncurses.h>

#define MAX_THREADS 5
int positions[5];
void * initScreen(void) {
	initsrc();
	noecho();

}
