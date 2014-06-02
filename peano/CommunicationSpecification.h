// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_COMMUNICATION_SPECIFICATION_H_
#define _PEANO_COMMUNICATION_SPECIFICATION_H_

#include <string>


namespace peano {
  struct CommunicationSpecification;
}



/**
 * Specification of communication behaviour of a mapping
 *
 * Peano applies a clear broadcast/reduction paradigm in principle. Prior to
 * the actual traversal on any rank, the state is broadcasted, vertices and
 * cells are exchanged and then every rank runs. In return, the vertices and
 * cells from a rank are reduced when this rank has finished.
 *
 * This has a fundamental drawback: No rank can start to work before it has
 * received the master's state and its vertices. And it can send back the state
 * and the vertices not before the last touchVertexLastTime() has been called.
 * Unfortunately, each local tree is embedded into 3^d-1 other trees to preserve
 * the global Peano order on all grid entities, i.e. the worker receives data,
 * then runs through a part of this ghost layer, runs through the local partition,
 * through the remaining ghost cells, and then sends state, vertices, and cell
 * data up to the master again. Otherwise, even simple things such as the count
 * of the number of vertices would become invalid. However, sometimes it is
 * speed you need while you can live with inaccurate results. This specification
 * allows you to indicate this.
 *
 * This is important in particular if you perform several operations in a row.
 * In iterative solvers, e.g. There, global reduction typically is not required
 * in each individual step though you might require reduction in the tree to
 * couple different grid hierarchies.
 *
 * !!! exchangeMasterWorkerData
 *
 * If you set BeforeDescendIntoLocalSubtree, beginIteration() and multiple
 * touchVertexFirstTime() are called before the state actually is received, as
 * the state is received before the rank descends into its local partition not
 * before it starts to traverse the data structures.
 *
 * !!! exchangeWorkerMasterData
 *
 * Typically, all data of the coarsest level are sent back to the master after
 * the very last touchVertexLastTime() call. If you do not depend on this
 * touchVertexLastTime(), you may decide that the cell and the 2^d vertices on
 * the coarsest level are sent back immediately when all local cells of a rank
 * have been processed.
 *
 * !! exchangeStateAsPreamblePostamble
 *
 * If the flags for the vertex and cell data are set appropriately, one might
 * have the idea to send the state to the worker also only prior to the
 * traversal of local data and send it back immediately when all local cells
 * have been processed. In this case, all the statistics about vertices and
 * spacetree nodes however become wrong, as the state it sent 'too late' to the
 * worker and 'too early' up again. It works only anyway if no lb is done.
 *
 * However, if it is speed that matters, you might decide to life with these
 * corrumpted states.
 *
 * !!! Reduction
 *
 * Please note that the reduction also interplays with the specification of the
 * worker-master communication. If the reduction is switched off, the
 * specification is ignored.
 */
struct peano::CommunicationSpecification {
  enum ExchangeMasterWorkerData {
    BeforeFirstTouchVertexFirstTime, BeforeDescendIntoLocalSubtree
  };

  enum ExchangeWorkerMasterData {
    AfterLastTouchVertexLastTime, AfterProcessingOfLocalSubtree
  };

  bool exchangeStateAsPreamblePostamble;

  ExchangeMasterWorkerData  exchangeMasterWorkerData;
  ExchangeWorkerMasterData  exchangeWorkerMasterData;

  static CommunicationSpecification getMinimalSpecification();

  /**
   * @param exchangeMasterWorkerData
   * @param exchangeWorkerMasterData
   * @param exchangeStateAsPreamblePostamble
   */
  CommunicationSpecification( ExchangeMasterWorkerData  exchangeMasterWorkerData_, ExchangeWorkerMasterData  exchangeWorkerMasterData_, bool exchangeStateAsPreamblePostamble_ );

  bool sendStateBackToMasterAtEndOfTraversal() const;

  bool receiveDataFromMasterPriorToTraversal(bool stateMayUseLazyStateAndDataReceives) const;
  bool receiveStateFromMasterPriorToTraversal(bool stateMayUseLazyStateAndDataReceives) const;

  std::string toString() const;
};



/**
 * Combine two specifications.
 */
peano::CommunicationSpecification operator&(const peano::CommunicationSpecification& lhs, const peano::CommunicationSpecification& rhs);

#endif
