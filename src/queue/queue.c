#include "queue.h"

#include <pthread.h>
#include <stdlib.h>

struct Node* node_new(struct Task* t) {
  struct Node* node = malloc(sizeof(struct Node));
  if (node == NULL) { return NULL; }
  node->task = t;
  node->next = NULL;
  return node;
}


struct Queue* queue_new(int max_size) {
  struct Queue* q = malloc(sizeof(struct Queue));
  if (q == NULL) { return NULL; }
  q->max_size = max_size;
  q->current_size = 0;
  pthread_cond_init(&q->empty, NULL);
  pthread_cond_init(&q->full, NULL);
  pthread_mutex_init(&q->mutex, NULL);
  return q;
}

void queue_enqueue(struct Queue* q, struct Task* t) {
  pthread_mutex_lock(&q->mutex);
  while (q->current_size == q->max_size) { pthread_cond_wait(&q->full, &q->mutex); }
  struct Node* node = node_new(t);
  if (node != NULL) {
    if (q->current_size == 0) {
      q->head = q->tail = node;
      q->current_size++;
    } else {
      q->tail->next = node;
      q->tail = node;
      q->current_size++;
    }
    pthread_cond_broadcast(&q->empty);
  }
  pthread_mutex_unlock(&q->mutex);
}

struct Task* queue_dequeue(struct Queue* q) {
  pthread_mutex_lock(&q->mutex);
  while (q->current_size == 0) { pthread_cond_wait(&q->empty, &q->mutex); }
  struct Node* tmp = q->head;
  struct Task* task = tmp->task;
  q->head = q->head->next;
  if (q->head == NULL) { q->tail = NULL; }
  q->current_size--;
  free(tmp);
  pthread_cond_broadcast(&q->full);
  pthread_mutex_unlock(&q->mutex);
  return task;
}

void queue_free(struct Queue* q) {
  while (q->current_size != 0) { queue_dequeue(q); }
  pthread_cond_destroy(&q->empty);
  pthread_cond_destroy(&q->full);
  pthread_mutex_destroy(&q->mutex);
  free(q);
}
