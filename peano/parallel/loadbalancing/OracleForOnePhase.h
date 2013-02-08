// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_PARALLEL_LOADBALANCING_ORACLE_FOR_ONE_PHASE_H_
#define _PEANO_PARALLEL_LOADBALANCING_ORACLE_FOR_ONE_PHASE_H_


#include <map>
#include <string>


#include "peano/utils/Globals.h"


namespace peano {
  namespace parallel {
    namespace loadbalancing {
      class OracleForOnePhase;

      /**
       * Represents action to be done.
       */
      enum LoadBalancingFlag {
        Continue=-2, Join=-1, UndefinedLoadBalancingFlag=0, ForkOnce=1, ForkGreedy=THREE_POWER_D_MINUS_ONE
      };

      LoadBalancingFlag getLoadBalancingFlag( int value );

      std::string toString(const LoadBalancingFlag& flag);
    }
  }
}


/**
 * Abstract superclass of all oracles
 *
 * This is the principal plugin point for any load balancing strategy. This
 * class decides for existing workers whether these workers shall continue
 * their work, shall merge their domains with their master (where this
 * oracle is running on), or whether they should try to fork further. The
 * important operations of this class are OracleForOnePhase::receivedStartCommand(),
 * OracleForOnePhase::getCommandForWorker(), and OracleForOnePhase::receivedTerminateCommand().
 *
 * @image html OracleForOnePhase0.png
 *
 * @image html OracleForOnePhase1.png
 *
 * !!! Copy constructor
 *
 * The oracle singleton never works with the original oracle. Instead, it
 * clones its oracle strategy for each new phase (see createNewOracle()).
 *
 * @author Tobias Weinzierl
 */
class peano::parallel::loadbalancing::OracleForOnePhase {
  public:
    virtual ~OracleForOnePhase() {}

    /**
     * Receive start command from master
     *
     * This operation is invoked per grid traversal at the very beginning. The
     * master tells its worker what to do. Hence, also the worker's oracle is
     * informed about the master's strategy. For example, if the master tells
     * this node to fork further, i.e. to reduce its local workload, this might
     * also be relevant to the oracle telling workers of the worker to fork, too.
     *
     * If you wanna keep track of the runtime per worker, the simplest way to
     * realise this is to write a time stamp within this method, as this method
     * is invoked exactly once per traversal.
     *
     * @see Oracle
     */
    virtual void receivedStartCommand(const LoadBalancingFlag& commandFromMaster ) = 0;

    /**
     * Get command for one single worker
     *
     * This way, Peano asks the oracle how to proceed with the worker
     * workerRank. Shall the parallel travesal simply continue, shall the
     * worker in turn try to fork its partition, or shall the worker merge
     * with the master. The latter is not allowed all the time, i.e. if
     * joinIsAllowed is not set, please do not return Join as result.
     *
     * The operation provides a way to implement dynamic load balancing. You
     * always are notices by receivedTerminateCommand() how long a worker has
     * needed to complete its tasks. This is the right plugin point for an
     * oracle for one phase to keep track of the runtimes. If a worker is too
     * slow, you should pass im a fork command the next time it is asking for
     * getCommandForWorker().
     *
     * There is one oracle per phase, i.e. per adapter. Usually, one or two
     * different adapter coin the runtime, while others have completely different
     * runtime characteristics. An example: For a CFD code for incompressible
     * fluids with an explicit time stepping, the update of time steps and the
     * computation of the right-hand side are rather cheap and/or invoked not that
     * often. Most of the time is spent to solve the pressure poisson equation. As
     * a consequence, the load balancing should only be realised in the oracle for
     * this pressure poisson equation.
     *
     * When you design your load balancing, please keep in mind that the whole
     * Peano code defines a logical tree topology on all the ranks, and that the
     * oracles are not working globally but do exist on each MPI rank. This
     * directly yields the design of getCommandForWorker() and
     * receivedTerminateCommand(): The global automaton runs through the spacetree
     * (or regular grid) top-down. Throughout the top-down steps or traversal,
     * respectively, it invokes getCommandForWorker() for its workers. They then
     * start to traverse their own stuff (and to invoke their own workers in
     * turn). When the local tree/grid is traversed, the node waits for its
     * workers and, as soon as they have finished their stuff, calls
     * receivedTerminateCommand().
     *
     * @see Oracle
     * @see receivedTerminateCommand()
     */
    virtual LoadBalancingFlag getCommandForWorker( int workerRank, bool forkIsAllowed, bool joinIsAllowed ) = 0;

    /**
     * @param waitedTime This is the time the master had to wait for the worker,
     * i.e. this is not the runtime of the worker, as the runtime of the worker
     * is not important for the master - only whether the worker deployed its
     * results on time or not. If you are interested in runtimes per worker, you
     * should implement receivedStartCommand().
     *
     * @see receivedStartCommand()
     * @see getCommandForWorker()
     * @see Oracle
     *
     * @param workerRank   Rank of the worker that has just reported that it finished its traversal.
     * q
     */
    virtual void receivedTerminateCommand(
      int     workerRank,
      double  waitedTime,
      double  workerNumberOfInnerVertices,
      double  workerNumberOfBoundaryVertices,
      double  workerNumberOfOuterVertices,
      double  workerNumberOfInnerCells,
      double  workerNumberOfOuterCells,
      int     workerMaxLevel,
      int     workerLocalWorkload,
      int     workerTotalWorkload,
      int     currentLevel,
      int     parentCellLocalWorkload,
      int     parentCellTotalWorkload
    ) = 0;

    // @todo
    // neue Operation, um entscheiden zu koennen, was man denn so macht

    /**
     * Plot something to info log device.
     */
    virtual void plotStatistics() = 0;

    /**
     * Clone this oracle. This operation is used by the singleton whenever a
     * piece of code asks for parallelisation that never asked before.
     *
     * @param adapterNumber Number of your adapter. Have a closer look to your
     *        repository's state if you want to find out which adapters are
     *        mapped to which state. You can even use the toString() operation
     *        there to map this parameter to a string.
     */
    virtual OracleForOnePhase* createNewOracle(int adapterNumber) const = 0;

    /**
     * @see Oracle
     */
    virtual void forkFailed() = 0;
};


#endif
