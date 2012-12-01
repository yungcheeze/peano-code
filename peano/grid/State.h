// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_STATE_H_
#define _PEANO_GRID_STATE_H_


#include "tarch/logging/Log.h"
#include "peano/utils/PeanoOptimisations.h"


#include <set>


namespace peano {
  namespace grid {
      template <class StateData>
      class State;
	}
}



/**
 * Super class for all states of the grid.
 *
 * @author Tobias Weinzierl
 */
template <class StateData>
class peano::grid::State {
  public:
    #if defined(StoreOnlyPersistentAttributes) && defined(PackedRecords)
    typedef typename StateData::Packed::PersistentRecords  PersistentState;
    #endif

    #if !defined(StoreOnlyPersistentAttributes) && defined(PackedRecords)
    typedef typename StateData::Packed                     PersistentState;
    #endif

    #if defined(StoreOnlyPersistentAttributes) && !defined(PackedRecords)
    typedef typename StateData::PersistentRecords          PersistentState;
    #endif

    #if !defined(StoreOnlyPersistentAttributes) && !defined(PackedRecords)
    typedef StateData                                      PersistentState;
    #endif

    #ifdef Parallel
    /**
     * This operation is robust, i.e. you can call it several times.
     */
    static void initDatatype();
    #endif

    #if defined(ParallelExchangePackedRecords)
    typedef typename StateData::Packed       MPIDatatypeContainer;
    #else
    typedef StateData                        MPIDatatypeContainer;
    #endif

    typedef StateData                                      Records;

  protected:
    StateData _stateData;

    State();
    State(const PersistentState& argument);

    #ifdef Parallel
    /**
     * Merge with worker state
     *
     * This is a local variant of the state merger that is usually not called
     * alone. However, I have some general remarks on the state:
     *
     * - beginIteration is the first time when a mapping sees a state.
     * - This state then is distributed by Peano automatically among the
     *   workers.
     * - Throughout the traversal, Peano merges back worker states into
     *   the master due to this operation, i.e.
     * - When you receive endIteration(), all the worker states already are
     *   merged into the master state and no merges are pending.
     */
    void mergeWithWorkerState(const peano::grid::State<StateData>& workerState);
    #endif

  private:
    /**
     * Logging device
     */
    static tarch::logging::Log  _log;

    #ifdef Parallel
    /**
     * The regular grid holds one integer storing the rank of the
     * forking/joining/triggering rank, and this integer is not
     * parallelised. For the spacetree grid, such a solution is not
     * possible, as there might be several forking and joining ranks.
     * I consequently do not model the sequence of ranks as DaStGen
     * attribute but make it a vector here.
     *
     * So, this flag either holds all the joining ranks or all the
     * forking ranks.
     */
    std::set<int>                _loadRebalancingRemoteRanks;
    #endif
  public:
     ~State();

    /**
     * Proxy.
     */
    std::string toString() const;

    /**
     * Proxy.
     */
    void toString(std::ostream& out) const;

    /**
     * @see getMaximumMeshWidth() for comments on the validity of the result
     */
     double getNumberOfInnerVertices() const;

    /**
     * @see getMaximumMeshWidth() for comments on the validity of the result
     */
     double getNumberOfBoundaryVertices() const;

    /**
     * @see getMaximumMeshWidth() for comments on the validity of the result
     */
     double getNumberOfOuterVertices() const;

    /**
     * @see getMaximumMeshWidth() for comments on the validity of the result
     */
     double getNumberOfInnerCells() const;

    /**
     * @see getMaximumMeshWidth() for comments on the validity of the result
     */
     double getNumberOfOuterCells() const;

    /**
     * Get maximum  mesh width
     *
     * This operation returns a valid value at the end of the iteration, i.e.
     * in endIteration(). Throughout the traversal and in particular in
     * beginIteration(), it holds invalid values. If you need this value
     * throughout the traversal, add your application-specific state a field,
     * copy this attribute's value into this field in the traversal before, and
     * read out this buffered value afterwards instead of using this function.
     *
     * Be aware that for such a pattern, the value still might be invalid, if
     * the grid changes throughout the traversal.
     *
     * @see resetStateAtBeginOfIteration()
     */
    tarch::la::Vector<DIMENSIONS,double> getMaximumMeshWidth() const;

    /**
     * @see getMaximumMeshWidth() for comments on the validity of the result
     */
    tarch::la::Vector<DIMENSIONS,double> getMinimumMeshWidth() const;

    int getMaxLevel() const;

    void incNumberOfInnerVertices( double increment );
    void incNumberOfBoundaryVertices( double increment );
    void incNumberOfOuterVertices( double increment );
    void incNumberOfInnerCells( double increment );
    void incNumberOfOuterCells( double increment );

    bool isTraversalInverted() const;

