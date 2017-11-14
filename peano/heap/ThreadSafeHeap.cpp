#include "peano/heap/ThreadSafeHeap.h"

tarch::logging::Log peano::heap::ThreadSafeHeap::_log("peano::heap::ThreadSafeHeap");
std::atomic_size_t peano::heap::ThreadSafeHeap::_objCount = 0;

inline size_t inBytes(const std::vector<double>::size_type& size) {
    return sizeof(double) * size;
}
//Constructors
peano::heap::ThreadSafeHeap::ThreadSafeHeap(size_type count):
    _data(count), _id(++_objCount)
{
    logInfo( "ThreadSafeHeap()",
             "constructor: "
             << "id " << _id << "; "
             << "size " << inBytes(_data.size()) << "b; ");
}

//destructor
peano::heap::ThreadSafeHeap::~ThreadSafeHeap() {
    logInfo( "~ThreadSafeHeap()", "destructor: " 
             << "id " << _id << "; ");
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
    logInfo("clear()", "clear: "
            << "id " << _id << "; "
            << "mem_freed " << inBytes(_data.size()));
    _data.clear();
}

void peano::heap::ThreadSafeHeap::shrink_to_fit() {
    size_type old_capacity = _data.capacity();
    size_type old_size = _data.size();
    _data.shrink_to_fit();
    size_type new_capacity = _data.capacity();
    size_type new_size = _data.size();
    logInfo("shrink_to_fit()", "shrink_to_fit: "
            << "id " << _id << "; "
            << "old_size " << inBytes(old_size) << "b; "
            << "old_capacity " << inBytes(old_capacity) << "b; "
            << "new_size " << inBytes(new_size) << "b; "
            << "new_capacity " << inBytes(new_capacity) << "b; ");
}

void peano::heap::ThreadSafeHeap::resize(size_type count) {
    size_type old_capacity = _data.capacity();
    size_type old_size = _data.size();
    _data.resize(count);
    size_type new_capacity = _data.capacity();
    size_type new_size = _data.size();
    logInfo("resize()", "resize: "
            << "id " << _id << "b; "
            << "old_size " << inBytes(old_size) << "b; "
            << "old_capacity " << inBytes(old_capacity) << "b; "
            << "new_size " << inBytes(new_size) << "b; "
            << "new_capacity " << inBytes(new_capacity) << "b; ");
}

void peano::heap::ThreadSafeHeap::reserve(size_type count) {
    size_type old_capacity = _data.capacity();
    size_type old_size = _data.size();
    _data.reserve(count);
    size_type new_capacity = _data.capacity();
    size_type new_size = _data.size();
    logInfo("reserve()", "reserve: "
            << "id " << _id << "b; "
            << "old_size " << inBytes(old_size) << "b; "
            << "old_capacity " << inBytes(old_capacity) << "b; "
            << "new_size " << inBytes(new_size) << "b; "
            << "new_capacity " << inBytes(new_capacity) << "b; ");
}
