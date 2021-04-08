#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

#define NUM_THREADS 3 // test with 1,2,3,4 threads
uthread_t threads[NUM_THREADS];
uthread_mutex_t mx;
uthread_cond_t release;
int waiting = 0;

void randomStall() {
  int i, r = random() >> 16;
  while (i++<r);
}

void waitForAllOtherThreads() {

  uthread_mutex_lock(mx);
  waiting++;

  if (waiting == NUM_THREADS) {
    // if everyone is waiting, tell them theyre good to go
    waiting = 0;
    uthread_cond_broadcast(release);
  } else {
    // wait for others (releases the mutex before blocking)
    uthread_cond_wait(release);
  }
  uthread_mutex_unlock(mx);

}

void* p(void* v) {
  randomStall();

  printf("a\n");

  waitForAllOtherThreads();

  printf("b\n");

  return NULL;
}

int main(int arg, char** arv) {

  uthread_init(4);
  mx = uthread_mutex_create();
  release = uthread_cond_create(mx);

  // create da threads
  for (int i=0; i<NUM_THREADS; i++)
    threads[i] = uthread_create(p, NULL);

  // wait till all done
  for (int i=0; i<NUM_THREADS; i++)
    uthread_join (threads[i], NULL);

  printf("------\n");

}