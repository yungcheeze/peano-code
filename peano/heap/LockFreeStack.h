#ifndef LOCKFREESTACK_H
#define LOCKFREESTACK_H

#include <atomic>

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
  
  void incrementPopCount();
  void decrementPopCount();
  void incrementPushCount();
  void decrementPushCount();

  void node_push(HeadNode &hn, Node *new_top);
  Node* node_pop(HeadNode &hn);

public:
  HeadNode head;
  HeadNode rhead;
  Node *tail;

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
