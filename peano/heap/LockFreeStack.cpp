#include "peano/heap/LockFreeStack.h"

LockFreeStack::LockFreeStack() {
  tail = new Node();
  head.a_top.exchange(AbaPtr {tail, 0});
}

LockFreeStack::~LockFreeStack() {
  delete tail;
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
  return result;
}
