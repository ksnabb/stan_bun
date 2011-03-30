// -*- mode: c++ -*-
/// \file vector_3.hh
/// \brief 3D vector operations.

#ifndef CGMATH_VECTOR_3_HH
#define CGMATH_VECTOR_3_HH

namespace cgmath
{
  typedef vector_n<float,3>  vector_3f;
  typedef vector_n<double,3> vector_3d;
  
  /// Create 3D vector from scalars.
  template<typename scalar>
  vector_n<scalar,3> vec (const scalar& x, 
			  const scalar& y, 
			  const scalar& z)
  {    
    vector_n<scalar, 3> result;
    result(0) = x;
    result(1) = y;
    result(2) = z;
    return result;
  }

  /// Cross product.
  template<typename scalar>
  vector_n<scalar,3> cross (const vector_n<scalar,3>& u,
			    const vector_n<scalar,3>& v)
  {    
    vector_n<scalar, 3> result;
    result(0) = u(1)*v(2) - u(2)*v(1);
    result(1) = u(2)*v(0) - u(0)*v(2);
    result(2) = u(0)*v(1) - u(1)*v(0);
    return result;
  }

  /// Cross product (physicists uses wedge, which looks like ^)
  template<typename scalar>
  vector_n<scalar,3> operator^ (const vector_n<scalar,3>& u,
				const vector_n<scalar,3>& v)
  {    
    return cross(u,v);
  }

  /// Scalar triple product u . v x w and 3 x 3 determinant |u v w|.
  template<typename scalar>
  scalar det (const vector_n<scalar,3>& u,
	      const vector_n<scalar,3>& v,
	      const vector_n<scalar,3>& w)
  {
    return u * (v ^ w);
  }
}

#endif
