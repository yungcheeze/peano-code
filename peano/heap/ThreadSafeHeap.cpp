#include "peano/heap/ThreadSafeHeap.h"

tarch::logging::Log ThreadSafeHeap::_log("ThreadSafeHeap");

//constructors
ThreadSafeHeap::ThreadSafeHeap(size_type count): _data(count) {
    logInfo( "ThreadSafeHeap()", "constructor" << count );
}

//indexing
double& ThreadSafeHeap::operator[](size_type index) {
    return _data[index];
}

const double& ThreadSafeHeap::operator[](size_type index) const {
    return _data[index];
}


//iterators
ThreadSafeHeap::iterator ThreadSafeHeap::begin() {
    return _data.begin();
}

ThreadSafeHeap::const_iterator ThreadSafeHeap::begin() const {
    return _data.begin();
}


ThreadSafeHeap::iterator ThreadSafeHeap::end() {
    return _data.end();
}

ThreadSafeHeap::const_iterator ThreadSafeHeap::end() const {
    return _data.end();
}

double* ThreadSafeHeap::data() {
    return _data.data();
}

const double* ThreadSafeHeap::data() const {
    return _data.data();
}

//memory management
void ThreadSafeHeap::clear() {
    _data.clear();
}
void ThreadSafeHeap::shrink_to_fit() {
    logInfo( "shrink_to_fit()", "shrink_to_fit" );
    _data.shrink_to_fit();
}
void ThreadSafeHeap::resize(size_type count) {
    logInfo( "resize()", "resize" << count);
   _data.resize(count);
}
void ThreadSafeHeap::reserve(size_type count) {
    logInfo( "reserve()", "reserve" << count);
    _data.reserve(count);
}
