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
#ifdef DCAS
  head.a_top.exchange(AbaPtr {tail, 0});
#else
  head.a_top.exchange(tail);
#endif
}

void LockFreeStack::free() {
  while (!empty()) {
    pop();
  }
  delete tail;
}

void LockFreeStack::push(int key) {

#ifdef DCAS
  AbaPtr curr_top = head.a_top.load();
  AbaPtr new_top = curr_top;
  new_top.ptr = new Node(key);
  new_top.ptr->next = curr_top.ptr;
  new_top.aba += 1;
  while(!head.a_top.compare_exchange_weak(curr_top, new_top)){
    new_top.ptr->next = curr_top.ptr;
    new_top.aba = curr_top.aba + 1;
  }
#else
  incrementPushCount();
  Node* curr_top = head.a_top.load();
  Node* new_top = new Node(key);
  new_top->next = curr_top;
  while(!head.a_top.compare_exchange_weak(curr_top, new_top)){
    new_top->next = curr_top;
  }
  decrementPushCount();
#endif
}


int LockFreeStack::pop() {
#ifdef DCAS
  AbaPtr curr_top = head.a_top.load();
  if(curr_top.ptr == tail) return -1;
  AbaPtr new_top = curr_top;
  tarch::multicore::Lock lock(_deleteSemaphore);
  if(curr_top.ptr != nullptr)
    new_top.ptr = curr_top.ptr->next;
  lock.free();
  new_top.aba += 1;
  while(!head.a_top.compare_exchange_weak(curr_top, new_top)){
    if(curr_top.ptr == tail) return -1;
    lock.lock();
    if(curr_top.ptr != nullptr)
      new_top.ptr = curr_top.ptr->next;
    lock.free();
    new_top.aba = curr_top.aba + 1;
  }
  int result = curr_top.ptr->_data;
  lock.lock();
  delete curr_top.ptr;
  lock.free();
  return result;
#else
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
#endif
}

LockFreeStack::size_type LockFreeStack::size() const {
#ifdef DCAS
  if(head.a_top.load().ptr == tail)
    return 0;
  else
    return 1;
#else
  if(head.a_top.load() == tail)
    return 0;
  else
    return 1;
#endif
}

bool LockFreeStack::empty() const {
#ifdef DCAS
  if(head.a_top.load().ptr == tail)
    return true;
  else
    return false;
#else
  if(head.a_top.load() == tail)
    return true;
  else
    return false;
#endif
}
