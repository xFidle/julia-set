#include "tpool.h"
#include "queue.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#define TERMINATION_TASK NULL

struct TPool* tpool_new(struct TPoolConfig* cfg) {
  struct TPool* tp = malloc(sizeof(struct TPool));
  if (tp == NULL) { return NULL; }
  tp->n_threads = cfg->n_threads;
  tp->task_queue = queue_new(cfg->queue_size);
  tp->threads = malloc(sizeof(pthread_t) * cfg->n_threads);
  if (tp->task_queue == NULL || tp->threads == NULL) {
    tpool_free(tp);
    return NULL;
  }
  pthread_mutex_init(&tp->mutex, NULL);
  pthread_cond_init(&tp->cond, NULL);
  for (int i = 0; i < tp->n_threads; ++i) { pthread_create(&tp->threads[i], NULL, &tpool_start_thread, tp); }
  return tp;
}

void* tpool_start_thread(void* args) {
  struct TPool* tp = args;
  while (1) {
    struct Task* t = queue_dequeue(tp->task_queue);
    if (t == TERMINATION_TASK) break;
    t->function(t->args);
    pthread_mutex_lock(&tp->mutex);
    t->is_finished = true;
    pthread_cond_signal(&tp->cond);
    pthread_mutex_unlock(&tp->mutex);
  }
  return NULL;
}

void tpool_free(struct TPool* tp) {
  if (tp->task_queue) {
    if (tp->threads) {
      for (int i = 0; i < tp->n_threads; ++i) { queue_enqueue(tp->task_queue, TERMINATION_TASK); }
      for (int i = 0; i < tp->n_threads; ++i) { pthread_join(tp->threads[i], NULL); }
      free(tp->threads);
    }
    queue_free(tp->task_queue);
  }
  pthread_mutex_destroy(&tp->mutex);
  pthread_cond_destroy(&tp->cond);
  free(tp);
}
