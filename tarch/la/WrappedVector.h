// Copyright (C) 2009 Technische Universitaet Muenchen
// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www5.in.tum.de/peano
#ifndef _TARCH_LA_WRAPPEDVECTOR_H_
#define _TARCH_LA_WRAPPEDVECTOR_H_

#include "tarch/la/VectorAssign.h"
#include "tarch/la/VectorAssignList.h"
#include "tarch/la/VectorOperations.h"
#include "tarch/la/VectorScalarOperations.h"
#include "tarch/la/VectorVectorOperations.h"
#include "tarch/utils/EnableIf.h"
#include "tarch/la/traits/IsVector.h"

namespace tarch {
  namespace la {

    /**
     * Zero-overhead static-sized wrapping for raw arrays and slicing of vectors.
     */
    template<int Size, typename Scalar>
    class WrappedVector
    {
    private:

      /**
       * Disallow copy construction from another WrappedVector.
       */
      WrappedVector(const WrappedVector<Size,Scalar>& toCopy);

    public:

      /**
       * Returns the number of elements in the vector.
       */
      int size () const;

      /**
       * Returns a reference to the element at index.
       */
      Scalar& operator[](int index);

      /**
       * Returns a const reference to the element at index.
       */
      const Scalar& operator[](int index) const;

      /**
       * Assigns the element values of another WrapppedVector to this vector.
       */
      WrappedVector<Size,Scalar>& operator= (const WrappedVector<Size,Scalar>& toAssign);

      /**
       * Assigns the element values of another vector to this vector.
       */
      template<typename Vector>
        typename utils::EnableIf<IsVector<Vector>::value,
        WrappedVector<Size,Scalar>&
      >::Type operator= (const Vector& toAssign);
    };

    /**
     * Returns a wrapped raw array, which is fully compatible to the la-functionality.
     */
    template<int Size, typename Scalar>
    WrappedVector<Size,Scalar>& wrap ( Scalar* toWrap );

    template<int Size, typename Scalar>
    const WrappedVector<Size,Scalar>& wrap ( const Scalar* toWrap );

    /**
     * Returns a WrappedVector, that references Size elements of toSlice, the
     * first element being at fromIndex.
     */
    template<int Size, typename Vector>
      typename utils::LazyEnableIf<IsVector<Vector>::value,
      utils::LazyType<WrappedVector<Size,typename VectorTraits<Vector>::Scalar>&>
    >::Type slice (Vector& toSlice, int fromIndex);

    template<int Size, typename Vector>
      typename utils::LazyEnableIf<IsVector<Vector>::value,
      utils::LazyType<const WrappedVector<Size,typename VectorTraits<Vector>::Scalar>&>
    >::Type slice (const Vector& toSlice, int fromIndex);

  } // namespace la
} // namespace tarch

#include "WrappedVector.cpph"

#endif /* _TARCH_LA_WRAPPEDVECTOR_H_ */
