#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/BackgroundTasks.h"


// This implementation is valid iff neither OpenMP nor TBBs nor any other
// shared memory parallelisation are active

#if !defined(SharedMemoryParallelisation)

tarch::multicore::BooleanSemaphore::BooleanSemaphore() {
}


tarch::multicore::BooleanSemaphore::~BooleanSemaphore() {
}


void tarch::multicore::BooleanSemaphore::enterCriticalSection() {
}


void tarch::multicore::BooleanSemaphore::leaveCriticalSection() {
}


void tarch::multicore::BooleanSemaphore::sendTaskToBack() {
  processBackgroundTasks();
}

#endif
