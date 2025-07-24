#ifndef _QUEUE_H
#define _QUEUE_H

#include <pthread.h>
#include <stdbool.h>

struct Task {
  void (*function)(void*);
  void* args;
};

struct Node {
  struct Task* task;
  struct Node* next;
};

struct Node* node_new(struct Task* t);
void node_free(struct Node* n);

struct Queue {
  struct Node* head;
  struct Node* tail;
  int max_size;
  int current_size;
  pthread_cond_t empty;
  pthread_cond_t full;
  pthread_mutex_t mutex;
};

struct Queue* queue_new(int max_size);
void queue_enqueue(struct Queue* q, struct Task* t);
struct Task* queue_dequeue(struct Queue* q);
void queue_free(struct Queue* q);

#endif