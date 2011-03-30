// -*- mode: c++ -*-
/// \file vector_2.hh
/// \brief 2D vector operations.

#ifndef CGMATH_VECTOR_2_HH
#define CGMATH_VECTOR_2_HH

namespace cgmath
{
  typedef vector_n<float,2>  vector_2f;
  typedef vector_n<double,2> vector_2d;
  
  /// Create 2D vector from scalars.
  template<typename scalar>
  vector_n<scalar,2> vec (const scalar& x, const scalar& y)
  {    
    vector_n<scalar, 2> result;
    result(0) = x;
    result(1) = y;
    return result;
  }

  /// Rotation by 90 degrees, 2D "cross-product".
  template<typename scalar>
  vector_n<scalar,2> perp (const vector_n<scalar,2>& u)
  {
    return vec(-u(1), u(0));
  }

}

#endif
