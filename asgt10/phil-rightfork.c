#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

#define MAX_THINKING_TIME 25000

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__)
#else
#define VERBOSE_PRINT(S, ...) ((void) 0) // do nothing
#endif

typedef struct fork {
  uthread_mutex_t lock;
  uthread_cond_t forfree;
  long free;
} fork_t;

int num_phils, num_meals;    
fork_t *forks;


void deep_thoughts() {
  usleep(random() % MAX_THINKING_TIME);
}

void initfork(int i) {
  forks[i].lock    = uthread_mutex_create();
  forks[i].forfree = uthread_cond_create(forks[i].lock);
  forks[i].free    = 1;
}

int leftfork(long i) {
  return i;
}

int rightfork(long i) {
  return (i + 1) % num_phils;
}

/* Given a philosopher's id, gets the right fork if it's free. If not free, waits until
   a signal that it is. */
void get_right_fork(long id) {

    /* Get ID of right fork */
    long i = rightfork(id);

    uthread_mutex_lock(forks[i].lock);

    /* If the fork is free, get it */
    if (forks[i].free) {
        VERBOSE_PRINT("- P%d getting fork %d\n", id, i);
        forks[i].free = 0;

    /* If the fork is not free, wait until someone signals that it is, then get it. */
    } else {
        VERBOSE_PRINT(". P%d waiting for fork %d\n", id, i);
        uthread_cond_wait(forks[i].forfree);
        VERBOSE_PRINT("- P%d getting fork %d\n", id, i);
        forks[i].free = 0;
    }

    uthread_mutex_unlock(forks[i].lock);
}

/* Same logic as get_right_fork. The two are separate for print statement clarity. */
void get_left_fork(long id) {
    long i = leftfork(id);
    uthread_mutex_lock(forks[i].lock);
    if (forks[i].free) {
        VERBOSE_PRINT("- P%d getting fork %d\n", id, i);
        forks[i].free = 0;
    } else {
        VERBOSE_PRINT(". P%d waiting for fork %d\n", id, i);
        uthread_cond_wait(forks[i].forfree);
        VERBOSE_PRINT("- P%d getting fork %d\n", id, i);
        forks[i].free = 0;
    }
    uthread_mutex_unlock(forks[i].lock);
}

/* Given philosopher ID, puts down the fork to their right and signals that it's free. */
void put_right_fork(long id) {

    /* Get ID of right fork */
    long i = rightfork(id);

    uthread_mutex_lock(forks[i].lock);

    VERBOSE_PRINT("- P%d putting down fork %d\n", id, i);
    
    /* Free up fork and signal that it is free */
    forks[i].free = 1;
    uthread_cond_signal(forks[i].forfree);

    uthread_mutex_unlock(forks[i].lock);
}

/* Same logic as put_right_fork. The two are separate for print statement clarity. */
void put_left_fork(long id) {
    long i = leftfork(id);
    uthread_mutex_lock(forks[i].lock);
    VERBOSE_PRINT("- P%d putting down fork %d\n", id, i);
    forks[i].free = 1;
    uthread_cond_signal(forks[i].forfree);
    uthread_mutex_unlock(forks[i].lock);
}

/* Doesn't actually do anything, other than announce that a philosopher is eating. */
void eat_spaghetti(long id) {

    /* Colored text for philosophers 0 and 1 */
    switch (id) {
        case 0:
        VERBOSE_PRINT("\033[0;31m* P%d is eating\n\n", id);
        break;

        case 1:
        VERBOSE_PRINT("\033[0;32m* P%d is eating\n\n", id);
        break;

        default:
        VERBOSE_PRINT("* P%d is eating\n\n", id);
        break;
    }

    /* Set color back to white */
    VERBOSE_PRINT("\033[0;37m");
}

/* A philosopher with a unique ID, who repeatedly thinks and eats until the number of
   meals to be eaten is met. Philosophers need two forks to eat. */
void* phil_thread(void* arg) {

    uintptr_t id = (uintptr_t) arg;
    int meals = 0;
    
    while (meals < num_meals) {
        deep_thoughts();

        get_right_fork(id);         // get right fork
        deep_thoughts();

        get_left_fork(id);          // get left fork
        deep_thoughts();

        eat_spaghetti(id);          // eat
        deep_thoughts();

        put_right_fork(id);         // place right fork
        deep_thoughts();

        put_left_fork(id);          // place left fork
        deep_thoughts();

        meals++;
    }
    return NULL;
}

int main(int argc, char **argv) {

  uthread_t *p;
  uintptr_t i;
  
  if (argc != 3) {
    fprintf(stderr, "Usage: %s num_philosophers num_meals\n", argv[0]);
    return 1;
  }

  num_phils = strtol(argv[1], 0, 0);
  num_meals = strtol(argv[2], 0, 0);
  
  forks = malloc(num_phils * sizeof(fork_t));
  p = malloc(num_phils * sizeof(pthread_t));

  uthread_init(num_phils);
  
  srandom(time(0));
  for (i = 0; i < num_phils; ++i) {
    initfork(i);
  }

  /* TODO: Create num_phils threads, all calling phil_thread with a
   * different ID, and join all threads.
   */

  uthread_t philosopher_thread[num_phils];

  for (i = 0; i < num_phils; ++i) {
      philosopher_thread[i] = uthread_create(phil_thread, (void*) i);
  }

  for (i = 0; i < num_phils; ++i) {
      uthread_join(philosopher_thread[i], NULL);
  }

  return 0;
}
