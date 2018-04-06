#include "peano/heap/LockFreeStack.h"
#include "tarch/multicore/Lock.h"

LockFreeStack::LockFreeStack() {
  init();
}

LockFreeStack::~LockFreeStack() {
  free();
}

void LockFreeStack::init() {
  tail = new Node();
  head.a_top.exchange(AbaPtr {tail, 0});
}

void LockFreeStack::free() {
  Node* top = head.a_top.load().ptr;
  Node* next = top->next;
  delete top;
  while(next != nullptr){
    top = next;
    next = top->next;
    delete top;
  }
}

void LockFreeStack::push(int key) {
  AbaPtr curr_top = head.a_top.load();
  AbaPtr new_top = curr_top;
  new_top.ptr = new Node(key);
  new_top.ptr->next = curr_top.ptr;
  new_top.aba += 1;
  while(!head.a_top.compare_exchange_weak(curr_top, new_top)){
    new_top.ptr->next = curr_top.ptr;
    new_top.aba = curr_top.aba + 1;
  }
}


int LockFreeStack::pop() {
  tarch::multicore::Lock lock(_deleteSemaphore);
  AbaPtr curr_top = head.a_top.load();
  if(curr_top.ptr == tail) return -1;
  AbaPtr new_top = curr_top;
  new_top.ptr = curr_top.ptr->next;
  new_top.aba += 1;
  while(!head.a_top.compare_exchange_weak(curr_top, new_top)){
    if(curr_top.ptr == tail) return -1;
    new_top.ptr = curr_top.ptr->next;
    new_top.aba = curr_top.aba + 1;
  }
  int result = curr_top.ptr->_data;
  delete curr_top.ptr;
  lock.free();
  return result;
}

LockFreeStack::size_type LockFreeStack::size() {
  if(head.a_top.load().ptr->next == tail)
    return 0;
  else
    return 1;
}

bool LockFreeStack::empty() {
  if(head.a_top.load().ptr->next == tail)
    return true;
  else
    return false;
}
