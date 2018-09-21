 /* File:     lab2part3.c
 *
 * Purpose:  A handwritten multithreaded and threadsafe string tokenzier
 * 
 * Compile:  gcc ...
 *           
 * Usage:    ./lab2part3 <number of threads> < <input>
 * Input:    lines of text
 * Output:   the line read by the program, and the tokens identified by my_strtok
 *
 * Note:     For each line of input, next thread reads the line, and "tokenizes" it. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

const int MAX = 1000;

int thread_count;
sem_t* sems;

char *my_strtok(char* seps, char** next_string_p);
int separator(char* current, char* seps);
void *Tokenize(void* rank);  /* Thread function */

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles; 

   thread_count = atoi(argv[1]);

   thread_handles = (pthread_t*) malloc (thread_count*sizeof(pthread_t));
   sems = (sem_t*) malloc(thread_count*sizeof(sem_t));
   sem_init(&sems[0], 0, 1);
   for (thread = 1; thread < thread_count; thread++)
      sem_init(&sems[thread], 0, 0);

   printf("Enter text\n");
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], (pthread_attr_t*) NULL, Tokenize, (void*) thread);

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);

   for (thread=0; thread < thread_count; thread++)
      sem_destroy(&sems[thread]);

   free(sems);

   free(thread_handles);
   return 0;
}  /* main */

/*-------------------------------------------------------------------
 * Function:    my_strtok
 * Purpose:     return the next token in the string
 * In arg:      seps, characters that can separate successive tokens
 * In/out arg:  next_string_p
 *                 on input: pointer to separator or start of next token in input.
 *                 on output: pointer to separator marking end of returned string.
 * Return val:  pointer to copy of next token in in_string or NULL if no new token
 *
 */
char *my_strtok(char* seps, char** next_string_p) {
   char* token;
   int   length = 0;
   char* start;
   char* current = *next_string_p;

   /* Find beginning of next token */
   while (separator(current, seps))
      if ((*current == '\0') || (*current == '\n'))
         return NULL;
      else
         current++;
   start = current;

   /* Find end of token */
   while (!separator(current, seps)) {
      length++;
      current++;
   }

   /* Create storage for token and copy current token */
   token = (char*) malloc((length+1)*sizeof(char));
   strncpy(token, start, length);
   token[length] = '\0';

   /* Update pointer to start of new separator */
   *next_string_p = current;

   return token;
}  /* my_strtok */

/*-------------------------------------------------------------------
 * Function:    separator
 * Purpose:     determine whether the current character is a separator or \0
 * In args:     current:  pointer to current character
 *              seps:  list of characters to checkrank
 */
int separator(char* current, char* seps) {
   int len = strlen(seps);
   int i;

   if (*current == '\0') return 1;
   for (i = 0; i < len; i++)
      if (*current == seps[i]) return 1;
   return 0;
}  /* separator */


/*-------------------------------------------------------------------
 * Function:    Tokenize
 * Purpose:     Tokenize lines of input
 * In arg:      rank
 * Global vars: thread_count (in), sems (in/out)
 * Return val:  Ignored
 */
void *Tokenize(void* rank) {
   long my_rank = (long) rank;
   int count;
   char *fg_rv;
   char my_line[MAX+1];
   char *my_string;
   char *next_string;

   /* Have each thread read consecutive lines and tokenize them in turn */
   fg_rv = fgets(my_line, MAX, stdin);
   while (fg_rv != NULL) {
      printf("Thread %ld > my line = %s", my_rank, my_line);

      count = 0;
      next_string = my_line;
      my_string = my_strtok(" \t\n", &next_string);
      while ( my_string != NULL ) {
         count++;
         printf("Thread %ld > string %d = %s\n", my_rank, count, my_string);
         free (my_string);
         my_string = my_strtok(" \t\n", &next_string);
      }
      fg_rv = fgets(my_line, MAX, stdin);
   }

   return NULL;
}  /* Tokenize */
