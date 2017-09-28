#include "peano/heap/ThreadSafeHeap.h"

tarch::logging::Log peano::heap::ThreadSafeHeap::_log("peano::heap::ThreadSafeHeap");
std::atomic_size_t peano::heap::ThreadSafeHeap::_objCount;
std::atomic_size_t peano::heap::ThreadSafeHeap::_totalCap;
std::atomic_size_t peano::heap::ThreadSafeHeap::_totalSize;

inline size_t inBytes(const std::vector<double>::size_type& size) {
    return sizeof(double) * size;
}
//Constructors
peano::heap::ThreadSafeHeap::ThreadSafeHeap(size_type count):
    _data(count)
{
    logInfo( "ThreadSafeHeap()",
             "constructor: "
             << "size " << inBytes(_data.size()) << "b; ");

    _totalCap += inBytes(_data.capacity());
    logInfo( "ThreadSafeHeap()", "total_capacity " << _totalCap);

    _totalSize += inBytes(_data.capacity());
    logInfo( "ThreadSafeHeap()", "total_size " << _totalSize);
}

//destructor
peano::heap::ThreadSafeHeap::~ThreadSafeHeap() {
    logInfo( "~ThreadSafeHeap()", "destructor: " );

    _totalCap -= inBytes(_data.capacity());
    logInfo( "~ThreadSafeHeap()", "total_capacity " << _totalCap);

    _totalSize -= inBytes(_data.size());
    logInfo( "~ThreadSafeHeap()", "total_size " << _totalSize);
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
            << "mem_freed " << inBytes(_data.size()));

    _totalSize -= inBytes(_data.size());
    logInfo( "clear()", "total_size " << _totalSize);

    _data.clear();
}

void peano::heap::ThreadSafeHeap::shrink_to_fit() {
    size_type old_capacity = _data.capacity();
    size_type old_size = _data.size();
    _data.shrink_to_fit();
    size_type new_capacity = _data.capacity();
    size_type new_size = _data.size();
    logInfo("shrink_to_fit()", "shrink_to_fit: "
            << "old_size " << inBytes(old_size) << "b; "
            << "old_capacity " << inBytes(old_capacity) << "b; "
            << "new_size " << inBytes(new_size) << "b; "
            << "new_capacity " << inBytes(new_capacity) << "b; ");

    if (new_capacity != old_capacity) {
        _totalCap += inBytes(new_capacity - old_capacity);
        logInfo( "shrink_to_fit()", "total_capacity " << _totalCap);
    }
}

void peano::heap::ThreadSafeHeap::resize(size_type count) {
    size_type old_capacity = _data.capacity();
    size_type old_size = _data.size();
    _data.resize(count);
    size_type new_capacity = _data.capacity();
    size_type new_size = _data.size();
    logInfo("resize()", "resize: "
            << "old_size " << inBytes(old_size) << "b; "
            << "old_capacity " << inBytes(old_capacity) << "b; "
            << "new_size " << inBytes(new_size) << "b; "
            << "new_capacity " << inBytes(new_capacity) << "b; ");

    if (new_capacity != old_capacity) {
        _totalCap += inBytes(new_capacity - old_capacity);
        logInfo( "resize()", "total_capacity " << _totalCap);
    }
    if (new_size != old_size) {
        _totalSize += inBytes(new_size - old_size);
        logInfo( "resize()", "total_size " << _totalSize);
    }
}

void peano::heap::ThreadSafeHeap::reserve(size_type count) {
    size_type old_capacity = _data.capacity();
    size_type old_size = _data.size();
    _data.reserve(count);
    size_type new_capacity = _data.capacity();
    size_type new_size = _data.size();
    logInfo("reserve()", "reserve: "
            << "old_size " << inBytes(old_size) << "b; "
            << "old_capacity " << inBytes(old_capacity) << "b; "
            << "new_size " << inBytes(new_size) << "b; "
            << "new_capacity " << inBytes(new_capacity) << "b; ");

    if (new_capacity != old_capacity) {
        _totalCap += inBytes(new_capacity - old_capacity);
        logInfo( "reserve()", "total_capacity " << _totalCap);
    }
}
