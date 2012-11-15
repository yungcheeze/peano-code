// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_LOOPS_LOAD_VERTEX_LOOP_BODY_H_
#define _PEANO_GRID_NODES_LOOPS_LOAD_VERTEX_LOOP_BODY_H_


#include "tarch/logging/Log.h"
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/MulticoreDefinitions.h"

#include "peano/utils/Globals.h"
#include "peano/datatraversal/Action.h"
#include "peano/geometry/Geometry.h"


namespace peano {
    namespace grid {
      namespace nodes {
        namespace loops {
        template <class Vertex, class Cell, class State, class VertexStack, class EventHandle, int CellFlagsFromEnumerator>
          class LoadVertexLoopBody;
        }
    }
  }
}




/**
 * Load Loop Body
 *
 * This loop body is responsible to fetch data from the vertex containers and
 * write them to the Cartesian grid data structure. See Grid::loadVertices()
 * for a detailed description of the loop body's semantics.
 *
 * @author Tobias Weinzierl
 */
template <class Vertex, class Cell, class State, class VertexStack, class EventHandle, int CellFlagsFromEnumerator>
class peano::spacetreegrid::nodes::loops::LoadVertexLoopBody {
  private:
    static tarch::logging::Log _log;

    static tarch::multicore::BooleanSemaphore  _semaphore;

    State&                                    _state;
    Vertex*                                   _fineGridVertices;
    const SingleLevelEnumerator&              _fineGridVerticesEnumerator;
    Cell&                                     _coarseGridCell;
    Vertex * const                            _coarseGridVertices;
    const SingleLevelEnumerator&              _coarseGridVerticesEnumerator;
    const tarch::la::Vector<DIMENSIONS,int>&  _fineGridPositionOfCell;
    int*                                      _counter;
    VertexStack&                              _vertexStack;

#if defined(SharedMemoryParallelisation)
    EventHandle&                                                _eventHandle;
    EventHandle                                                 _threadLocalEventHandle;
#else
    EventHandle&                                                _eventHandle;
    EventHandle&                                                _threadLocalEventHandle;
#endif

    /**
     * Usually, the geometry should be const. See getters of geometry for a
     * description.
     */
    peano::geometry::Geometry&                _geometry;


    double _numberOfInnerVertices;
    double _numberOfBoundaryVertices;
    double _numberOfOuterVertices;
    bool   _hasRefinened;
    bool   _hasErased;
    bool   _hasChangedVertexState;

    void loadVertexFromInputStream(int positionInArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell);
    void createHangingNode(int positionInArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell);

    /**
     *
     * Invokes switchRefinementTriggeredToRefining() and
     * switchCoarseningTriggeredToCoarsening() on the loaded vertex.
     *
     * !!! Thread-safety
     *
     * This operation is invoked for vertices fetched from the input stack.
     * The access to the input stack is already serialised, i.e. there's
     * never two simultaneous accesses to the input stack. Consequently, this
     * operation also is serialised and there's no need to make it thread-safe
     * explicitly.
     *
     * !!! Parallel Computing
     *
     * The create-new-vertex operation can do an immediate refinement, i.e. it
     * can switch a vertex with refinement-triggered into a refining vertex
     * immediately. This way, we can create whole grids in one pass instead of
     * running through the grid for each additional level. However, if the
     * vertex belongs to the parallel boundary, we may not create the new
     * levels immediately. If we did, it might happen that one node refines
     * immediately as it decides only locally to fine, and then the grids are
     * inconsistent. So, for these nodes, we do not update immediately but wait
     * one iteration: Only one level per traversal is refined along parallel
     * boundaries. This way, we ensure that grids along parallel boundaries are
     * consistent.
     */
    void updateRefinementFlagsOfVertexAfterLoad(int positionInArray, bool counterIsDelete);

    /**
     *  Create New Vertex
     *
     *  Creates a new vertex, switch it to non-hanging, and analyse its
     *  adjacency flags (in the parallel mode). This operation does not
     *  invoke the create vertex event as it creates an outer vertex.
     *  Later, the vertex's state will be analysed by
     *  updateGeometryAndInvokeLoadVertexEvents() and this operation will
     *  call the create event, i.e. the vertex creation process is the same as
     *  the process for moving geometries: An outer vertex just becomes an
     *  inner/boundary vertex.
     */
    void createNewVertex(
      int                                       positionInArray,
      const tarch::la::Vector<DIMENSIONS,int>&  positionInLocalCell
    );

    /**
     * Updates geoemtry information
     *
     * Asks the scenario server whether the geometry has changed. If it has
     * changed, the operation updates the vertex's state.
     *
     * !!! Parallelisation
     *
     * If the code is running in parallel, these checks are performed
     * exclusively for local vertices, i.e. vertices of the shadow layer
     * that are handled by remote ranks are not taken into account.
     *
     * Due to the definition of remote (see Vertex::isRemote()) a vertex
     * is still local even if all adjacent ranks are right now set to
     * forking. This might occur strange but it is necessary, as the
     * vertex might exist on different nodes. If a local node does fork,
     * the other ranks are not yet aware of this fork. So, here it is: We
     * also have to update remote ranks.
     */
    void updateGeometry(int positionInArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell);

    /**
     *
     * !!! Parallelisation
     *
     * See remarks for updateGeometry() for further information.
     */
    void invokeLoadVertexEvents(int positionInArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell);

    /**
     * In debug mode, this operation compares the vertex's position and level
     * to the cell data.
     */
    void validateVertexInArray( int positionInVertexArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell ) const;

    void invalidateVertexIfGeometryHasChanged( int positionInVertexArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell ) const;
  public:
    LoadVertexLoopBody(
      State&                                    state,
      Vertex*                                   fineGridVertices,
      const SingleLevelEnumerator&              fineGridVerticesEnumerator,
      Cell&                                     coarseGridCell,
      Vertex * const                            coarseGridVertices,
      const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell,
      int*                                      counter,
      VertexStack&                              vertexStack,
      EventHandle&                              eventHandle,
      peano::geometry::Geometry&                geometry
    );

    ~LoadVertexLoopBody();

    /**
     * Load a Vertex
     *
     *
     *
|| Stack \\ Counter || persistent                        || hanging                           || new                               || delete                            || none
|| InOut            |  load(-1)                          |  create hanging node               |  create persistent node            |  load(-1)                          |  nop (vertex is already loaded)
||                  |  counter->CounterNodeAlreadyLoaded |  counter->CounterNodeAlreadyLoaded |  counter->CounterNodeAlreadyLoaded |  counter->CounterNodeAlreadyLoaded |  inc counter
|| Temporary        |  load(stack number)                |  load(stack number)                |  load(stack number)                |  load(stack number)                |  nop (vertex is already loaded)
     */
    void operator() (const peano::datatraversal::Action& action);
};


#include "peano/kernel/spacetreegrid/nodes/loops/LoadVertexLoopBody.cpph"


#endif
