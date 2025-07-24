#include "queue.h"
#include <pthread.h>
#include <stdlib.h>

struct Node* node_new(struct Task* t) {
  struct Node* node = malloc(sizeof(struct Node));
  node->task = t;
  node->next = NULL;
  return node;
}

void node_free(struct Node* n) {
  n->next = NULL;
  free(n);
}

struct Queue* queue_new(int max_size) {
  struct Queue* q = malloc(sizeof(struct Queue));
  q->max_size = max_size;
  q->current_size = 0;
  pthread_cond_init(&q->empty, NULL);
  pthread_cond_init(&q->full, NULL);
  pthread_mutex_init(&q->mutex, NULL);
  return q;
}

void queue_enqueue(struct Queue* q, struct Task* t) {
  pthread_mutex_lock(&(q->mutex));
  while (q->current_size == q->max_size) { pthread_cond_wait(&(q->full), &(q->mutex)); }
  struct Node* node = node_new(t);
  if (q->current_size == 0) {
    q->head = q->tail = node;
    q->current_size++;
  } else {
    q->tail->next = node;
    q->tail = node;
    q->current_size++;
  }
  pthread_mutex_unlock(&(q->mutex));
  pthread_cond_broadcast(&(q->empty));
}

struct Task* queue_dequeue(struct Queue* q) {
  pthread_mutex_lock(&(q->mutex));
  while (q->current_size == 0) { pthread_cond_wait(&(q->empty), &(q->mutex)); }
  struct Node* tmp = q->head;
  struct Task* task = tmp->task;
  q->head = q->head->next;
  if (q->head == NULL) { q->tail = NULL; }
  q->current_size--;
  node_free(tmp);
  pthread_mutex_unlock(&(q->mutex));
  pthread_cond_broadcast(&(q->full));
  return task;
}

void queue_free(struct Queue* q) {
  while (q->current_size != 0) { queue_dequeue(q); }
  free(q);
}
