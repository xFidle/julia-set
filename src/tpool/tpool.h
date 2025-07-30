#ifndef _TPOOL_H
#define _TPOOL_H

#include "queue.h"
#include <pthread.h>

struct TPool {
  struct Queue* task_queue;
  pthread_t* threads;
  int n_threads;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
};

struct TPoolConfig {
  int n_threads;
  int queue_size;
};

struct TPool* tpool_new(struct TPoolConfig* cfg);
void* tpool_start_thread(void* args);
void tpool_free(struct TPool* tp);

#endif