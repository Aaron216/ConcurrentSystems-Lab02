/* File:     
 *     lab2part4.c
 *
 * Purpose: A simple producer consumer exmple
 *
 * Input: none
 *
 * Output: none
 *
 * Compile:  
 *    gcc ...
 * Usage:
 *    ./lab2part4
 *
 */

#include <stdio.h>
#include <pthread.h>

#define MAX 10000			/* Numbers to produce */
pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;
int buffer = 0;

void *producer(void *ptr);
void *consumer(void *ptr);

int main(int argc, char **argv) {
  pthread_t pro, con;

  // Initialize the mutex and condition variables
  pthread_mutex_init(&the_mutex, NULL);	
  pthread_cond_init(&condc, NULL);		/* Initialize consumer condition variable */
  pthread_cond_init(&condp, NULL);		/* Initialize producer condition variable */

  // Create the threads
  pthread_create(&con, NULL, consumer, NULL);
  pthread_create(&pro, NULL, producer, NULL);

  // Join the threads
  pthread_join(con, NULL);
  pthread_join(pro, NULL);

  // Cleanup
  pthread_mutex_destroy(&the_mutex);	/* Free up the_mutex */
  pthread_cond_destroy(&condc);		    /* Free up consumer condition variable */
  pthread_cond_destroy(&condp);		    /* Free up producer condition variable */
  
  printf("Final value in buffer = %d\n",buffer); 
  return 0;
}

void *producer(void* ptr) {
  int i;

  for (i = 1; i <= MAX; i++) {
    pthread_mutex_lock(&the_mutex);	        
    while (buffer != 0)		                
      pthread_cond_wait(&condp, &the_mutex);
    buffer = i;
    pthread_cond_signal(&condc);	        
    pthread_mutex_unlock(&the_mutex);	    
  }
  return NULL;
}

void *consumer(void* ptr) {
  int i;

  for (i = 1; i <= MAX; i++) {
    pthread_mutex_lock(&the_mutex);
    while (buffer == 0)
      pthread_cond_wait(&condc, &the_mutex);
    buffer = 0;
    pthread_cond_signal(&condp);
    pthread_mutex_unlock(&the_mutex);
  }
  return NULL;
}


