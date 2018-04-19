#ifndef LOCKFREESTACK_H
#define LOCKFREESTACK_H

#include <atomic>
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/logging/Log.h"
#include "tarch/logging/CommandLineLogger.h"

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
  
  struct RefCounter { unsigned pushCount; unsigned popCount; };
  std::atomic<RefCounter> _refcount;
  
  void incrementPopCount();
  void decrementPopCount();
  void incrementPushCount();
  void decrementPushCount();

  void node_push(HeadNode &hn, Node *new_top);
  Node* node_pop(HeadNode &hn);

  static tarch::logging::Log _log;
  std::atomic<unsigned> pp_count;
  std::atomic<unsigned> pp_loop;
  std::atomic<unsigned> rc_count;
  std::atomic<unsigned> rc_loop;
  std::atomic<unsigned> max_push_count;
  std::atomic<unsigned> max_pop_count;

public:
  HeadNode head;
  HeadNode rhead;
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
