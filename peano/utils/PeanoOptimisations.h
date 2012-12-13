// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_UTILS_PEANO_OPTIMISATIONS
#define _PEANO_UTILS_PEANO_OPTIMISATIONS

#include "tarch/compiler/CompilerSpecificSettings.h"


/**
 * Uses lookup tables within the dfor statements. To enable this features dfors
 * have to be used for 2,3,4 by 2,3,4 arrays only. Otherwise, the lookup table
 * mechanism will fail. By standard, lookup tables are only switched on for the
 * xPowI functions, where x is 2,3 or 4.
 */
#ifndef noDloopOptimiseAggressive
#define DloopOptimiseAggressive
#endif


/**
 * Store only the part of the objects that is marked as persistent. Reduces
 * size (memory consumption) of the grid data types.
 */
#ifndef noStoreOnlyPersistentAttributes
#define StoreOnlyPersistentAttributes
#endif


/**
 * Peano data packing enabled: less data per node/cell
 * Only in very rare cases, this flag might have negative influence on runtime.
 * You may even use it in Debug mode, as it is no compiler optimisation flag.
 */
#ifndef noPackedRecords
#define PackedRecords
#endif


/**
 * Send and receive data in packed version
 *
 * In the parallel code, the user either exchanges the raw vertices and cells or
 * the packed variants of either. Depending on the architecture (especially
 * bandwidth and latency), one option might result in a significant better
 * performance. The Altix, e.g., is faster if this flag is not set, but the
 * bandwidth requirements rise then.
 *
 * However,
 *
 * The operation is not activated, if the symbol Packed is undefined.
 */
#if defined(PackedRecords) && !defined(CompilerCLX)
#ifndef noParallelExchangePackedRecords
#define ParallelExchangePackedRecords
#endif
#endif


/**
 * Switch recursion unrolling on or off
 *
 * This flag might speed up your code but introduces a slightly bigger memory
 * footprint. With a shared memory parallelisation, it furthermore might be
 * that the runtime savings vanish, as the underlying computations that are
 * saved in a lookup table can be computed in parallel anyway.
 */
#ifndef noUseRecursionUnrollingOnRegularPatches
#define UseRecursionUnrollingOnRegularPatches
#endif


/**
 * @see ActionSetTraversal
 */
#ifndef noCacheActionSets
#define CacheActionSets
#endif


/**
 * Switch on manual alignment of vectors
 * 
 * If this flag is switched on, you have to specify the value 
 * VectorisationAlignment which is usually 16 for SSE and 32 for 
 * AVX.
 */
#ifndef noManualAlignment
#define UseManualAlignment
#define VectorisationAlignment 16 // SSE
//#define VectorisationAlignment 32 // AVX
#endif


#ifndef noManualInlining
#define UseManualInlining
#endif



/**
 * There are two different variants to call enterCell() and leaveCell() in
 * parallel with TBBs on regular subtrees. Peano either can use parallel_reduce
 * or a manual unbalanced partitioning. See ActionSetTraversalLoop::ActionSetTraversalLoopWithBiPartitioning
 * for explanations. In general, Peano's manual traversal concept seems to
 * outperform parallel_reduce, so it is a good choice not to switch on this
 * flag.
 */
//#define UseParallelReduceForCellHandling

/**
 * Counterpart of UseParallelReduceForCellHandling. While most test cases
 * result in the fact that UseParallelReduceForCellHandling should be switched
 * off, Peano in most cases is faster, if this flag is turned on.
 */
#define UseParallelReduceForVertexHandling

#endif
