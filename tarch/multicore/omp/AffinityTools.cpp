#include "tarch/multicore/AffinityTools.h"
#include "tarch/multicore/Core.h"


#if defined(SharedOMP)

std::vector<tarch::multicore::AffinityMask> tarch::multicore::getThreadAffinities() {
  std::vector<tarch::multicore::AffinityMask> coreAffinities( Core::getInstance().getNumberOfThreads(), -1 );

  // Run on each and every rank
  #pragma omp parallel
  {
    coreAffinities[ omp_get_thread_num() ] = tarch::multicore::getCPUSet();
  }

  return coreAffinities;
}

#endif

