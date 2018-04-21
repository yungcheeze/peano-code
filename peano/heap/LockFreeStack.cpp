#include "peano/heap/LockFreeStack.h"
#include "tarch/multicore/Lock.h"
tarch::logging::Log LockFreeStack::_log("peano::heap::LockFreeStack");

void LockFreeStack::incrementPopCount(){
#ifndef DCAS
  ++pop_rc_count;
  ++pop_rc_loop;
  RefCounter expected, new_val;
  expected = _refcount.load();
  expected.pushCount = 0;
  new_val = expected;
  ++new_val.popCount;
  while (!_refcount.compare_exchange_weak(expected, new_val)) {
    ++pop_rc_loop;
    expected.pushCount = 0;
    new_val.popCount = expected.popCount + 1;
  }
#endif
}

void LockFreeStack::decrementPopCount(){
#ifndef DCAS
  RefCounter expected, new_val;
  expected = _refcount.load();
  unsigned max_c = max_pop_count.load();
  if (expected.popCount > max_c)
    max_c = max_pop_count.exchange(expected.popCount);
  new_val = expected;
  --new_val.popCount;
  while (!_refcount.compare_exchange_weak(expected, new_val)) {
    new_val.popCount = expected.popCount - 1;
  }
#endif
}

void LockFreeStack::incrementPushCount(){
#ifndef DCAS
  ++push_rc_count;
  ++push_rc_loop;
  RefCounter expected, new_val;
  expected = _refcount.load();
  expected.popCount = 0;
  new_val = expected;
  ++new_val.pushCount;
  while (!_refcount.compare_exchange_weak(expected, new_val)) {
    ++push_rc_loop;
    expected.popCount = 0;
    new_val.pushCount = expected.pushCount + 1;
  }
#endif
}
void LockFreeStack::decrementPushCount(){
#ifndef DCAS
  RefCounter expected, new_val;
  expected = _refcount.load();
  unsigned max_c = max_push_count.load();
  if (expected.pushCount > max_c)
    max_c = max_push_count.exchange(expected.pushCount);
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
  unsigned pc = pop_count.load();
  if(pc != 0){
  logInfo( "~LockFreeStack()", "cas loopbacks: "
           << "push count " << push_count.load() << "; "
           << "push loops " << push_loop.load() << "; "
           << "push loopbacks " << push_loop.load() - push_count.load() << "; "
           << "pop count " << pop_count.load() << "; "
           << "pop loops " << pop_loop.load() << "; "
           << "pop loopbacks " << pop_loop.load() - pop_count.load() << "; ");
  logInfo( "~LockFreeStack()", "rc loopbacks: "
           << "push count " << push_rc_count.load() << "; "
           << "push loops " << push_rc_loop.load() << "; "
           << "push loopbacks " << push_rc_loop.load() - push_rc_count.load() << "; "
           << "pop count " << pop_rc_count.load() << "; "
           << "pop loops " << pop_rc_loop.load() << "; "
           << "pop loopbacks " << pop_rc_loop.load() - pop_rc_count.load() << "; ");
  logInfo( "~LockFreeStack()", "concurrency: "
           << "max_pushes " << max_push_count.load() << "; "
           << "max_pops " << max_pop_count.load() << "; ");
  }
}

void LockFreeStack::init() {
  tail = new Node();
  head.a_top.exchange(tail);
  rhead.a_top.exchange(tail);
  push_count.exchange(0);
  push_loop.exchange(0);
  pop_count.exchange(0);
  pop_loop.exchange(0);
  push_rc_count.exchange(0);
  push_rc_loop.exchange(0);
  pop_rc_count.exchange(0);
  pop_rc_loop.exchange(0);
  max_push_count.exchange(0);
  max_pop_count.exchange(0);
}

void LockFreeStack::free() {
  while (!empty()) {
    node_pop(head);
  }
  while (rhead.a_top.load() != tail) {
    node_pop(rhead);
  }
  delete tail;
}

void LockFreeStack::node_push(HeadNode &hn, Node *new_top) {
  ++push_count;
  ++push_loop;
  Node *curr_top = hn.a_top.load();
  new_top->next = curr_top;
  while(!hn.a_top.compare_exchange_weak(curr_top, new_top)){
    ++push_loop;
    new_top->next = curr_top;
  }
}

Node* LockFreeStack::node_pop(HeadNode &hn) {
  ++pop_count;
  ++pop_loop;
  Node *curr_top = hn.a_top.load();
  if(curr_top == tail) return 0;
  Node *new_top = curr_top;
  new_top = curr_top->next;
  while(!hn.a_top.compare_exchange_weak(curr_top, new_top)){
    ++pop_loop;
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
