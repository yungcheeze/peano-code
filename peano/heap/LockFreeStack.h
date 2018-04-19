#ifndef LOCKFREESTACK_H
#define LOCKFREESTACK_H

#include <atomic>
#include "tarch/multicore/BooleanSemaphore.h"

class Node
{
public:
  int _data;
  bool mark;
  Node* next;
  Node() : _data(0), mark(false) {}
  Node(int data): _data(data), mark(false) {}
};


class HeadNode {
public:
  int _data;
  std::atomic<Node*> a_top;
  HeadNode() : _data(0) {}
};

class LockFreeStack
{
private:
  void init();
  void free();
  
  struct RefCounter { unsigned pushCount; int popCount; };
  std::atomic<RefCounter> _refcount;

public:
  Node head;
  Node *tail;
  tarch::multicore::BooleanSemaphore _deleteSemaphore;

  typedef unsigned size_type;

  LockFreeStack();
  LockFreeStack(const LockFreeStack& other);
  LockFreeStack& operator=(const LockFreeStack& rhs);
  ~LockFreeStack();

  void push(int key);
  int pop();
  size_type size() const;
  bool empty() const;

};

#endif /* LOCKFREESTACK_H */
