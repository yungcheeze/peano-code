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
 */
#if defined(PackedRecords) && !defined(CompilerCLX)
  #ifndef noParallelExchangePackedRecordsAtBoundary
    #define ParallelExchangePackedRecordsAtBoundary
  #endif
  #ifndef noParallelExchangePackedRecordsBetweenMasterAndWorker
    #define ParallelExchangePackedRecordsBetweenMasterAndWorker
  #endif
  #ifndef noParallelExchangePackedRecordsInHeaps
    #define ParallelExchangePackedRecordsInHeaps
  #endif
  #ifndef noParallelExchangePackedRecordsThroughoutJoinsAndForks
    #define ParallelExchangePackedRecordsThroughoutJoinsAndForks
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


#ifndef noDistinguishStationaryAndInstationarySubtrees
#define DistinguishStationaryAndInstationarySubtrees
#endif


/**
 * @see ActionSetTraversal
 */
#ifndef noCacheActionSets
#define CacheActionSets
#endif


/**
 * @see Grid::sendStateToMaster()
 */
//#define RestrictStateImmediately

/**
 * Switch on manual alignment of vectors
 */
// @todo raus
#define noUseManualAlignment

#ifdef noUseManualAlignment
  #if defined(VectorisationAlignment)
    #warning Specified VectorisationAlignment though manual alignment was switched off due to -DnoUseManualAlignment
  #endif
#elif !defined(VectorisationAlignment)
  #warning No alignment specified by compiler though UseManualAlignment is switched on
#endif

/*
#if defined(CompilerHasSSE) && defined(noUseManualAlignment)
  #warning Code is trying to use SSE without manual alignment of data structures
#endif
*/


/**
 * The regular grid container has three different realisations to handle
 * regular grids. You can pick one of them. In my experiments, the plain
 * arrays outperformed all other realisations despite the fact that the
 * std variant uses tbb's memory allocators. Might nevertheless be worth
 * a try to study different variants as well.
 *
 * Please select only one out of these three variants.
 */
#define RegularGridContainerUsesPlainArrays
//#define RegularGridContainerUsesRawArrays
//#define RegularGridContainerUsesSTDArrays

#endif
