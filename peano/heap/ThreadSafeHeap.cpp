#include "peano/heap/ThreadSafeHeap.h"

tarch::logging::Log peano::heap::ThreadSafeHeap::_log("peano::heap::ThreadSafeHeap");
std::atomic_size_t peano::heap::ThreadSafeHeap::_objCount;
std::atomic_size_t peano::heap::ThreadSafeHeap::_totalCap;
std::atomic_size_t peano::heap::ThreadSafeHeap::_totalSize;
std::atomic_size_t peano::heap::ThreadSafeHeap::_maxCap;
std::atomic_size_t peano::heap::ThreadSafeHeap::_maxSize;

inline size_t inBytes(const std::vector<double>::size_type& size) {
  return sizeof(double) * size;
}
//Constructors
peano::heap::ThreadSafeHeap::ThreadSafeHeap(size_type count):
  _data(count), _objID(++_objCount)
{
  _totalCap += inBytes(_data.capacity());
  _totalSize += inBytes(_data.size());
  // logInfo( "ThreadSafeHeap()",
  //          "constructor: "
  //          << "objID " << _objID << "; "
  //          << "size " << inBytes(_data.capacity()) << "b; "
  //          << "total_capacity " << _totalCap << "b; "
  //          << "total_size " << _totalSize << "b; ");
}

//destructor
peano::heap::ThreadSafeHeap::~ThreadSafeHeap() {
  _totalCap -= inBytes(_data.capacity());
  _totalSize -= inBytes(_data.size());
  size_t count = _objCount.fetch_sub(1);
  if (count == 1) {
    logInfo( "~ThreadSafeHeap()", "fragmentation: "
             << "max_capacity " << _maxCap << "b; "
             << "max_size " << _maxSize << "b; ");
  }

  // logInfo( "~ThreadSafeHeap()", "destructor: "
  //          << "objID " << _objID << "; "
  //          << "mem_freed " << inBytes(_data.capacity()) << "b; "
  //          << "total_capacity " << _totalCap << "; "
  //          << "total_size " << _totalSize << "; ");
}

//indexing
double& peano::heap::ThreadSafeHeap::operator[](size_type index) {
  return _data[index];
}

const double& peano::heap::ThreadSafeHeap::operator[](size_type index) const {
  return _data[index];
}


//iterators
peano::heap::ThreadSafeHeap::iterator peano::heap::ThreadSafeHeap::begin() {
  return _data.begin();
}

peano::heap::ThreadSafeHeap::const_iterator peano::heap::ThreadSafeHeap::begin() const {
  return _data.begin();
}


peano::heap::ThreadSafeHeap::iterator peano::heap::ThreadSafeHeap::end() {
  return _data.end();
}

peano::heap::ThreadSafeHeap::const_iterator peano::heap::ThreadSafeHeap::end() const {
  return _data.end();
}

double* peano::heap::ThreadSafeHeap::data() {
  return _data.data();
}

const double* peano::heap::ThreadSafeHeap::data() const {
  return _data.data();
}

//memory management
void peano::heap::ThreadSafeHeap::clear() {
  size_t currsz = _totalSize.fetch_sub(inBytes(_data.size()));
  if (currsz > _maxSize) {
    _maxSize.exchange(currsz);
  }
  size_t currcap = _totalCap.load();
  if (currcap > _maxCap) {
    _maxCap.exchange(currcap);
  }
  // _totalSize -= inBytes(_data.size());
  // logInfo("clear()", "clear: "
  //         << "objID " << _objID << "; "
  //         << "mem_released " << inBytes(_data.size()) << "b; "
  //         << "total_capacity " << _totalCap << "b; "
  //         << "total_size " << _totalSize << "b; "
  //         );
  _data.clear();
}

void peano::heap::ThreadSafeHeap::shrink_to_fit() {
  size_type old_capacity = _data.capacity();
  _data.shrink_to_fit();
  size_type new_capacity = _data.capacity();

  _totalCap += inBytes(new_capacity - old_capacity);

  // logInfo("shrink_to_fit()", "shrink_to_fit: "
  //         << "objID " << _objID << "; "
  //         << "old_capacity " << inBytes(old_capacity) << "b; "
  //         << "new_capacity " << inBytes(new_capacity) << "b; "
  //         << "total_capacity " << _totalCap << "b; "
  //         << "total_size " << _totalSize << "b; "
  //         );
}

void peano::heap::ThreadSafeHeap::resize(size_type count) {
  size_type old_capacity = _data.capacity();
  size_type old_size = _data.size();
  _data.resize(count);
  size_type new_capacity = _data.capacity();
  size_type new_size = _data.size();

  // _totalCap += inBytes(new_capacity - old_capacity);
  // _totalSize += inBytes(new_size - old_size);

  size_t currsz = _totalSize.fetch_add(inBytes(new_size - old_size));
  if (currsz > _maxSize) {
    _maxSize.exchange(currsz);
  }
  size_t currcap = _totalCap.fetch_add(inBytes(new_capacity - old_capacity));
  if (currcap > _maxCap) {
    _maxCap.exchange(currcap);
  }
  // logInfo("resize()", "resize: "
  //         << "objID " << _objID << "; "
  //         << "old_size " << inBytes(old_size) << "b; "
  //         << "old_capacity " << inBytes(old_capacity) << "b; "
  //         << "new_size " << inBytes(new_size) << "b; "
  //         << "new_capacity " << inBytes(new_capacity) << "b; "
  //         << "total_capacity " << _totalCap << "b; "
  //         << "total_size " << _totalSize << "b; ");
}

void peano::heap::ThreadSafeHeap::reserve(size_type count) {
  size_type old_capacity = _data.capacity();
  size_type old_size = _data.size();
  _data.reserve(count);
  size_type new_capacity = _data.capacity();
  size_type new_size = _data.size();

  _totalCap += inBytes(new_capacity - old_capacity);
  _totalSize += inBytes(new_size - old_size);
  // logInfo("reserve()", "reserve: "
  //         << "objID " << _objID << "; "
  //         << "old_size " << inBytes(old_size) << "b; "
  //         << "old_capacity " << inBytes(old_capacity) << "b; "
  //         << "new_size " << inBytes(new_size) << "b; "
  //         << "new_capacity " << inBytes(new_capacity) << "b; "
  //         << "total_capacity " << _totalCap << "b; "
  //         << "total_size " << _totalSize << "b; ");
}
