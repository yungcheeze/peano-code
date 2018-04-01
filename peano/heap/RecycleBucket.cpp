#include "peano/heap/RecycleBucket.h"

peano::heap::RecycleBucket::RecycleBucket(): _data(), _dummy() {
  _dummy[-1] = recycleList(); // index to lookup on failure
  _dummy[-1].push_back(-1); // value to return on failure
}

int peano::heap::RecycleBucket::get_bucket_index(const int& heap_size) {
  int index = _data.count(heap_size) ? heap_size : -1;
  return index;
}


peano::heap::RecycleBucket::recycleList& peano::heap::RecycleBucket::get_bucket(const int &index) {
  //c&s loop(increment ref count if flag is false)
  recycleList& result = _data[index];
  //decrement ref count
  return result;
}

void peano::heap::RecycleBucket::add_bucket(const int &index){
  //TODO lock this
  //acquire iteration lock
  //c&s loop (set flag to true if ref count is zero)
  if(_data.count(index) == 0) {
    _data[index] = recycleList();
  }
  //set flag to false
}

bool peano::heap::RecycleBucket::empty() const {
  bool empty = true;
  //TODO acquire iteration lock
  for (auto it = _data.begin(); it != _data.end(); ++it) {
    if (!(it->second).empty()) {
      empty = false;
      break;
    }
  }
  return empty;
}

bool peano::heap::RecycleBucket::empty(const int& heap_size) {
  int index = get_bucket_index(heap_size);
  bool result = index == -1 ? false : get_bucket(index).empty();
  return result;
}

peano::heap::RecycleBucket::size_type peano::heap::RecycleBucket::size() const {
  size_type size = 0;
  //TODO acquire iteration lock
  for (auto it = _data.begin(); it != _data.end(); ++it) {
    size += (it->second).size();
  }
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
