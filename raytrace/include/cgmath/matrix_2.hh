// -*- mode: c++ -*-

#ifndef CGMATH_MATRIX_2_HH
#define CGMATH_MATRIX_2_HH

namespace cgmath
{
  typedef matrix_mn<float,2,2>  matrix_2f;
  typedef matrix_mn<double,2,2> matrix_2d;

  template<typename scalar>
  matrix_mn<scalar,2,2> 
  mat2 (const scalar& x00, const scalar& x01,
	const scalar& x10, const scalar& x11)
  {
    matrix_mn<scalar,2,2> X;
    X(0,0) = x00;  X(0,1) = x01;
    X(1,0) = x10;  X(1,1) = x11;
    return X;
  }
  
  template<typename scalar>
  matrix_mn<scalar, 2, 2> rotate (scalar theta)
  {
    scalar c = static_cast<scalar>(std::cos(theta));
    scalar s = static_cast<scalar>(std::sin(theta));
    return mat2 (c, -s, 
		 s,  c);
  }
}

#endif
