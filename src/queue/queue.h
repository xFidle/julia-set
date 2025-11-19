#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdbool.h>

struct Task {
  void* (*function)(void*);
  void* args;
  volatile bool is_finished;
};

struct Node {
  struct Task* task;
  struct Node* next;
};

struct Node* node_new(struct Task* t);

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
