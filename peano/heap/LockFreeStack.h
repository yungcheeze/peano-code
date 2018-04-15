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

struct AbaPtr { Node* ptr; unsigned short aba; };

class HeadNode {
public:
  int _data;
  #ifdef DCAS
  std::atomic<AbaPtr> a_top;
  #else
  std::atomic<Node*> a_top;
  #endif
  HeadNode() : _data(0) {}
};

class LockFreeStack
{
private:
  void init();
  void free();
  
  struct RefCounter { unsigned pushCount; int popCount; };
  std::atomic<RefCounter> _refcount;
  
  void incrementPopCount();
  void decrementPopCount();
  void incrementPushCount();
  void decrementPushCount();

public:
  HeadNode head;
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
