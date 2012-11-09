// Copyright (C) 2009 Technische Universitaet Muenchen
// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www5.in.tum.de/peano
#ifndef _TARCH_LA_VECTOR_OPERATIONS_H_
#define _TARCH_LA_VECTOR_OPERATIONS_H_

#include "tarch/la/traits/IsVector.h"
#include "tarch/la/traits/VectorTraits.h"
#include "tarch/la/ScalarOperations.h"
#include "tarch/utils/EnableIf.h"
#include <sstream>
#include <cmath>

namespace tarch {
  namespace la {

    /**
     * Computes the 1-norm of the vector, i.e. it sums up abs. component values.
     */
    template<typename Vector>
      typename utils::LazyEnableIf< IsVector<Vector>::value,
      utils::LazyType<typename VectorTraits<Vector>::Scalar>
    >::Type norm1 (const Vector& vector);

    /**
     * Computes the 2-norm of the vector, i.e. it takes the square-root of
     * summed up squared component values.
     */
    template<typename Vector>
      typename utils::LazyEnableIf< IsVector<Vector>::value,
      utils::LazyType<typename VectorTraits<Vector>::Scalar>
    >::Type norm2 (const Vector& vector);

    /**
     * Computes the absolute component values of the vector, creating a
     * temporary vector to hold the result.
     *
     * @return A copy of the temporary holding the result.
     */
    template<typename Vector>
      typename utils::EnableIf< IsVector<Vector>::value,
      Vector
    >::Type abs (const Vector& vector);

    /**
     * Computes the absolute component values of the vector into result.
     *
     * The input and result vector can have different types. This enables to
     * compute the result into a faster created static vector type when using
     * a dynamic vector as input.
     *
     * @return the modified vector result.
     */
    template<typename VectorA, typename VectorB>
      typename utils::EnableIf< IsVector<VectorA>::value && IsVector<VectorB>::value,
      VectorB&
    >::Type abs (const VectorA& vector, VectorB& result);

    /**
     * Sums up the component values of the vector.
     */
    template<typename Vector>
      typename utils::LazyEnableIf< IsVector<Vector>::value,
      utils::LazyType<typename VectorTraits<Vector>::Scalar>
    >::Type sum (const Vector& vector);

    /**
     * Sums up the components of subvectors in vector into result.
     *
     * Assumes vector has size k*size(result), i.e. the components of vector
     * are a sequence of smaller vectors with length of result.
     */
    template<typename VectorA, typename VectorB>
      typename utils::EnableIf< IsVector<VectorA>::value && IsVector<VectorB>::value,
      VectorB&
    >::Type sumSubvectors (const VectorA& vector, VectorB& result);

    /**
     * Computes the volume of the tetrahedron spanned by the Cartesian unit vectors
     * scaled by the corresponding components of the given vector.
     */
    template<typename Vector>
      typename utils::LazyEnableIf< IsVector<Vector>::value,
      utils::LazyType<typename VectorTraits<Vector>::Scalar>
    >::Type volume (const Vector& vector);

    /**
     * Returns the index of the element with maximal value (NOT absolute value).
     */
    template<typename Vector>
      typename utils::EnableIf< IsVector<Vector>::value,
      int
    >::Type indexMax (const Vector& vector);

    /**
     * Returns the index of the element with minimal value (NOT absolute value).
     */
    template<typename Vector>
      typename utils::EnableIf< IsVector<Vector>::value,
      int
    >::Type indexMin (const Vector& vector);

    /**
     * Returns the element with maximal value (NOT absolute value).
     */
    template<typename Vector>
      typename utils::LazyEnableIf< IsVector<Vector>::value,
      utils::LazyType<typename VectorTraits<Vector>::Scalar>
    >::Type max (const Vector& vector);

    /**
     * Returns the element with minimal value (NOT absolute value).
     */
    template<typename Vector>
      typename utils::LazyEnableIf< IsVector<Vector>::value,
      utils::LazyType<typename VectorTraits<Vector>::Scalar>
    >::Type min (const Vector& vector);

    /**
     * Returns a pointer to the first element of the vector.
     */
    template<typename Vector>
      typename utils::LazyEnableIf< IsVector<Vector>::value,
      utils::LazyType<typename VectorTraits<Vector>::Scalar*>
    >::Type raw (Vector& vector);

    /**
     * Returns a const pointer to the first element of the vector.
     */
    template<typename Vector>
      typename utils::LazyEnableIf< IsVector<Vector>::value,
      utils::LazyType<const typename VectorTraits<Vector>::Scalar*>
    >::Type raw (const Vector& vector);

    /**
     * Computes the square root of every component of the vector.
     */
    template<typename Vector>
      typename utils::EnableIf< IsVector<Vector>::value,
      Vector
    >::Type sqrt (const Vector& vector);

    /**
     * Pipes the elements of a vector into a std::string and returns the string.
     */
    template<typename Vector>
      std::string toString (const Vector& vector);
  } // namespace la
} // namespace tarch

/**
 * Streams the component values into a comma separated representation.
 */
template<typename Vector>
  typename tarch::utils::EnableIf< tarch::la::IsVector<Vector>::value,
  std::ostream&
>::Type operator<< (std::ostream & os, const Vector & vector);

#include "VectorOperations.cpph"

#endif /* _TARCH_LA_VECTOR_OPERATIONS_H_ */