    void updateRefinementHistoryAfterLoad( bool hasRefinened, bool hasErased, bool hasChangedVertexState );
    void updateRefinementHistoryBeforeStore( bool hasTriggeredRefinementForNextIteration, bool hasTriggeredEraseForNextIteration );
    void updateInLeaf( int level, const tarch::la::Vector<DIMENSIONS,double>& h );

    /**
     * Inform state that caller has changed a cell.
     */
    void changedCellState();

    /**
     * @return Has the grid changed the cells state in the last iteration.
     */
    bool isGridStationary() const;

    /**
     * Reset state at begin of iteration
     *
     * - All the have... (somehow changed the grid) are set to false,
     * - the maximum level is reset to zero,
     * - the maximum mesh width is reset to zero, and
     * - the minimum mesh width is reset to the maximum a double can hold.
     *
     * Throughout the traversal, the individual fields are updated. In
     * endIteration(), they contain valid values.
     */
    void resetStateAtBeginOfIteration();

    /**
     *
     * !!! Forking
     *
     * It can happen that throughout a fork that the grid did not change anymore.
     * The simplest example for this is a fork of rank 0 while the worker (rank
     * 1) doesn't refine anything. In this case, the data is deployed to the
     * worker. It is deployed before any vertex update on rank 0 has happened.
     * Consequently, rank 0 remains stationary. However, the new worker has
     * changed its grid in any case. So, the local state may indicate that
     * nothing has changed if and only if the state reduction is switched off.
     *
     * Nevertheless, if the grid is very coarse (only @f$ 3^d @f$), it can happen
     * that the grid remains stationary even though a fork has happened.
     */
    void resetStateAtEndOfIteration();

    #ifdef Parallel
    /**
     * Blocking send. initDatatype() has to be called before.
     *
     * This operation basically transfers the state's parallelise arguments
     * bit-by-bit to the master. However, the WorkerHasWorkerFlag might be
     * changed throughout the call (not locally).
     */
    void send(int destination, int tag);

    /**
     * Blocking receive. initDatatype() has to be called before.
     *
     * This operation always exchanges all data, i.e. not only the attributes
     * marked with parallelise. Please note that the attribute
     * _loadRebalancingRemoteRanks is a local attribute to this class and not
     * to the type modelled due to DaStGen. Hence, this container remains
     * unaltered. In contrast, the load balancing flag is modelled by DaStGen
     * and normally would be overwritten by the received state. As it is the
     * only flag that is not overwritten by the master's data, we preserve it
     * manually.
     */
    void receive(int source, int tag);

     void joinWithRank( int rank );
     void splitIntoRank( int rank );
     bool isForkTriggered() const;
     bool isForking() const;

     bool isInvolvedInJoinOrFork() const;

     bool isJoinWithMasterTriggered() const;
     bool isJoiningWithMaster() const;

     bool isJoiningWithWorker() const;

     bool isForkTriggeredForRank(int rank) const;

     bool isJoinTriggeredForRank(int rank) const;

     bool hasJoinedWithMaster() const;

     bool isForkingRank(int rank) const;
     bool isJoiningRank(int rank) const;

     bool isNewWorkerDueToForkOfExistingDomain() const;

     void setIsNewWorkerDueToForkOfExistingDomain(bool value);

    std::set<int> getForkingOrJoiningOrTriggeredForRebalancingRanks() const;

    /**
     * This operation should not be called by any user. It was created for the
     * repositories and the kernel only. If you want to switch off the global 
     * reduction, use the repository's iterate() method and set the argument to 
     * false.
     */
    void setReduceStateAndCell( bool value );

    /**
     * Shall we reduce data after the traversal or is the reduction switched off?
     *
     * The result of this attribute depends first of all on the internal flag
     * telling the state whether to reduce or not. Even if the state is asked
     * to reduce, this operation however might return false. This is the case
     * if the state is set to joining or if the state belongs to a node that
     * just is about to be forked.
     */
    bool reduceStateAndCell() const;

    /**
     * Has a worker
     *
     * In principle, the MPI oracle keeps track of which node has workers and
     * which one hasn't, i.e. there we can ask whether our worker employs other
     * workers or not. This information is propated bottom-up due to the state
     * and the state's infromation is set by send(), i.e. there is no setter as
     * send() sets the attribute and the receiving operation (see
     * Node::updateCellsParallelStateBeforeStore()) reads the attribute and
     * passes it on to the oracle.
     *
     * @see send()
     */
    bool hasWorker() const;

    bool mayForkDueToLoadBalancing(int workerRank) const;
    bool mayJoinDueToLoadBalancing(int workerRank) const;

    /**
     * Has to be called if you restart a repository, i.e. if you receive the
     * responsibility for a new subpartition.
     */
    void restart();
    #else
    bool isInvolvedInJoinOrFork() const { return false; }
    
    void restart();
    #endif
};


template <class StateData>
std::ostream& operator<<(std::ostream& out, const peano::grid::State<StateData>& properties);


#include "peano/grid/State.cpph"

#endif
