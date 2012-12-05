#include "peano/grid/nodes/RegularRefined.h"


int peano::grid::nodes::transformOracleResult( int oracleResult, int TreeDepth, int NumberOfVerticesInSubtree ) {
  assertion1(oracleResult>=0,oracleResult);

  int result;
  if (oracleResult==0) {
    result = -1;
  }
  else {
    result = 0;
    while ( result<=TreeDepth && oracleResult < NumberOfVerticesInSubtree ) {
      result++;
      NumberOfVerticesInSubtree /= THREE_POWER_D;
    }
  }

  return result;
}


void peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridVertexPosition(
  const tarch::la::Vector<DIMENSIONS,int>&  fineGridPosition,
  tarch::la::Vector<DIMENSIONS,int>&        offsetOfCoarseGridEnumerator,
  tarch::la::Vector<DIMENSIONS,int>&        fineGridPositionRelativeToCoarserCell
) {
  static tarch::logging::Log _log( "peano::grid::nodes::RegularRefined" );

  logTraceInWith1Argument( "computePositionRelativeToNextCoarserLevel(...)", fineGridPosition );
  for (int d=0; d<DIMENSIONS; d++) {
    offsetOfCoarseGridEnumerator(d)           = fineGridPosition(d) >= 3 ? (fineGridPosition(d)-1)/3 : fineGridPosition(d)/3;
    fineGridPositionRelativeToCoarserCell(d)  = fineGridPosition(d)-offsetOfCoarseGridEnumerator(d)*3;
  }
  logTraceOutWith2Arguments( "computePositionRelativeToNextCoarserLevel(...)", offsetOfCoarseGridEnumerator, fineGridPositionRelativeToCoarserCell );
}


void peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridCellPosition(
  const tarch::la::Vector<DIMENSIONS,int>&  fineGridPosition,
  tarch::la::Vector<DIMENSIONS,int>&        offsetOfCoarseGridEnumerator,
  tarch::la::Vector<DIMENSIONS,int>&        fineGridPositionRelativeToCoarserCell
) {
  static tarch::logging::Log _log( "peano::grid::nodes::RegularRefined" );

  logTraceInWith1Argument( "computePositionRelativeToNextCoarserLevel(...)", fineGridPosition );
  for (int d=0; d<DIMENSIONS; d++) {
    offsetOfCoarseGridEnumerator(d)           = fineGridPosition(d)/3;
    fineGridPositionRelativeToCoarserCell(d)  = fineGridPosition(d)-offsetOfCoarseGridEnumerator(d)*3;
  }
  logTraceOutWith2Arguments( "computePositionRelativeToNextCoarserLevel(...)", offsetOfCoarseGridEnumerator, fineGridPositionRelativeToCoarserCell );
}
