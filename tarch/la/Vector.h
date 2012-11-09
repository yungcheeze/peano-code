// Copyright (C) 2009 Technische Universitaet Muenchen
// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www5.in.tum.de/peano
#ifndef _TARCH_LA_VECTOR_H_
#define _TARCH_LA_VECTOR_H_

#include "tarch/utils/EnableIf.h"
#include "tarch/la/traits/IsVector.h"
#include "tarch/la/VectorAssign.h"
#include "tarch/la/VectorAssignList.h"
#include "tarch/la/VectorAssignRange.h"
#include "tarch/la/VectorOperations.h"
#include "tarch/la/VectorScalarOperations.h"
#include "tarch/la/VectorVectorOperations.h"

namespace tarch {
namespace la {

/**
 * Compile-time sized vector.
 *
 * This class does not contain all methods required to operate with vectors. The
 * functionality is external and applies to all vector types. Please do not add
 * any functionality, which can be implemented externally, and hence applies to
 * all vector types.
 */
template<int Size, typename Scalar>
struct Vector
{
private:
  Scalar _values[Size];

public:
  Vector ();

  /**
   * Assignment operator for any vector type.
   */
  template<typename VECTOR>
    typename utils::EnableIf< IsVector<VECTOR>::value,
    Vector<Size,Scalar>&
  >::Type operator= (const VECTOR& toAssign);

  /**
   * Assignment operator for list of comma separated scalar values, that has to
   * match the number of components of the vector. Otherwise a runtime assertion
   * goes wrong.
   */
  VectorAssignList<Vector<Size,Scalar> > operator=(const Scalar& value);

  /**
   * Copy constructor to copy from any vector type.
   *
   * The only way to accomplish this with enable-if is to specify a second
   * dummy argument with default value, which is (hopefully) optimized away.
   */
  template<typename VECTOR>
  Vector (const VECTOR& toCopy,
          typename utils::EnableIf< IsVector<VECTOR>::value,void*>::Type = NULL);

  /**
   * Construct new vector and initialize all components with initialValue.
   */
  Vector (const Scalar& initialValue);

  /**
   * Construct new 2D vector and initialize components separately.
   *
   * Runtime assertion when dimension is not 2.
   */
  Vector (const Scalar& initialValue0,
          const Scalar& initialValue1);

  /**
   * Construct new 3D vector and initialize components separately.
   *
   * Runtime assertion when dimension is not 3.
   */
  Vector (const Scalar& initialValue0,
          const Scalar& initialValue1,
          const Scalar& initialValue2);

  /**
   * Construct new 4D vector and initialize components separately.
   *
   * Runtime assertion when dimension is not 4.
   */
  Vector (const Scalar& initialValue0,
          const Scalar& initialValue1,
          const Scalar& initialValue2,
          const Scalar& initialValue3);

  /**
   * Returns the number of components of the vector.
   */
  int size() const;

  /**
   * Returns read-only ref. to component of given index.
   */
  const Scalar & operator[] (int index) const;

  /**
   * Returns ref. to component of given index.
   */
  Scalar & operator[] (int index);

  /**
   * Returns read-only ref. to component of given index.
   */
  const Scalar & operator() (int index) const;

  /**
   * Returns ref. to component of given index.
   */
  Scalar & operator() (int index);

  // No more methods here? They are all generic free methods now!
  // method just for RandomSpherePacking
  Vector<Size, int> integer() const;
  Vector<Size, double> Double() const;
  static Vector<Size, int> integer(const Vector<Size, Scalar>&);
  static Vector<Size, double> Double(const Vector<Size, Scalar>&);

};

}} // namespace tarch, la

#include "Vector.cpph"

#endif /* _TARCH_LA_VECTOR_H_ */
