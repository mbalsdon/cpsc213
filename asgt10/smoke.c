#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "uthread.h"
#include "uthread_mutex_cond.h"

#define NUM_ITERATIONS 1000

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__)
#else
#define VERBOSE_PRINT(S, ...) ((void) 0) // do nothing
#endif

struct Agent {
  uthread_mutex_t mutex;
  uthread_cond_t  match;
  uthread_cond_t  paper;
  uthread_cond_t  tobacco;
  uthread_cond_t  smoke;
};

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Resources have unique values; code sums these values to indicate which two
   resources are available. E.g. match + tobacco = 1 + 4 = 5 */
int code = 0;
uthread_cond_t match_paper;
uthread_cond_t match_tobacco;
uthread_cond_t paper_tobacco;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

struct Agent* createAgent() {
  struct Agent* agent = malloc (sizeof (struct Agent));
  agent->mutex   = uthread_mutex_create();
  agent->paper   = uthread_cond_create(agent->mutex);
  agent->match   = uthread_cond_create(agent->mutex);
  agent->tobacco = uthread_cond_create(agent->mutex);
  agent->smoke   = uthread_cond_create(agent->mutex);
  return agent;
}

/**
 * You might find these declarations helpful.
 *   Note that Resource enum had values 1, 2 and 4 so you can combine resources;
 *   e.g., having a MATCH and PAPER is the value MATCH | PAPER == 1 | 2 == 3
 */
enum Resource            {    MATCH = 1, PAPER = 2,   TOBACCO = 4};
char* resource_name [] = {"", "match",   "paper", "", "tobacco"};

// # of threads waiting for a signal. Used to ensure that the agent
// only signals once all other threads are ready.
int num_active_threads = 0;

int signal_count [5];  // # of times resource signalled
int smoke_count  [5];  // # of times smoker with resource smoked

/**
 * This is the agent procedure.  It is complete and you shouldn't change it in
 * any material way.  You can modify it if you like, but be sure that all it does
 * is choose 2 random resources, signal their condition variables, and then wait
 * wait for a smoker to smoke.
 */
void* agent (void* av) {
  struct Agent* a = av;
  static const int choices[]         = {MATCH|PAPER, MATCH|TOBACCO, PAPER|TOBACCO};
  static const int matching_smoker[] = {TOBACCO,     PAPER,         MATCH};

  srandom(time(NULL));
  
  uthread_mutex_lock (a->mutex);
  // Wait until all other threads are waiting for a signal
  // while (num_active_threads < 3) {
  //   uthread_cond_wait (a->smoke);
  // }

  for (int i = 0; i < NUM_ITERATIONS; i++) {
    int r = random() % 6;
    switch(r) {
    case 0:
      signal_count[TOBACCO]++;
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      break;
    case 1:
      signal_count[PAPER]++;
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      break;
    case 2:
      signal_count[MATCH]++;
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      break;
    case 3:
      signal_count[TOBACCO]++;
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      break;
    case 4:
      signal_count[PAPER]++;
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      break;
    case 5:
      signal_count[MATCH]++;
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      break;
    }
    VERBOSE_PRINT ("agent is waiting for smoker to smoke\n");
    uthread_cond_wait (a->smoke);
  }
  
  uthread_mutex_unlock (a->mutex);
  return NULL;
}


/** - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * TODO
 * You will probably need to add some procedures and struct etc.
 * 
 */

/* Based on the input code, wakes up the respective smoker (and resets the code)
   MATCH = 1    PAPER = 2   TOBACCO = 4 */
void notify_smokers(int c) {

  VERBOSE_PRINT("....Attempting to signal with code = %d\n", c);

  if (c == 3) {
    VERBOSE_PRINT("...Signalling TOBACCO\n");
    uthread_cond_signal(match_paper);   // s_with_tobacco
    code = 0;
  } else if (c == 5) {
    VERBOSE_PRINT("...Signalling PAPER\n");
    uthread_cond_signal(match_tobacco); // s_with_paper
    code = 0;
  } else if (c == 6) {
    VERBOSE_PRINT("...Signalling MATCHES\n");
    uthread_cond_signal(paper_tobacco); // s_with_matches
    code = 0;
  }

}

