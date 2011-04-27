// -*- mode: c++ -*-
/// \file vector_4.hh
/// \brief 4D vector operations.

#ifndef CGMATH_VECTOR_4_HH
#define CGMATH_VECTOR_4_HH

namespace cgmath
{
  typedef vector_n<float,4>  vector_4f;
  typedef vector_n<double,4> vector_4d;

  /// Create 4D vector from scalars.
  template<typename scalar>
  vector_n<scalar,4> vec (const scalar& x, 
			  const scalar& y, 
			  const scalar& z,
			  const scalar& w)
  {    
    vector_n<scalar, 4> result;
    result(0) = x;
    result(1) = y;
    result(2) = z;
    result(3) = w;
    return result;
  }
  
  /// Create 4D vector from vector and scalar.
  template<typename scalar>
  vector_n<scalar,4> vec (vector_n<scalar,3>& x,
			  const scalar& w)
  {    
    vector_n<scalar, 4> result;
    result(0) = x(0);
    result(1) = x(1);
    result(2) = x(2);
    result(3) = w;
    return result;
  }

  /// Convert homogeneous to cartesian coordinates (divide by w).
  template<typename scalar>
  vector_n<scalar,3> cartesian (const vector_n<scalar,4> a)
  {    
    double _w = 1.0/a(3);
    return vec<scalar>(_w * a(0), _w * a(1), _w * a(2));
  }
}

#endif
