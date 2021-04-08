#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <assert.h>
#include "uthread.h"
#include "queue.h"
#include "disk.h"

queue_t pending_read_queue;
volatile int pending_reads;
volatile int is_read_pending;

void interrupt_service_routine() {
  // TODO

  void* val;
  void (*callback)(void*, void*);
  queue_dequeue(pending_read_queue, &val, NULL, &callback);
  callback(val, NULL);

  is_read_pending = 0;
}

void handleOtherReads(void *resultv, void *countv) {
  // TODO

  int val = *((int*) resultv);
  // printf("next=%d\n", val);
  pending_reads = pending_reads - 1;
}

void handleFirstRead(void *resultv, void *countv) {
  // TODO

  int val = *((int*) resultv);
  pending_reads = val;
  // printf("first=%d\n", val);
  
  int result[val];
  int runs = val;

  for (int i = 0; i < runs; i++) {
    is_read_pending = 1;
    queue_enqueue(pending_read_queue, &result[i], NULL, handleOtherReads);
    disk_schedule_read(&result[i], val);
    while (is_read_pending);
    val = result[i];
  }

  printf("%d", val);

}

int main(int argc, char **argv) {
  // Command Line Arguments
  static char* usage = "usage: treasureHunt starting_block_number";
  int starting_block_number;
  char *endptr;
  if (argc == 2)
    starting_block_number = strtol (argv [1], &endptr, 10);
  if (argc != 2 || *endptr != 0) {
    printf ("argument error - %s \n", usage);
    return EXIT_FAILURE;
  }

  // Initialize
  uthread_init (1);
  disk_start (interrupt_service_routine);
  pending_read_queue = queue_create();
  pending_reads = 1;

  // Start the Hunt
  // TODO

  int result0;
  queue_enqueue(pending_read_queue, &result0, NULL, handleFirstRead);
  disk_schedule_read(&result0, starting_block_number);

  while (pending_reads > 0);
}