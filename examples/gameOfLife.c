/* Author:   Derek Dang
 * File:     life.c
 * Purpose:  Implement John Conway's Game of Life.  The game is ``played''
 *           on a board or *world* consisting of a rectangular grid with
 *           m rows and n columns.  Each cell in the grid is ``alive'' 
 *           or ``dead.'' An initial generation (generation 0) is either 
 *           entered by the user or generated using a random number 
 *           generator.  
 *
 *           Subsequent generations are computed according to the 
 *           following rules:
 *
 *              - Any live cell with fewer than two live neighbors 
 *              dies, as if caused by under-population.
 *              - Any live cell with two or three live neighbors 
 *              lives on to the next generation.
 *              - Any live cell with more than three live neighbors 
 *              dies, as if by over-population.
 *              - Any dead cell with exactly three live neighbors 
 *              becomes a live cell, as if by reproduction.
 *
 *           Updates take place all at once.
 * 
 * Compile:  gcc -g -Wall -o life life.c -lpthread
 * Run:      ./life <r> <s> <m> <n> <max gens> <'i'|'g'>
 *              r = number of threads for rows.
 *              s = number of threads for cols.
 *              m = number of rows in the world
 *              n = number of cols in the world
 *              max gens = max number of generations
 *              'i' = user will enter generation 0
 *              'g' = program should generate generation 0
 *
 * Input:    If command line has the "input" char ('i'), the first
 *              generation.  Each row should be entered on a separate
 *              line of input.  Live cells should be indicated with
 *              a capital 'X', and dead cells with a blank, ' '.
 *           If command line had the "generate" char ('g'), the
 *              probability that a cell will be alive.
 *
 * Output:   The initial world (generation 0) and the world after
 *           each subsequent generation up to and including
 *           generation = max_gen.  If all of the cells die,
 *           the program will terminate.
 *
 * Notes:
 * 1.  This implementation uses a "toroidal world" in which the
 *     the last row of cells is adjacent to the first row, and
 *     the last column of cells is adjacent to the first.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LIVE 1
#define DEAD 0
#define LIVE_IO 'X'
#define DEAD_IO ' '
#define MAX_TITLE 1000
#define BARRIER_COUNT 1000

int r, s, m, n, max, thread_count = 0, live_count = 0, curr_gen = 0;
int done = 0;
int *w1, *w2;
int barrier_thread_count = 0;
pthread_mutex_t barrier_mutex;
pthread_cond_t ok_to_proceed;

void Usage(char prog_name[]);
void Read_world(char prompt[], int w1[], int m, int n);
void Gen_world(char prompt[], int w1[], int m, int n);
void Print_world(char title[], int w1[]);
void Barrier();
void *Play_life(void* rank);
int  Count_nbhrs(int *w1, int m, int n, int i, int j);

int main(int argc, char* argv[])
{
   long thread;
   pthread_t* thread_handles;
   char ig;

   if (argc != 7) Usage(argv[0]);
   r = strtol(argv[1], NULL, 10);
   s = strtol(argv[2], NULL, 10);
   thread_count = r * s;
   if (thread_count <= 0) Usage(argv[0]);
   m = strtol(argv[3], NULL, 10);
   n = strtol(argv[4], NULL, 10);
   max = strtol(argv[5], NULL, 10);
   ig = argv[6][0];

   thread_handles = malloc(thread_count*sizeof(pthread_t));
   w1 = malloc(m*n*sizeof(int));
   w2 = malloc(m*n*sizeof(int));

   pthread_mutex_init(&barrier_mutex, NULL);
   pthread_cond_init(&ok_to_proceed, NULL);

   if (ig == 'i') Read_world("Enter generation 0", w1, m, n);
   else Gen_world("What's the prob that a cell is alive?", w1, m, n);

   printf("\n");
   Print_world("Generation 0", w1);

   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
          (void*) Play_life, (void*) thread);

   for (thread = 0; thread < thread_count; thread++) 
      pthread_join(thread_handles[thread], NULL); 

   pthread_mutex_destroy(&barrier_mutex);
   pthread_cond_destroy(&ok_to_proceed);
   free(w1);
   free(w2);
   free(thread_handles);
}  /* main */


/*---------------------------------------------------------------------
 * Function:   Usage
 * Purpose:    Show user how to start the program and quit
 * In arg:     prog_name
 */
void Usage(char prog_name[])
{
   fprintf(stderr, "usage: %s <rows> <cols> <max> <i|g>\n", prog_name);
   fprintf(stderr, "       r = number of threads for rows\n");
   fprintf(stderr, "       s = number of threads for columns\n");
   fprintf(stderr, "       m = number of rows in the world\n");
   fprintf(stderr, "       n = number of cols in the world\n");
   fprintf(stderr, "     max = max number of generations\n");
   fprintf(stderr, "       i = user will enter generation 0\n");
   fprintf(stderr, "       g = program should generate generation 0\n");
   exit(0);
}  /* Usage */


/*---------------------------------------------------------------------
 * Function:   Read_world
 * Purpose:    Get generation 0 from the user
 * In args:    prompt
 *             m:  number of rows in visible world
 *             n:  number of cols in visible world
 * Out arg:    w1:  stores generation 0
 *
 */
