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

void LockFreeStack::init() {
  tail = new Node();
  head.a_top.exchange(tail);
  rhead.a_top.exchange(tail);
}

void LockFreeStack::free() {
  while (!empty()) {
    pop();
  }
  delete tail;
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

void LockFreeStack::node_push(HeadNode &hn, Node *new_top) {
  Node *curr_top = hn.a_top.load();
  new_top->next = curr_top;
  while(!hn.a_top.compare_exchange_weak(curr_top, new_top)){
    new_top->next = curr_top;
  }
}

Node* LockFreeStack::node_pop(HeadNode &hn) {
  Node *curr_top = hn.a_top.load();
  if(curr_top == tail) return 0;
  Node *new_top = curr_top;
  new_top = curr_top->next;
  while(!hn.a_top.compare_exchange_weak(curr_top, new_top)){
    if(curr_top == tail) return 0;
    new_top = curr_top->next;
  }
  return curr_top;
}

void LockFreeStack::push(int key) {
  incrementPushCount();
  Node* new_top = node_pop(rhead);
  if (new_top == 0)
    new_top = new Node(key);
  else
    new_top->_data = key;
  node_push(head, new_top);
  decrementPushCount();
}


int LockFreeStack::pop() {
  int result;
  incrementPopCount();

  Node* old_top = node_pop(head);
  if (old_top != 0) {
    result = old_top->_data;
    old_top->_data = -1;
    node_push(rhead, old_top);
  } else {
    result = -1;
  }

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
