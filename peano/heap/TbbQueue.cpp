#include "peano/heap/TbbQueue.h"

void TbbQueue::push(int key) {
  _data.push(key);
  ++_size;
}

int TbbQueue::pop() {
  int result;
  if (_data.try_pop(result)) {
    --_size;
  } else {
    result = -1;
  }
  return result;
}

TbbQueue::size_type TbbQueue::size() {
  return _size.load();
}

bool TbbQueue::empty() {
  return _data.empty();
}