void Read_world(char prompt[], int w1[], int m, int n)
{
   int i, j;
   char c;

   printf("%s\n", prompt);
   for (i = 0; i < m; i++)
   {
      for (j = 0; j < n; j++)
      {
         scanf("%c", &c);
         if (c == LIVE_IO)
            w1[i*n + j] = LIVE;
         else
            w1[i*n + j] = DEAD;
      }
      /* Read end of line character */
      scanf("%c", &c);
  }
}  /* Read_world */


/*---------------------------------------------------------------------
 * Function:   Gen_world
 * Purpose:    Use a random number generator to create generation 0 
 * In args:    prompt
 *             m:  number of rows in visible world
 *             n:  number of cols in visible world
 * Out arg:    w1:  stores generation 0
 *
 */
void Gen_world(char prompt[], int w1[], int m, int n)
{
   int i, j;
   double prob;
#  ifdef DEBUG
   int live_count = 0;
#  endif
   
   printf("%s\n", prompt);
   scanf("%lf", &prob);

   srandom(1);
   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
         if (random()/((double) RAND_MAX) <= prob)
         {
            w1[i*n + j] = LIVE;
#           ifdef DEBUG
            live_count++;
#           endif
         } 
         else w1[i*n + j] = DEAD;

#  ifdef DEBUG
   printf("Live count = %d, request prob = %f, actual prob = %f\n",
         live_count, prob, ((double) live_count)/(m*n));
#  endif
}  /* Gen_world */


/*---------------------------------------------------------------------
 * Function:   Print_world
 * Purpose:    Print the current world
 * In args:    title
 *             m:  number of rows in visible world
 *             n:  number of cols in visible world
 *             w1:  current gen
 *
 */
void Print_world(char title[], int w1[])
{
   int i, j;
   
   for (i = 0; i < m; i++)
   {
      for (j = 0; j < n; j++)
         if (w1[i*n + j] == LIVE)
            printf("%c", LIVE_IO);
         else
            printf("%c", DEAD_IO);
      printf("\n");
   }
   printf("%s\n\n", title);
}  /* Print_world */


/*-------------------------------------------------------------------
 * Function:    Barrier
 * Purpose:     Implement barrier
 * In arg:      none
 * Global var:  thread_count, barrier_thread_count, barrier_mutex,
 *              ok_to_proceed, w1, w2, curr_gen, live_count, m, n
 * Return val:  Ignored
 */
void Barrier()
{
   int *tmp;
   char title[MAX_TITLE];

   pthread_mutex_lock(&barrier_mutex);
   barrier_thread_count++;
   if (barrier_thread_count == thread_count)
   {
      barrier_thread_count = 0;
      tmp = w1;
      w1 = w2;
      w2 = tmp;
      curr_gen++;
      if (live_count > 0)
      {
         sprintf(title, "Generation %d", curr_gen);
         Print_world(title, w1);
      }
      else
         done = 1;
      live_count = 0;
      pthread_cond_broadcast(&ok_to_proceed);
   }
   else 
      while (pthread_cond_wait(&ok_to_proceed, &barrier_mutex) != 0);
   pthread_mutex_unlock(&barrier_mutex);
} /* Barrier */


/*---------------------------------------------------------------------
 * Function:     Play_life
 * Purpose:      Play Conway's game of life.  (See header doc)
 * Global var:   m, n, r, s, w1, w2, live_count, LIVE, DEAD
 */
void *Play_life(void* rank)
{
   long my_rank = (long) rank;
   int loc_row = m / r, loc_col = n / s;
   int row_rank = my_rank / s, col_rank = my_rank % s;
   int i, j, count;

   int start_row = row_rank * loc_row, last_row = start_row + loc_row;
   int start_col = col_rank * loc_col, last_col = start_col + loc_col;

   while (curr_gen < max && done != 1)
   {
      for (i = start_row; i < last_row; i++)
      {
         for (j = start_col; j < last_col; j++)
         {
            count = Count_nbhrs(w1, m, n, i, j);
#           ifdef DEBUG
            printf("curr_gen = %d, i = %d, j = %d, count = %d\n",
                  curr_gen, i, j, count);
#           endif
            if (count < 2 || count > 3)
               w2[i*n + j] = DEAD;
            else if (count == 2)
               w2[i*n + j] = w1[i*n + j];
            else /* count == 3 */
               w2[i*n + j] = LIVE;
            if (w2[i*n + j] == LIVE) 
               live_count++;
         }
      }
      Barrier();
   }
   return NULL;
}  /* Play_life */


/*---------------------------------------------------------------------
 * Function:   Count_nbhrs
 * Purpose:    Count the number of living nbhrs of the cell (i,j)
 * In args:    w1:  current world
 *             m:   number of rows in world
 *             n:   number of cols in world
 *             i:   row number of current cell
 *             j:   col number of current cell
 * Ret val:    The number of neighboring cells with living neighbors
 *
 * Note:       Since the top row of cells is adjacent to the bottom
 *             row, and since the left col of cells is adjacent to the
 *             right col, in a very small world, it's possible to
 *             count a cell as a neighbor twice.  So we assume that
 *             m and n are at least 3.
 */
int Count_nbhrs(int* w1, int m, int n, int i, int j)
{
   int i1, j1, i2, j2;
   int count = 0;

   for (i1 = i-1; i1 <= i+1; i1++)
      for (j1 = j-1; j1 <= j+1; j1++)
      {
         i2 = (i1 + m) % m;
         j2 = (j1 + n) % n;
         count += w1[i2*n + j2];
      }
      count -= w1[i*n + j];

   return count;
}  /* Count_nbhrs */
