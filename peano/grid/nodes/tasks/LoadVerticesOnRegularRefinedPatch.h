// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_TASKS_LOAD_VERTICES_ON_REGULAR_REFINED_PATCH_H_
#define _PEANO_GRID_NODES_TASKS_LOAD_VERTICES_ON_REGULAR_REFINED_PATCH_H_


#include "tarch/logging/Log.h"
#include "tarch/multicore/BooleanSemaphore.h"

#include "peano/utils/Globals.h"
#include "peano/grid/UnrolledLevelEnumerator.h"
#include "peano/grid/RegularGridContainer.h"

#include <bitset>


namespace peano {
  namespace grid {
    namespace nodes {
      namespace tasks {
        template <class Vertex, class Cell, class VertexStack>
        class LoadVerticesOnRegularRefinedPatch;

        /**
         * Can be true if and only
         *
         * - if vertexPosition is at patch boundary and
         * - if a fork has happened,
         * - i.e. if the vertex is located at an untouched face.
         */
        bool isVertexOnLevel1DataDecompositionBoundaryInRegularTree(
          tarch::la::Vector<DIMENSIONS,int>             vertexPosition,
          const peano::grid::UnrolledLevelEnumerator&   cellsVertexEnumerator,
          const std::bitset<THREE_POWER_D>              forkedSubtrees
        );
      }
    }
  }
}


/**
 * Load Vertices on Regular Refined Subtree
 *
 * This operation loads vertices into the regular grid container. It may run
 * in parallel to the load cell task and the events (see RegularRefined).
 *
 *
 * !!! Runtime Behaviour if Load Process Does Not Fork Again
 *
 * In the standard spacetree, I may not load the vertices in parallel to the
 * cells, as I need the cell data to know where to load the vertices from.
 * However, I do not need the whole vertices but only the traversal order and
 * the odd/even flags, so what I do is: I create these data on-the-fly in the
 * load-vertex operation once more. This implementation variant yields four
 * different possible realisations. We can parallelise and we can decide
 * whether to create the cell data for the vertex load process on-the-fly or
 * to take this data from cells loaded before. The four variants come along
 * with different runtime characteristics (four cores):
 *
||                                        || parallel || serial
|| read loaded cells from grid container  |      311  |  214s
|| regenerate cell information on-the-fly |      296  |  224s
 *
 * Obviously, it is important to regenerate the cells on-the-fly if we run
 * the load vertices and load cells process in parallel (the parallel version
 * here is slower than the serial one; a fact discussed below). It is, in
 * contrast, important to use the already loaded cells if we run the code
 * serially.
 *
 * @author Tobias Weinzierl
 */
template <class Vertex, class Cell, class VertexStack>
class peano::grid::nodes::tasks::LoadVerticesOnRegularRefinedPatch {
  private:
    static tarch::logging::Log  _log;

    static tarch::multicore::BooleanSemaphore                         _vertexCounterSemaphore;

    const bool                                                        _isTraversalInverted;
    peano::grid::RegularGridContainer<Vertex,Cell>&                   _regularGridContainer;
    VertexStack&                                                      _vertexStack;

    const bool                                                        _loadProcessRunsInParallelToOtherTasks;
    int                                                               _maxLevelToFork;

    typename VertexStack::PopBlockVertexStackView                     _stackView;

    /**
     * Coarsest level of this task
     *
     * It is a relative quantity, i.e. relative to the regular tree's root.
     */
    const int                                                         _coarsestLevelOfThisTask;

    /**
     * If _coarsestLevelOfThisTask equals 0, this flag has to be zero, too.
     * Otherwise, it says where to take the coarsest cell from in the regular
     * grid container.
     */
    const tarch::la::Vector<DIMENSIONS,int>                           _coarsestCellsOffset;

    /**
     * Stores the number of reads per level, i.e.~how many @f$ 3^d @f$ patches
     * on each level have been handled by this task.
     */
    std::vector<int>                                                  _trackNumberOfReadsPerLevel;

    /**
     * Mark all the subtrees on level 1 that are handled by other threads. The
     * subtrees are enumerated lexicographically.
     */
    std::bitset<THREE_POWER_D>                                        _forkedSubtree;

    /**
     * Load Vertices and Get Where-to-load-from Information from On-the-fly Generated Cells
     *
     * Peano's built-in refinement mechanism for cells resets the refined
     * cell's state to refined. I'm using this mechanism to construct the
     * refined cells on-the-fly. As a consequence, I have to work with a
     * cell copy (at least not with a const reference). Also, I have to
     * manually reset the cell's state to leaf.
     *
     * You may/shall use this variant if and only if task parallelism is
     * switched on an no further decomposition of this task is allowed.
     *
     * As we are working with on-the-fly generated cells, i.e. we are not
     * working with any originals, we may not use the analysed information
     * whether temporary stacks are used, how many load operations have been
     * done, and so forth.
     *
     * @param coarseGridCell Copy of a coarse cell, i.e. never the original
     *                        one stored in the tree data structure, as the
     *                        operation modifies this record.
     */
    void loadVerticesWithOnTheFlyCellReconstruction(
      Cell&                                     coarseGridCell,
      const int                                 currentLevelOfCoarseCell,
      const tarch::la::Vector<DIMENSIONS,int>&  currentCoarseCellPositionWithinUnrolledPatch
    );

