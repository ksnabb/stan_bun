// -*- mode: c++ -*-
/// \file ray.hh
/// \brief Rays

#ifndef CGMATH_RAY_HH
#define CGMATH_RAY_HH

namespace cgmath
{
  /// Infinite ray from a point to given direction.
  template<typename scalar, unsigned n>
  class ray
  {
  public:
    /// Default constructor
    ray ()
    {}
    
    /// Copy constructor
    ray (const ray<scalar,n>& r)
      : p(r.p), d(r.d)
    {}
    
    /// Constructor from start and direction
    ray (const vector_n<scalar,n>& _p,
	 const vector_n<scalar,n>& _d)
      : p(_p), d(_d)
    {}
    
    /// Scalar type conversion.
    template<typename scalar2>
    operator ray<scalar2, n> () const
    {
      return ray<scalar2, n> (static_cast< vector_n<scalar2, n> > (p),
			      static_cast< vector_n<scalar2, n> > (d));
    }
    
    vector_n<scalar,n> operator() (const scalar& t) const
    {
      return p + t*d;
    }
    
    vector_n<scalar,n> p; ///< start of ray
    vector_n<scalar,n> d; ///< direction of ray
  };

  typedef ray<float,  2> ray_2f;
  typedef ray<double, 2> ray_2d;
  typedef ray<float,  3> ray_3f;
  typedef ray<double, 3> ray_3d;

  /// Transform ray in affine transform.
  template<typename scalar>
  ray<scalar,3> affine_transform (const matrix_mn<scalar, 4, 4>& A,
				  const ray<scalar, 3>& r)
  {
    vector_n<scalar, 4> Tp = A * vec<scalar>(r.p(0), r.p(1), r.p(2), 1);
    vector_n<scalar, 4> Td = A * vec<scalar>(r.d(0), r.d(1), r.d(2), 0);
    return ray<scalar,3>(vec<scalar> (Tp(0), Tp(1), Tp(2)),
			 vec<scalar> (Td(0), Td(1), Td(2)));
  }

}

#endif
