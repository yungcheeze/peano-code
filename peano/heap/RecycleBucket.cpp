#include "peano/heap/RecycleBucket.h"

int peano::heap::RecycleBucket::get_bucket_index(const int& heap_size) {
  //prevent infinite looping when finding targetlevel
  if (heap_size < 0) {
    return 0;
  }

  //obtain bucket index corresponding to heap_size
  // see https://stackoverflow.com/questions/994593/how-to-do-an-integer-log2-in-c
  int index = heap_size;
  unsigned int targetlevel = 0;
  while (index >>= 1) ++targetlevel;

  if (targetlevel >= _data.size()) {
    return 0;
  }
  return targetlevel;
}

bool peano::heap::RecycleBucket::empty() const {
  bool empty = true;
  for (auto it = _data.begin(); it != _data.end(); ++it) {
    if (!(it->empty())) {
      empty = false;
      break;
    }
  }
  return empty;
}

bool peano::heap::RecycleBucket::empty(const int& heap_size) {
  int index = get_bucket_index(heap_size);
  return _data[index].empty();
}

peano::heap::RecycleBucket::size_type peano::heap::RecycleBucket::size() const {
  size_type size = 0;
  for (auto it = _data.begin(); it != _data.end(); ++it) {
    size += it->size();
  }
  return size;
}

void peano::heap::RecycleBucket::remove(const int &heap_size, const int &value) {
  int index = get_bucket_index(heap_size);
  _data[index].remove(value);
}

void peano::heap::RecycleBucket::push_back(const int &heap_size, const int &value) {
  int index = get_bucket_index(heap_size);
  _data[index].push_back(value);
}

peano::heap::RecycleBucket::reference peano::heap::RecycleBucket::front(const int &heap_size) {
  int index = get_bucket_index(heap_size);
  return _data[index].front();
}

void peano::heap::RecycleBucket::pop_front(const int &heap_size) {
  int index = get_bucket_index(heap_size);
  _data[index].pop_front();
}
