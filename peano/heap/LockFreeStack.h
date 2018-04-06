#ifndef LOCKFREESTACK_H
#define LOCKFREESTACK_H

#include <atomic>
#include "tarch/multicore/BooleanSemaphore.h"
#include "tbb/concurrent_queue.h"

class Node
{
public:
  int _data;
  bool mark;
  Node* next;
  Node() : _data(0), mark(false) {}
  Node(int data): _data(data), mark(false) {}
};

struct AbaPtr { Node* ptr; unsigned short aba; };

class HeadNode {
public:
  int _data;
  std::atomic<AbaPtr> a_top;
  HeadNode() : _data(0) {}
};

class LockFreeStack
{
private:
  void init();
  void free();
  
public:
  HeadNode head;
  Node *tail;
  tarch::multicore::BooleanSemaphore _deleteSemaphore;

  typedef unsigned size_type;

  LockFreeStack();
  LockFreeStack(const LockFreeStack& other) = delete;
  LockFreeStack& operator=(const LockFreeStack& rhs) = delete;
  ~LockFreeStack();

  void push(int key);
  int pop();
  size_type size();
  bool empty();

};

#endif /* LOCKFREESTACK_H */