/* Updates code and signals smokers when the agent dispenses tobacco */
void* tobacco_observer(void* agent) {

  struct Agent* a = agent;

  uthread_mutex_lock(a->mutex);

  /* "Watches" for tobacco signal by looping forever */
  while (1) {
    /* Wait for signal, alert smokers when received */
    uthread_cond_wait(a->tobacco);
    VERBOSE_PRINT("..Observed TOBACCO\n");
    code = code + TOBACCO;
    notify_smokers(code);
  }

  uthread_mutex_unlock(a->mutex);
}

/* Same as tobacco observer */
void* match_observer(void* agent) {
  struct Agent* a = agent;
  uthread_mutex_lock(a->mutex);
  while (1) {
    uthread_cond_wait(a->match);
    VERBOSE_PRINT("..Observed MATCH\n");
    code = code + MATCH;
    notify_smokers(code);
  }
  uthread_mutex_unlock(a->mutex);
}

/* Same as tobacco observer */
void* paper_observer(void* agent) {
  struct Agent* a = agent;
  uthread_mutex_lock(a->mutex);
  while (1) {
    uthread_cond_wait(a->paper);
    VERBOSE_PRINT("..Observed PAPER\n");
    code = code + PAPER;
    notify_smokers(code);
  }
  uthread_mutex_unlock(a->mutex);
}

/* Same logic as observers - smokes when match + paper are available */
void* s_with_tobacco(void* agent) {
  
  struct Agent* a = agent;

  uthread_mutex_lock(a->mutex);

  /* Smokes when match + paper available, loops forever */
  while (1) {
    //num_active_threads++;
    uthread_cond_wait(match_paper);

    VERBOSE_PRINT(".TOBACCO is smoking\n");
    smoke_count[TOBACCO]++;

    uthread_cond_signal(a->smoke);
  }
  
  uthread_mutex_unlock(a->mutex);
}

/* Same as tobacco smoker */
void* s_with_matches(void* agent) {
  struct Agent* a = agent;
  uthread_mutex_lock(a->mutex);
  while (1) {
    //num_active_threads++;
    uthread_cond_wait(paper_tobacco);
    VERBOSE_PRINT(".MATCHES is smoking\n");
    smoke_count[MATCH]++;
    uthread_cond_signal(a->smoke);
  }
  uthread_mutex_unlock(a->mutex);
}

/* Same as tobacco smoker */
void* s_with_paper(void* agent) {
  struct Agent* a = agent;
  uthread_mutex_lock(a->mutex);
  while (1) {
    //num_active_threads++;
    uthread_cond_wait(match_tobacco);
    VERBOSE_PRINT(".PAPER is smoking\n");
    smoke_count[PAPER]++;

    uthread_cond_signal(a->smoke);
  }
  uthread_mutex_unlock(a->mutex);
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


int main (int argc, char** argv) {
  
  uthread_init(7);
  struct Agent* a = createAgent();
  
  // TODO
  match_paper = uthread_cond_create(a->mutex);
  match_tobacco = uthread_cond_create(a->mutex);
  paper_tobacco = uthread_cond_create(a->mutex);

  /* Everything watches the agent since the agent can't send alerts */
  uthread_create(tobacco_observer, a);
  uthread_create(match_observer, a);
  uthread_create(paper_observer, a);

  uthread_create(s_with_tobacco, a);
  uthread_create(s_with_matches, a);
  uthread_create(s_with_paper, a);

  uthread_t agent_thread = uthread_create(agent, a);

  uthread_join(agent_thread, NULL);

  assert (signal_count [MATCH]   == smoke_count [MATCH]);
  assert (signal_count [PAPER]   == smoke_count [PAPER]);
  assert (signal_count [TOBACCO] == smoke_count [TOBACCO]);
  assert (smoke_count [MATCH] + smoke_count [PAPER] + smoke_count [TOBACCO] == NUM_ITERATIONS);

  printf ("Smoke counts: %d matches, %d paper, %d tobacco\n",
          smoke_count [MATCH], smoke_count [PAPER], smoke_count [TOBACCO]);

  return 0;
}
