#include "peano/heap/ThreadSafeHeap.h"

tarch::logging::Log ThreadSafeHeap::_log("ThreadSafeHeap");

//constructors
inline ThreadSafeHeap::ThreadSafeHeap(size_type count): _data(count) {}

//indexing
inline double& ThreadSafeHeap::operator[](size_type index) {
    return _data[index];
}

inline const double& ThreadSafeHeap::operator[](size_type index) const {
    return _data[index];
}


//iterators
inline ThreadSafeHeap::iterator ThreadSafeHeap::begin() {
    return _data.begin();
}

inline ThreadSafeHeap::const_iterator ThreadSafeHeap::begin() const {
    return _data.begin();
}


inline ThreadSafeHeap::iterator ThreadSafeHeap::end() {
    return _data.end();
}

inline ThreadSafeHeap::const_iterator ThreadSafeHeap::end() const {
    return _data.end();
}

inline double* ThreadSafeHeap::data() {
    return _data.data();
}

inline const double* ThreadSafeHeap::data() const {
    return _data.data();
}

//memory management
inline void ThreadSafeHeap::clear() { _data.clear(); }
inline void ThreadSafeHeap::shrink_to_fit() { _data.shrink_to_fit(); }
inline void ThreadSafeHeap::resize(size_type count) {
   _data.resize(count);
}
inline void ThreadSafeHeap::reserve(size_type count) {
    _data.reserve(count);
}
