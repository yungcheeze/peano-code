#include "peano/heap/LockFreeStack.h"
#include "tarch/multicore/Lock.h"

void LockFreeStack::incrementPopCount(){
#ifndef DCAS
  RefCounter expected, new_val;
  expected = _refcount.load();
  expected.pushCount = 0;
  new_val = expected;
  ++new_val.popCount;
  while (!_refcount.compare_exchange_weak(expected, new_val)) {
    expected.pushCount = 0;
    new_val.popCount = expected.popCount + 1;
  }
#endif
}

void LockFreeStack::decrementPopCount(){
#ifndef DCAS
  RefCounter expected, new_val;
  expected = _refcount.load();
  new_val = expected;
  --new_val.popCount;
  while (!_refcount.compare_exchange_weak(expected, new_val)) {
    new_val.popCount = expected.popCount - 1;
  }
#endif
}

void LockFreeStack::incrementPushCount(){
#ifndef DCAS
  RefCounter expected, new_val;
  expected = _refcount.load();
  expected.popCount = 0;
  new_val = expected;
  ++new_val.pushCount;
  while (!_refcount.compare_exchange_weak(expected, new_val)) {
    expected.popCount = 0;
    new_val.pushCount = expected.pushCount + 1;
  }
#endif
}
void LockFreeStack::decrementPushCount(){
#ifndef DCAS
  RefCounter expected, new_val;
  expected = _refcount.load();
  new_val = expected;
  --new_val.pushCount;
  while (!_refcount.compare_exchange_weak(expected, new_val)) {
    new_val.pushCount = expected.pushCount - 1;
  }
#endif
}

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
  head.a_top.exchange(tail);
}

void LockFreeStack::free() {
  while (!empty()) {
    pop();
  }
  delete tail;
}

void LockFreeStack::push(int key) {
  incrementPushCount();
  Node* curr_top = head.a_top.load();
  Node* new_top = new Node(key);
  new_top->next = curr_top;
  while(!head.a_top.compare_exchange_weak(curr_top, new_top)){
    new_top->next = curr_top;
  }
  decrementPushCount();
}


int LockFreeStack::pop() {
  incrementPopCount();
  Node* curr_top = head.a_top.load();
  if(curr_top == tail) {decrementPopCount(); return -1;}
  Node* new_top = curr_top;

  tarch::multicore::Lock lock(_deleteSemaphore);
  if(curr_top != nullptr)
    new_top = curr_top->next;
  lock.free();

  while(!head.a_top.compare_exchange_weak(curr_top, new_top)){
    if(curr_top == tail) {decrementPopCount(); return -1;}

    lock.lock();
    if(curr_top != nullptr)
      new_top = curr_top->next;
    lock.free();

  }
  int result = curr_top->_data;

  lock.lock();
  delete curr_top;
  lock.free();

  decrementPopCount();
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
