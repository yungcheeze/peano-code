// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MPIANALYIS_DEFAULT_ANALYSER_H_
#define _TARCH_MPIANALYIS_DEFAULT_ANALYSER_H_


#include "tarch/mpianalysis/Analyser.h"
#include "tarch/logging/Log.h"
#include "tarch/timing/Watch.h"


namespace tarch {
  namespace mpianalysis {
    class DefaultAnalyser;
  }
}



/**
 * Analyser Interface
 *
 * @author Roland Wittmann, Tobias Weinzierl
 */
class tarch::mpianalysis::DefaultAnalyser: public tarch::mpianalysis::Analyser {
  private:
    static tarch::logging::Log     _log;

    tarch::timing::Watch           _watch;

    tarch::timing::Watch           _synchronousHeapWatch;
    tarch::timing::Watch           _asynchronousHeapWatch;
  public:
    DefaultAnalyser();
    virtual ~DefaultAnalyser();

    virtual void beginIteration();

    virtual void endIteration();

    virtual void addWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<3,double>&  boundingBoxOffset,
      const tarch::la::Vector<3,double>&  boundingBoxSize
    );

    virtual void removeWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<3,double>&  boundingBoxOffset,
      const tarch::la::Vector<3,double>&  boundingBoxSize
    );

    virtual void addWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<2,double>&  boundingBoxOffset,
      const tarch::la::Vector<2,double>&  boundingBoxSize
    );

    virtual void removeWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<2,double>&  boundingBoxOffset,
      const tarch::la::Vector<2,double>&  boundingBoxSize
    );

    virtual void tagIsUsedFor( int tag, const std::string& communicationTypeIdentifier );
    virtual void dataWasNotReceivedInBackground( int fromRank, int tag, int cardinality, int pageSize );
    virtual void dataWasNotReceivedFromWorker( int fromRank, double calendarTime );
    virtual void logNodePoolStatistics(int registeredWorkers, int idleWorkers);
    virtual void beginToReleaseSynchronousHeapData();
    virtual void endToReleaseSynchronousHeapData();
    virtual void beginToPrepareAsynchronousHeapDataExchange();
    virtual void endToPrepareAsynchronousHeapDataExchange();
    virtual void endReleaseOfJoinData();
    virtual void endReleaseOfBoundaryData();
};


#endif
