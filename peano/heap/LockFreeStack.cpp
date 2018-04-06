#include "peano/heap/LockFreeStack.h"
#include "tarch/multicore/Lock.h"

LockFreeStack::LockFreeStack() {
  init();
}

LockFreeStack::LockFreeStack(const LockFreeStack& other) {
  free();
  init();
  //TODO facilitate proper copying
}

LockFreeStack& LockFreeStack::operator=(const LockFreeStack &rhs)
{
  // Check for self-assignment!
  if (this == &rhs)
    return *this;

  free();
  init();
  //TODO facilitate proper copying
  return *this;
}
LockFreeStack::~LockFreeStack() {
  free();
}

void LockFreeStack::init() {
  tail = new Node();
  head.a_top.exchange(tail);
}

void LockFreeStack::free() {
  while (!empty()) {
    int out = pop();
  }
  head.a_top.exchange(0);
  delete tail;
}

void LockFreeStack::push(int key) {
  Node* curr_top = head.a_top.load();
  Node* new_top = new Node(key);
  new_top->next = curr_top;
  while(!head.a_top.compare_exchange_weak(curr_top, new_top)){
    new_top->next = curr_top;
  }
}


int LockFreeStack::pop() {
  tarch::multicore::Lock lock(_deleteSemaphore);
  Node* curr_top = head.a_top.load();
  if(curr_top == tail) return -1;
  Node* new_top = curr_top;
  new_top = curr_top->next;
  while(!head.a_top.compare_exchange_weak(curr_top, new_top)){
    if(curr_top == tail) return -1;
    new_top = curr_top->next;
  }
  int result = curr_top->_data;
  delete curr_top;
  lock.free();
  return result;
}

LockFreeStack::size_type LockFreeStack::size() const {
  if(head.a_top.load() == tail)
    return 0;
  else
    return 1;
}

bool LockFreeStack::empty() const {
  if(head.a_top.load() == tail)
    return true;
  else
    return false;
}
