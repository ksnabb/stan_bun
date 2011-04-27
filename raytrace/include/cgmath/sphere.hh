// -*- mode: c++ -*-
/// \file sphere.hh
/// \brief Spheres.

#ifndef CGMATH_SPHERE_HH
#define CGMATH_SPHERE_HH

namespace cgmath
{
  /// Generic sphere.
  template<typename scalar, unsigned n>
  class sphere
  {
  public:
    /// Default constructor.
    sphere ()
    {}

    /// Copy constructor.
    sphere (const sphere<scalar,n>& s)
      : c(s.c), r(s.r)
    {}
    
    /// Constructor from center and radius.
    sphere (const vector_n<scalar,n>& _c, const scalar& _r)
      : c(_c), r(_r)
    {}
    
    /// Conversion to another scalar type.
    template<typename scalar2>
    operator sphere< scalar2, n > () const
    {
      return sphere<scalar2, n> (c, r);
    }
    
    /// Grow sphere to contain point.
    void grow (const vector_n<scalar,n>& point)
    {
      scalar n2 = norm2 (point - c);
      if (r * r < n2)
	r = static_cast<scalar>(sqrt(n2));
    }
    
    vector_n<scalar,n> c; ///< center of sphere
    scalar r;             ///< radius of sphere
  };

  typedef sphere<float,  2> sphere_2f;
  typedef sphere<double, 2> sphere_2d;
  typedef sphere<float,  3> sphere_3f;
  typedef sphere<double, 3> sphere_3d;

  /// \brief  Transform sphere in euclidean transform (rotate + translate, no scaling).
  ///
  /// This does not check that A is euclidean. That's your job. This is a 
  /// reminder.
  template<typename scalar>
  sphere<scalar,3> euclidean_transform (const matrix_mn<scalar, 4,4>& A,
					const sphere<scalar, 3>& S)
  {
    return sphere<scalar,3>(affine_transform(A, S.c), S.r);
  }
}

#endif
