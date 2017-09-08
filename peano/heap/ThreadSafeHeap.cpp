#include "peano/heap/ThreadSafeHeap.h"

tarch::logging::Log peano::heap::ThreadSafeHeap::_log("peano::heap::ThreadSafeHeap");

//constructors
peano::heap::ThreadSafeHeap::ThreadSafeHeap(size_type count): _data(count) {
    logInfo( "ThreadSafeHeap()", "constructor" << count );
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
    _data.clear();
}
void peano::heap::ThreadSafeHeap::shrink_to_fit() {
    logInfo( "shrink_to_fit()", "shrink_to_fit" );
    _data.shrink_to_fit();
}
void peano::heap::ThreadSafeHeap::resize(size_type count) {
    logInfo( "resize()", "resize" << count);
   _data.resize(count);
}
void peano::heap::ThreadSafeHeap::reserve(size_type count) {
    logInfo( "reserve()", "reserve" << count);
    _data.reserve(count);
}
