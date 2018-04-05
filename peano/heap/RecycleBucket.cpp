#include "peano/heap/RecycleBucket.h"
#include "tarch/multicore/Lock.h"

peano::heap::RecycleBucket::RecycleBucket(): _data(),
                                             _dummy(),
                                             _lookupCounter(),
                                             _dataMapSemaphore()
{
  _dummy[-1] = recycleList(); // index to lookup on failure
  _dummy[-1].push_back(-1); // value to return on failure
  RefCounter tempCounter = RefCounter();
  tempCounter.lockAcquired = false;
  tempCounter.count = 0;
  _lookupCounter.exchange(tempCounter);
}

void peano::heap::RecycleBucket::incrementRefCount() {
  RefCounter expected, new_val;
  expected = _lookupCounter.load();
  expected.lockAcquired = false;
  new_val = expected;
  ++new_val.count;
  while (!_lookupCounter.compare_exchange_weak(expected, new_val)) {
    expected.lockAcquired = false;
    new_val.count = expected.count + 1;
  }
}

void peano::heap::RecycleBucket::decrementRefCount() {
  RefCounter expected, new_val;
  //TODO log warning if lockAcquired = true;
  expected = _lookupCounter.load();
  new_val = expected;
  --new_val.count;
  while (!_lookupCounter.compare_exchange_weak(expected, new_val)) {
    new_val.count = expected.count - 1;
    //should it matter if lock acquired or not?
  }
}

void peano::heap::RecycleBucket::setLockAquiredFlag() {
  RefCounter expected, new_val;
  //TODO log warnings if expected lockAcquired = true;
  expected = _lookupCounter.load();
  expected.count = 0;
  new_val = expected;
  new_val.lockAcquired = true;
  while (!_lookupCounter.compare_exchange_weak(expected, new_val)) {
    expected.count = 0;
    //stronger if set expected to true; lighter without
  }
}

void peano::heap::RecycleBucket::unsetLockAquiredFlag() {
  RefCounter expected, new_val;
  //TODO log warnings if expected lockAcquired = false
  expected = _lookupCounter.load();
  expected.count = 0;
  new_val = expected;
  new_val.lockAcquired = false;
  while (!_lookupCounter.compare_exchange_weak(expected, new_val)) {
    expected.count = 0;
    //stronger if set expected to true; lighter without
    //TODO should not loopback; log if it does
  }
}

int peano::heap::RecycleBucket::get_bucket_index(const int& heap_size) {
  incrementRefCount();
  int index = _data.count(heap_size) ? heap_size : -1;
  decrementRefCount();
  return index;
}


peano::heap::RecycleBucket::recycleList& peano::heap::RecycleBucket::get_bucket(const int &index) {
  incrementRefCount(); //will wait if lock is acquired
  recycleList& result = _data[index];
  decrementRefCount();
  return result;
}

void peano::heap::RecycleBucket::add_bucket(const int &index){
  tarch::multicore::Lock lock(_dataMapSemaphore);
  setLockAquiredFlag();

  if(_data.count(index) == 0) {
    _data[index] = recycleList();
  }

  unsetLockAquiredFlag();
  lock.free();
}

bool peano::heap::RecycleBucket::empty() {
  bool empty = true;
  //TODO deal with constness
  tarch::multicore::Lock lock(_dataMapSemaphore);
  for (auto it = _data.begin(); it != _data.end(); ++it) {
    if (!(it->second).empty()) {
      empty = false;
      break;
    }
  }
  lock.free();
  return empty;
}

bool peano::heap::RecycleBucket::empty(const int& heap_size) {
  int index = get_bucket_index(heap_size);
  bool result = index == -1 ? true : get_bucket(index).empty();
  return result;
}

peano::heap::RecycleBucket::size_type peano::heap::RecycleBucket::size() {
  size_type size = 0;
  //TODO deal with constness
  tarch::multicore::Lock lock(_dataMapSemaphore);
  for (auto it = _data.begin(); it != _data.end(); ++it) {
    size += (it->second).size();
  }
  lock.free();
  return size;
}

void peano::heap::RecycleBucket::remove(const int &heap_size, const int &value) {
  int index = get_bucket_index(heap_size);
  if(index != -1) get_bucket(index).remove(value);
}

void peano::heap::RecycleBucket::push_back(const int &heap_size, const int &value) {
  int index = get_bucket_index(heap_size);
  if (index == -1) add_bucket(heap_size);
  get_bucket(index).push_back(value);
}

peano::heap::RecycleBucket::reference peano::heap::RecycleBucket::front(const int &heap_size) {
  int index = get_bucket_index(heap_size);
  if(index == -1) return _dummy[-1].front();
  return get_bucket(index).front();
}

void peano::heap::RecycleBucket::pop_front(const int &heap_size) {
  int index = get_bucket_index(heap_size);
  if (index != -1) get_bucket(index).pop_front();
}
