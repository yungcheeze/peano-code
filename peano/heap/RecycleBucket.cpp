#include "peano/heap/RecycleBucket.h"

peano::heap::RecycleBucket::RecycleBucket(): _data() {
  _data[-1] = recycleList();
}

int peano::heap::RecycleBucket::get_bucket_index(const int& heap_size) {
  return heap_size;
}


peano::heap::RecycleBucket::recycleList& peano::heap::RecycleBucket::get_bucket(const int &index) {
    if(_data.count(index) > 0) {
      return _data[index];
    } else {
      return _data[-1];
    }
}

void peano::heap::RecycleBucket::add_bucket(const int &index){
  if(_data.count(index) == 0) {
    _data[index] = recycleList();
  }
}

bool peano::heap::RecycleBucket::empty() const {
  bool empty = true;
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
  return get_bucket(index).empty();
}

peano::heap::RecycleBucket::size_type peano::heap::RecycleBucket::size() const {
  size_type size = 0;
  for (auto it = _data.begin(); it != _data.end(); ++it) {
    size += (it->second).size();
  }
  return size;
}

void peano::heap::RecycleBucket::remove(const int &heap_size, const int &value) {
  int index = get_bucket_index(heap_size);
  get_bucket(index).remove(value);
}

void peano::heap::RecycleBucket::push_back(const int &heap_size, const int &value) {
  int index = get_bucket_index(heap_size);
  get_bucket(index).push_back(value);
}

peano::heap::RecycleBucket::reference peano::heap::RecycleBucket::front(const int &heap_size) {
  int index = get_bucket_index(heap_size);
  return get_bucket(index).front();
}

void peano::heap::RecycleBucket::pop_front(const int &heap_size) {
  int index = get_bucket_index(heap_size);
  get_bucket(index).pop_front();
}
