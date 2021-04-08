#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"
#include "uthread_mutex_cond.h"

uthread_t t0, t1, t2;
uthread_mutex_t mx;
uthread_cond_t c0, c1, c2;

int zero_has_gone = 0;
int one_has_gone = 0; // prevents deadlocks where threads wait before other threads signal
int two_has_gone = 0;

void randomStall() {
  int i, r = random() >> 16;
  while (i++<r);
}

void* p0(void* v) {
  randomStall();

  uthread_mutex_lock(mx);

  printf("zero\n");
  zero_has_gone = 1;
  uthread_cond_signal(c0); // signal that p0 has gone
  
  uthread_mutex_unlock(mx);

  return NULL;
}

void* p1(void* v) {
  randomStall();

  uthread_mutex_lock(mx);

  if (!zero_has_gone) uthread_cond_wait(c0); // wait till p0 has gone
  printf("one\n");
  one_has_gone = 1;
  uthread_cond_signal(c1); // signal that p1 has gone

  uthread_mutex_unlock(mx);

  return NULL;
}

void* p2(void* v) {
  randomStall();

  uthread_mutex_lock(mx);

  if (!one_has_gone) uthread_cond_wait(c1); // wait till p1 has gone
  printf("two\n");
  two_has_gone = 1;
  uthread_cond_signal(c2); // signal that p2 has gone

  uthread_mutex_unlock(mx);

  return NULL;
}

int main(int arg, char** arv) {

  uthread_init(4);
  mx = uthread_mutex_create();
  c0 = uthread_cond_create(mx);
  c1 = uthread_cond_create(mx);
  c2 = uthread_cond_create(mx);

  t0 = uthread_create(p0, NULL);
  t1 = uthread_create(p1, NULL);
  t2 = uthread_create(p2, NULL);

  randomStall();

  uthread_join (t0, NULL);
  uthread_join (t1, NULL);
  uthread_join (t2, NULL);


  printf("three\n");
  printf("------\n");
}