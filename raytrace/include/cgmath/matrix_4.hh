//-*- mode: c++ -*-
/// \file matrix_4.hh
/// \brief 4x4 matrix operations, mostly affine transforms.

#ifndef CGMATH_MATRIX_4_HH
#define CGMATH_MATRIX_4_HH

namespace cgmath
{
  typedef matrix_mn<float,4,4>  matrix_4f;
  typedef matrix_mn<double,4,4> matrix_4d;

  /// Make 4x4 matrix from scalars.
  template<typename scalar>
  matrix_mn<scalar,4,4> 
  mat4 (const scalar& x00, const scalar& x01, const scalar& x02, const scalar& x03,
	const scalar& x10, const scalar& x11, const scalar& x12, const scalar& x13,
	const scalar& x20, const scalar& x21, const scalar& x22, const scalar& x23,
	const scalar& x30, const scalar& x31, const scalar& x32, const scalar& x33)
  {
    matrix_mn<scalar,4,4> X;
    X(0,0) = x00;  X(0,1) = x01; X(0,2) = x02; X(0,3) = x03; 
    X(1,0) = x10;  X(1,1) = x11; X(1,2) = x12; X(1,3) = x13; 
    X(2,0) = x20;  X(2,1) = x21; X(2,2) = x22; X(2,3) = x23; 
    X(3,0) = x30;  X(3,1) = x31; X(3,2) = x32; X(3,3) = x33; 
    return X;
  }

  /// Make 4x4 affine transform matrix from linear part and translation.
  template<typename scalar>
  matrix_mn<scalar,4,4> affine_matrix (const matrix_mn<scalar, 3,3>& A,
				       const vector_n<scalar, 3>&    b)
  {
    return mat4<scalar>(A(0,0), A(0,1), A(0,2), b(0),
			A(1,0), A(1,1), A(1,2), b(1),
			A(2,0), A(2,1), A(2,2), b(2),
			     0,      0,      0,    1);
  }
				       

  /// Make matrix for scaling transform.
  template<typename scalar>
  matrix_mn<scalar,4,4> scale (const scalar& x, 
			       const scalar& y, 
			       const scalar& z)
  {
    return mat4<scalar> (x, 0, 0, 0,
			 0, y, 0, 0,
			 0, 0, z, 0,
			 0, 0, 0, 1);
  }

  /// Make matrix for translation.
  template<typename scalar>
  matrix_mn<scalar,4,4> translate (const scalar& x, 
				   const scalar& y, 
				   const scalar& z)
  {
    return mat4<scalar> (1, 0, 0, x,
			 0, 1, 0, y,
			 0, 0, 1, z,
			 0, 0, 0, 1);
  }

  /// Make matrix for translation from vector.
  template<typename scalar>
  matrix_mn<scalar,4,4> translate (const vector_n<scalar,3>& d)
  {
    return mat4<scalar> (1, 0, 0, d(0),
			 0, 1, 0, d(1),
			 0, 0, 1, d(2),
			 0, 0, 0, 1);
  }

  /// Make matrix for rotation around angle.
  template<typename scalar>
  matrix_mn<scalar,4,4> rotate_4 (const scalar& angle_rad, 
				  const scalar& x, 
				  const scalar& y, 
				  const scalar& z)
  {
    return affine_matrix (rotate(angle_rad, vec(x,y,z)), vec<scalar>(0,0,0));
  }

  /// Make matrix for rotation around angle.
  template<typename scalar>
  matrix_mn<scalar,4,4> rotate_4 (const scalar& angle_rad, 
				  const vector_n<scalar, 3>& a)
  {
    return affine_matrix (rotate(angle_rad, a), vec<scalar>(0,0,0));
  }
  
  /// Matrix for lookat transform (same as gluLookat)
  template<typename scalar>
  matrix_mn<scalar,4,4> lookat (const vector_n<scalar, 3>& eye,
				const vector_n<scalar, 3>& to,
				const vector_n<scalar, 3>& up)
  {
    vector_n<scalar, 3> z = normalized(eye-to);
    vector_n<scalar, 3> x = up ^ z;
    vector_n<scalar, 3> y = z ^ x;
    return mat4<scalar>(x(0), x(1), x(2), 0,
			y(0), y(1), y(2), 0,
			z(0), z(1), z(2), 0,
			0, 0, 0, 1) * translate(-eye);
  }

