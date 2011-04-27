//-*- mode: c++ -*-
/// \file matrix_3.hh
/// \brief 3x3 matrix operations, mostly rotations.

#ifndef CGMATH_MATRIX_3_HH
#define CGMATH_MATRIX_3_HH

namespace cgmath
{
  typedef matrix_mn<float,3,3>  matrix_3f;
  typedef matrix_mn<double,3,3> matrix_3d;

  /// Make 3x3 matrix from scalars.
  template<typename scalar>
  matrix_mn<scalar,3,3> 
  mat3 (const scalar& x00, const scalar& x01, const scalar& x02,
	const scalar& x10, const scalar& x11, const scalar& x12,
	const scalar& x20, const scalar& x21, const scalar& x22)
  {
    matrix_mn<scalar,3,3> X;
    X(0,0) = x00;  X(0,1) = x01; X(0,2) = x02; 
    X(1,0) = x10;  X(1,1) = x11; X(1,2) = x12; 
    X(2,0) = x20;  X(2,1) = x21; X(2,2) = x22; 
    return X;
  }

  /// Make 3x3 matrix from column vectors.
  template<typename scalar>
  matrix_mn<scalar,3,3> mat_cols (const vector_n<scalar,3>& x0, 
				  const vector_n<scalar,3>& x1, 
				  const vector_n<scalar,3>& x2) 
  {
    matrix_mn<scalar,3,3> X;
    X(0,0) = x0(0);  X(0,1) = x1(0); X(0,2) = x2(0); 
    X(1,0) = x0(1);  X(1,1) = x1(1); X(1,2) = x2(1); 
    X(2,0) = x0(2);  X(2,1) = x1(2); X(2,2) = x2(2); 
    return X;
  }

  /// Make 3x3 matrix from row vectors.
  template<typename scalar>
  matrix_mn<scalar,3,3> mat_rows (const vector_n<scalar,3>& x0, 
				  const vector_n<scalar,3>& x1, 
				  const vector_n<scalar,3>& x2) 
  {
    matrix_mn<scalar,3,3> X;
    X(0,0) = x0(0);  X(1,0) = x1(0); X(2,0) = x2(0); 
    X(0,1) = x0(1);  X(1,1) = x1(1); X(2,1) = x2(1); 
    X(0,2) = x0(2);  X(1,2) = x1(2); X(2,2) = x2(2); 
    return X;
  }

  /// Make 3x3 matrix corresponding to cross product with v, [v]_x.
  template<typename scalar>
  matrix_mn<scalar,3,3> cross_matrix (const vector_n<scalar,3>& v)
  {
    scalar _0 = static_cast<scalar>(0);
    return mat3 (   _0, -v(2),  v(1),
		  v(2),    _0, -v(0),
		 -v(1),  v(0),   _0);
  }

  /// Rotation of theta radians around axis. Rodrigues formula.
  template<typename scalar>
  matrix_mn<scalar, 3, 3> rotate (scalar theta, vector_n<scalar,3> axis)
  {
    vector_n<scalar,3> u = normalized(axis);
    scalar c = static_cast<scalar>(std::cos(theta));
    scalar s = static_cast<scalar>(std::sin(theta));
    return c * matrix_mn<scalar,3,3>::eye()
      + s * cross_matrix(u) + (static_cast<scalar>(1) - c) * outer(u,u);
  }

  /// Return matrix to rotate one unit vector to another.
  template<typename scalar>
  matrix_mn<scalar, 3, 3> rotate (vector_n<scalar,3> u, 
				  vector_n<scalar,3> v)
  {
    vector_n<scalar,3> a = cross(u,v);
    if (normalize(a) < 1e-12)
      {
	// Check if no rotation or 180 degree rotation.
	// In case of 180 degrees find some axis to rotate.
	if (dot(u,v) > 0)
	  return matrix_mn<scalar, 3, 3>::identity();
	else if (std::abs(u(0)) > 1e-12 || std::abs(u(1)) > 1e-12)
	  a = normalized(vec<scalar>(-u(1), u(0), 0.0));
	else 
	  a = vec<scalar>(u(2), 0, 0);
      }
    vector_n<scalar,3> uxa = cross(u,a);
    vector_n<scalar,3> vxa = cross(v,a);
    return mat_cols (v, a, vxa) * mat_rows (u, a, uxa);
  }
 
}

#endif
