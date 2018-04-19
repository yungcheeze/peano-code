#include "peano/heap/LockFreeStack.h"
#include "tarch/multicore/Lock.h"

LockFreeStack::LockFreeStack(): _refcount(RefCounter {0, 0}) {
  init();
}

LockFreeStack::LockFreeStack(const LockFreeStack& other): _refcount(RefCounter {0, 0}) {
  free();
  init();
  //TODO facilitate proper copying
}

LockFreeStack& LockFreeStack::operator=(const LockFreeStack &rhs)
{
  // Check for self-assignment!
  if (this == &rhs)
    return *this;

  _refcount.exchange(rhs._refcount);
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
  head.next = tail;
}

void LockFreeStack::free() {
  while (!empty()) {
    pop();
  }
  delete tail;
}

void LockFreeStack::push(int key) {
  tarch::multicore::Lock lock(_deleteSemaphore);
  Node* new_top = new Node(key);
  new_top->next = head.next;
  head.next = new_top;
  lock.free();
}


int LockFreeStack::pop() {
  tarch::multicore::Lock lock(_deleteSemaphore);
  if(head.next == tail) { return -1;}
  Node* old_top = head.next;
  head.next = old_top->next;
  int result = old_top->_data;
  delete old_top;
  lock.free();
  return result;
}

LockFreeStack::size_type LockFreeStack::size() const {
  if(head.next == tail)
    return 0;
  else
    return 1;
}

bool LockFreeStack::empty() const {
  if(head.next == tail)
    return true;
  else
    return false;
}