  /// Matrix for perspective projection (same as glFrustum)
  template<typename scalar>
  matrix_mn<scalar,4,4> frustum (const scalar& left,
				 const scalar& right, 
				 const scalar& bottom, 
				 const scalar& top,
				 const scalar& nearVal,
				 const scalar& farVal)
  {
    scalar A = (right + left) / (right - left);
    scalar B = (top + bottom) / (top - bottom);
    scalar C = -(farVal + nearVal) / (farVal - nearVal);
    scalar D = -static_cast<scalar>(2) * farVal * nearVal / (farVal - nearVal);
    scalar E = static_cast<scalar>(2) * nearVal / (right - left); 
    scalar F = static_cast<scalar>(2) * nearVal / (top - bottom); 
    return mat4<scalar> (E, 0,  A, 0,
			 0, F,  B, 0,
			 0, 0,  C, D,
			 0, 0, -1, 0);
  }


  /// Matrix for orthogonal projection (same as glOrtho)
  template<typename scalar>
  matrix_mn<scalar,4,4> ortho (const scalar& left,
			       const scalar& right, 
			       const scalar& bottom, 
			       const scalar& top,
			       const scalar& nearVal,
			       const scalar& farVal)
  {
    scalar tx = -(right + left) / (right - left);
    scalar ty = -(top + bottom) / (top - bottom);
    scalar tz = -(farVal + nearVal) / (farVal - nearVal);
    scalar A =  static_cast<scalar>(2) / (right - left); 
    scalar B =  static_cast<scalar>(2) / (top - bottom); 
    scalar C = -static_cast<scalar>(2) / (farVal - nearVal);
    return mat4<scalar> (A,  0,  0, tx,
			 0,  B,  0, ty,
			 0,  0,  C, tz,
			 0,  0,  0,  0);
  }

  /// Matrix for OpenGL window to viewport transform.
  template<typename scalar>
  matrix_mn<scalar,4,4> viewport (const scalar& x,
				  const scalar& y, 
				  const scalar& width,
				  const scalar& height)
  {
    scalar w2 = static_cast<scalar>(width  / 2.0);
    scalar h2 = static_cast<scalar>(height / 2.0);
    return mat4<scalar> (w2,  0,  0, x + w2,
			 0,  h2,  0, y + h2,
			 0,   0,  1,      0,
			 0,   0,  0,      1);
  }

  /// Matrix for perspective projection (same as gluPerspective, except in radians)
  template<typename scalar>
  matrix_mn<scalar,4,4> perspective (const scalar& fovy,
				     const scalar& aspect, 
				     const scalar& nearVal,
				     const scalar& farVal)
  {
    scalar f = 1.0/std::tan(fovy/2.0);
    scalar A = f / aspect;
    scalar B = f;
    scalar C = -(farVal + nearVal) / (farVal - nearVal);
    scalar D = -static_cast<scalar>(2) * farVal * nearVal / (farVal - nearVal);
    return mat4<scalar> (A, 0,  0, 0,
			 0, B,  0, 0,
			 0, 0,  C, D,
			 0, 0, -1, 0);
  }

  /// Use matrix as affine transform. (TODO: using top 3x4 is enough).
  template<typename scalar>
  vector_n<scalar,3> affine_transform (const matrix_mn<scalar, 4,4>& A,
				       const vector_n<scalar, 3>& x)
  {
    vector_n<scalar, 4> Ax4 = A * vec<scalar>(x(0), x(1), x(2), 1);
    return vec<scalar> (Ax4(0), Ax4(1), Ax4(2));
  }

  /// Use matrix as normal transform. (TODO: using top 3x4 is enough).
  /// Matrix should be inverse of original transform.
  template<typename scalar>
  vector_n<scalar,3> normal_transform (const matrix_mn<scalar, 4,4>& Ainv,
				       const vector_n<scalar, 3>&    n)
  {
    vector_n<scalar, 4> n4 = vec<scalar>(n(0), n(1), n(2), 0) * Ainv;
    return vec<scalar> (n4(0), n4(1), n4(2));
  }
  
  /// Extract linear and translation parts.
  template<typename scalar>
  void decompose_affine (const matrix_mn<scalar, 4,4>& T,
			 matrix_mn<scalar, 3,3>&       A,
			 vector_n<scalar, 3>&          b)
  {
    for (unsigned i = 0; i < 3; ++i)
      for (unsigned j = 0; j < 3; ++j)
	A(i,j) = T(i,j);
    for (unsigned i = 0; i < 3; ++i)
      b(i) = T(i,3);
  }

}
#endif
