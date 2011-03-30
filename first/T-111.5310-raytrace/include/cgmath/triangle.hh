// -*- mode: c++ -*-
/// \file triangle.hh
/// \brief Triangles.

#ifndef CGMATH_TRIANGLE_HH
#define CGMATH_TRIANGLE_HH

namespace cgmath
{
  /// Triangle
  template<typename scalar, unsigned n> 
  class triangle
  {
  public:
    /// Default constructor.
    triangle ()
    {}

    /// Copy constructor.
    triangle (const triangle<scalar,n>& T)
      : a(T.a), b(T.b), c(T.c)
    {}

    /// Construct from vertices.
    triangle (const vector_n<scalar,n>& _a,
	      const vector_n<scalar,n>& _b,
	      const vector_n<scalar,n>& _c)
      : a(_a), b(_b), c(_c)
    {}

    /// Scalar type conversion.
    template<typename scalar2>
    operator triangle<scalar2, n> () const
    {
      return triangle<scalar2, n> (static_cast< vector_n<scalar2, n> > (a),
				   static_cast< vector_n<scalar2, n> > (b),
				   static_cast< vector_n<scalar2, n> > (c));
    }

    // Area of triangle.
    double area (void) const
    {
      return 0.5 * norm(cross(b - a, c - a));
    }
    
    /// Centroid or mean of triangle
    vector_n<scalar,n> centroid (void) const
    {
      return static_cast<scalar>(1.0/3.0) * (a + b + c);
    }

    /// Covariance matrix of uniform triangular distribution.
    matrix_mn<scalar,n,n> cov (void) const
    {
      vector_n<scalar,n> u = b - a;
      vector_n<scalar,n> v = c - a;
      return static_cast<scalar>(1.0/36.0) * 
	(static_cast<scalar>(2) * outer(u,u)
	 - outer(u,v)
	 - outer(v,u)
	 + static_cast<scalar>(2) * outer(v,v));
    }
   
    vector_n<scalar,n> a; ///< first vertex of triangle
    vector_n<scalar,n> b; ///< second vertex of triangle
    vector_n<scalar,n> c; ///< third vertex of triangle
  };

  typedef triangle<float,  2> triangle_2f;
  typedef triangle<double, 2> triangle_2d;
  typedef triangle<float,  3> triangle_3f;
  typedef triangle<double, 3> triangle_3d;

  /// Normal of 3D triangle.
  template<typename scalar>
  vector_n<scalar, 3> normal (const triangle<scalar,3>& T)
  {
    return (T.b - T.a)^(T.c - T.a);
  }

  /// Plane equation of 3D triangle.
  template<typename scalar>
  vector_n<scalar, 4> plane (const triangle<scalar,3>& T)
  {
    vector_n<scalar,3> n = normal(T);
    return vec(n(0), n(1), n(2), -dot(n,T.a));
  }

  /// Transform triangle in affine transform.
  template<typename scalar>
  triangle<scalar,3> affine_transform (const matrix_mn<scalar, 4,4>& A,
				       const triangle<scalar, 3>& T)
  {
    // TODO: use 3x4 submatrix directly.
    vector_n<scalar, 4> Ta4 = A * vec<scalar>(T.a(0), T.a(1), T.a(2), 1);
    vector_n<scalar, 4> Tb4 = A * vec<scalar>(T.b(0), T.b(1), T.b(2), 1);
    vector_n<scalar, 4> Tc4 = A * vec<scalar>(T.c(0), T.c(1), T.c(2), 1);
    return triangle<scalar,3>(vec<scalar> (Ta4(0), Ta4(1), Ta4(2)),
			      vec<scalar> (Tb4(0), Tb4(1), Tb4(2)),
			      vec<scalar> (Tc4(0), Tc4(1), Tc4(2)));
  }
}

#endif