    /**
     * Load Vertices of a Regular Refined Subtree
     *
     * Different to the other operation variant, this operation works on the
     * loaded cells to find out from which stack which vertex is to be taken
     * from. As a result, we need access to the grid container's cells and we
     * need a semaphore that ensures that a level's cells already are loaded
     * before we proceed.
     *
     * Different to the other load operation, we work with a reference to
     * coarseCell.
     */
    void loadSubVerticesWithCellsFromGridContainer(
      const int                                 currentLevelOfCoarseCell,
      const tarch::la::Vector<DIMENSIONS,int>&  currentCoarseCellPositionWithinUnrolledPatch
    );

    /**
     * Handle cells/vertices of first level
     *
     * The original approach of the parallel load was planning to fork solely
     * cells inside the spacetree, i.e. no cells adjacent to the boundary of
     * the regular subtree. For such cells, we can be sure that no temporary
     * stacks are involved.
     *
     * However, this approach does not scale very good. Due to the depth-first
     * paradigm (we cannot fork level-wisely as we are opening views taking into
     * account the current stack position), there's first lots of depth
     * traversals going on, before we can fork any tree. Then, the first forks
     * are done on rather fine levels and not on the coarsest levels.
     *
     * I thus decided to handle the first level of the tree separately. If this
     * one may be forked, i.e. if any forks shall happen at all, I open a stack
     * view on the root process, fork this central element of the @f$ 3^d @f$
     * patch, handle depth-first the view, and then continue with the stack.
     * In parallel, the central element will fork further and perhaps fill the
     * other cores with additional workload.
     */
    void loadSubVerticesWithCellsOnFirstLevelInSharedMemoryMode();

    /**
     * !!! Implementation
     *
     * We explicitly have to copy the vertex enumerator, as we do manipulate
     * its offset. Multiple threads might work on the same level and modify the
     * offset, so we should copy explicitly.
     *
     * @param cellsPositionWithinUnrolledTreeLevel That is the fine grid cell, i.e. currentCell's, position
     */
    void loadVerticesOfOneCellWithinRegularSubtree(
      const Cell&                               currentCell,
      const tarch::la::Vector<DIMENSIONS,int>&  cellsPositionWithinUnrolledTreeLevel,
      const int                                 currentLevel
    );


    void loadVerticesOfOneCellAtBoundaryofSubtree(
      const Cell&                               currentCell,
      const tarch::la::Vector<DIMENSIONS,int>&  cellsPositionWithinUnrolledTreeLevel,
      const int                                 currentLevel
    );

    /**
     * Constructor for task splitting
     *
     * Exclusively for forks. A task of for level L means that the task shall
     * load all the @f$ 3^d @f$ cells on level L. Consequently, currentLevel
     * has to be equal to L-2 (it will be incremented in the functor
     * operation), and coarse cell has to be the cell from level L-1, i.e. the
     * father of the @f$ 3^d @f$ cells to load.
     */
    LoadVerticesOnRegularRefinedPatch(
      const bool                                                        isTraversalInverted,
      peano::grid::RegularGridContainer<Vertex,Cell>&                   regularGridContainer,
      VertexStack&                                                      vertexStack,
      bool                                                              loadProcessRunsInParallelToOtherTasks,
      int                                                               maxLevelToFork,
      const int                                                         currentLevel,
      const tarch::la::Vector<DIMENSIONS,int>&                          offsetWithinPatch,
      typename VertexStack::PopBlockVertexStackView                     stackView,
      const std::bitset<THREE_POWER_D>&                                 forkedSubtree
   );
  public:
    /**
     *
     * @param maxNumberOfForks The oracles do not support recursive analysis,
     *                         i.e. we have to determine once a priori, how
     *                         often we may split up a tree load process.
     */
    LoadVerticesOnRegularRefinedPatch(
      const bool                                                        isTraversalInverted,
      peano::grid::RegularGridContainer<Vertex,Cell>&                   regularGridContainer,
      VertexStack&                                                      vertexStack,
      bool                                                              loadProcessRunsInParallelToOtherTasks,
      int                                                               maxLevelToFork
    );

    ~LoadVerticesOnRegularRefinedPatch();

    void operator()();
};


#include "peano/grid/nodes/tasks/LoadVerticesOnRegularRefinedPatch.cpph"


#endif
