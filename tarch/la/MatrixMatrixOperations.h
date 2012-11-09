// Copyright (C) 2009 Technische Universitaet Muenchen
// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www5.in.tum.de/peano
#ifndef _TARCH_LA_MATRIXMATRIXOPERATIONS_H_
#define _TARCH_LA_MATRIXMATRIXOPERATIONS_H_

#include "tarch/la/traits/IsMatrix.h"
#include "tarch/la/traits/MatrixTraits.h"
#include "tarch/utils/EnableIf.h"
//#include "tarch/la/traits/EqualScalars.h"
#include "tarch/la/Scalar.h"

namespace tarch {
  namespace la {

    /**
     * Performs a matrix x matrix multiplication.
     *
     * The resulting matrix has to be created outside and given as a parameter.
     */
    template<typename LMatrix, typename RMatrix, typename ResultMatrix>
      typename utils::EnableIf<
      IsMatrix<LMatrix>::value && IsMatrix<RMatrix>::value && IsMatrix<ResultMatrix>::value,
      void
    >::Type multiply (
      const LMatrix& lMatrix,
      const RMatrix& rMatrix,
      ResultMatrix&  result);

    /**
     * Bitwise comparison of the components of two matrices on equality.
     */
    template<typename LMatrix, typename RMatrix>
      typename utils::EnableIf<
      IsMatrix<LMatrix>::value && IsMatrix<RMatrix>::value,
      bool
    >::Type operator== (
      const LMatrix& lMatrix,
      const RMatrix& rMatrix);

    /**
     * Compares to matrices on equality by means of a numerical accuracy.
     */
    template<typename LMatrix, typename RMatrix>
      typename utils::EnableIf<
      IsMatrix<LMatrix>::value && IsMatrix<RMatrix>::value /*&& EqualScalars<LMatrix,RMatrix>::value*/,
      bool
    >::Type equals (
      const LMatrix&                         lMatrix,
      const RMatrix&                         rMatrix,
      typename MatrixTraits<LMatrix>::Scalar tolerance = NUMERICAL_ZERO_DIFFERENCE
    );
    /**
     * Adds rVector to lVector.
     *
     * A temporary vector is created and copied to store return back the result.
     */
    template<typename LMatrix, typename RMatrix>
      typename utils::EnableIf<
      IsMatrix<LMatrix>::value && IsMatrix<RMatrix>::value,
      LMatrix
    >::Type operator+ (
      const LMatrix& lMatrix,
      const RMatrix& rMatrix
      );
    /**
     * Return Index of element which is not equals.
     *
     */
    template<typename LMatrix, typename RMatrix>
      typename utils::EnableIf<
      IsMatrix<LMatrix>::value && IsMatrix<RMatrix>::value,
      int
    >::Type equalsReturnIndex (
      const LMatrix& lMatrix,
      const RMatrix& rMatrix,
      typename MatrixTraits<LMatrix>::Scalar tolerance = NUMERICAL_ZERO_DIFFERENCE
      );

  }
} // namespace tarch, la

#include "tarch/la/MatrixMatrixOperations.cpph"

#endif /* _TARCH_LA_MATRIXMATRIXOPERATIONS_H_ */
