// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_TESTS_REGULAR_REFINED_TEST_H_
#define _PEANO_GRID_TESTS_REGULAR_REFINED_TEST_H_


#include "tarch/tests/TestCase.h"


namespace peano {
  namespace grid {
    namespace tests {
      class RegularRefinedTest;
    }
  }
}


class peano::grid::tests::RegularRefinedTest: public tarch::tests::TestCase {
  private:
    void test2DComputePositionRelativeToNextCoarserLevelFromFineGridVertexPosition();
    void test2DComputePositionRelativeToNextCoarserLevelFromFineGridCellPosition();
    void testIsVertexOnLevel1DataDecompositionBoundaryAlgorithm();
  public:
    RegularRefinedTest();

    virtual ~RegularRefinedTest();

    virtual void run();

    void virtual setUp();
};


#endif
