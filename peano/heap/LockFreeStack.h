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

struct AbaPtr { Node* ptr; unsigned short aba; };

class HeadNode {
public:
  int _data;
  std::atomic<AbaPtr> a_top;
  HeadNode() : _data(0) {}
};

class LockFreeStack
{
public:
  HeadNode head;
  Node *tail;

  LockFreeStack();
  ~LockFreeStack();
  void push(int key);
  int pop();

};
