#if defined(SharedTBB)
#include "tarch/multicore/tbb/Core.h"
#include "tarch/Assertions.h"


tarch::logging::Log  tarch::multicore::tbb::Core::_log( "tarch::multicore::tbb::Core" );

const int tarch::multicore::tbb::Core::UseDefaultNumberOfThreads = 0;


tarch::multicore::tbb::Core::Core():
  _numberOfThreads(::tbb::task_scheduler_init::default_num_threads()),
  _task_scheduler_init(_numberOfThreads) {
}


tarch::multicore::tbb::Core::~Core() {

}


tarch::multicore::tbb::Core& tarch::multicore::tbb::Core::getInstance() {
  static tarch::multicore::tbb::Core singleton;
  return singleton;
}


void tarch::multicore::tbb::Core::shutDown() {
  _task_scheduler_init.terminate();
  _numberOfThreads = -1;
}


void tarch::multicore::tbb::Core::configure( int numberOfThreads ) {
  if (_task_scheduler_init.is_active()) {
    _task_scheduler_init.terminate();
  }

  if (numberOfThreads==UseDefaultNumberOfThreads) {
    _numberOfThreads = ::tbb::task_scheduler_init::default_num_threads();
  }
  else {
    _numberOfThreads = numberOfThreads;
  }

  _task_scheduler_init.initialize( _numberOfThreads );
}


int tarch::multicore::tbb::Core::getNumberOfThreads() const {
  assertion( isInitialised() );
  return _numberOfThreads;
}


bool tarch::multicore::tbb::Core::isInitialised() const {
  return _task_scheduler_init.is_active();
}

#endif
