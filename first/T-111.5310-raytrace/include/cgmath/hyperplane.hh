#ifndef CGMATH_HYPERPLANE_HH
#define CGMATH_HYPERPLANE_HH

namespace cgmath
{
  /// General hyperplane, given by x: x . n - d = 0.
  template<typename scalar, unsigned dim>
  class hyperplane
  {
  public:
    hyperplane (const vector_n<scalar,dim>& _n, const scalar& _d)
      : n(_n), d(_d)
    {}

    hyperplane (const hyperplane<scalar,dim>& rhs)
      : n(rhs.n), d(rhs.d)
    {}
    
    scalar operator () (const vector_n<scalar,dim>& x) const
    {
      return dot(x,n) - d;
    }
    
    vector_n<scalar, dim> n;
    scalar                d;
  };

  typedef hyperplane<float,3>  plane_3f;
  typedef hyperplane<double,3> plane_3d;
}

#endif
