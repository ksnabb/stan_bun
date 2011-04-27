//-*- mode: c++ -*-
/// \file vector_n.hh
/// \brief nD vector operations.

#ifndef CGMATH_VECTOR_N_HH
#define CGMATH_VECTOR_N_HH

#include <iostream>
#include <cmath>

namespace cgmath
{
  /// General n-dimensional vector.
  template<typename scalar, unsigned n>
  class vector_n
  {
  public:

    /// Default constructor.
    vector_n () {}

    /// Constructor that fills the vector with initial value.
    explicit vector_n (const scalar& s) 
    {
      std::fill (v, v+n, s);
    }

    /// Copy constructor. Also provides operator= to copy vectors.
    vector_n (const vector_n& rhs)
    {
      std::copy (rhs.v, rhs.v+n, v); 
    }

    /// Conversion to pointer. Needed to for passing vectors to glVertex3fv etc.
    operator scalar* () 
    {
      return v;
    }

    /// Conversion to const pointer. 
    operator const scalar* () const
    {
      return v;
    }

    /// Conversion to another scalar type.
    template<typename scalar2>
    operator vector_n< scalar2, n > () const
    {
      vector_n< scalar2, n > result;
      for (unsigned i = 0; i < n; ++i)
	result(i) = static_cast<scalar2>(v[i]);
      return result;
    }
    
    /// Vector indexing.
    scalar& operator () (unsigned i) 
    {
      return v[i];
    }

    /// Vector indexing.
    const scalar& operator () (unsigned i) const
    {
      return v[i];
    }

    /// Multiply in-place by scalar.
    vector_n& operator*= (const scalar& a)
    {
      for (unsigned i = 0; i < n; ++i)
	v[i] *= a;
      return *this;
    }

    /// Accumulate in-place.
    vector_n& operator+= (const vector_n& rhs)
    {
      for (unsigned i = 0; i < n; ++i)
	v[i] += rhs(i);
      return *this;
    }

    /// Subtract in-place.
    vector_n& operator-= (const vector_n& rhs)
    {
      for (unsigned i = 0; i < n; ++i)
	v[i] -= rhs(i);
      return *this;
    }

    static vector_n<scalar,n> uniform_random (scalar a = 0, scalar b = 1)
    {
      vector_n<scalar,n> v;
      for (unsigned i = 0; i < n; ++i)
	v(i) = a + (rand()/static_cast<double>(RAND_MAX)) * (b - a);
      return v;
    }

    // Vector elements.
    scalar v[n];
  };
  
  /// Vector output.
  template<typename scalar, unsigned n>
  std::ostream& operator<< (std::ostream& os, 
			    const vector_n<scalar, n>& u)
  {
    os << "(";
    for (unsigned i = 0; i < n; ++i)
      {
	os << u(i);
	if (i != n-1) os << ", ";
      }
    os << ")";
    return os;
  }

  /// Dot product.
  template<typename scalar, unsigned n>
  scalar dot (const vector_n<scalar,n>& u, const vector_n<scalar,n>& v)
  {
    scalar dot = static_cast<scalar>(0);
    for (unsigned i = 0; i < n; ++i)
      dot += u(i) * v(i);
    return dot;
  }

  /// Max norm of vector.
  template<typename scalar, unsigned n>
  scalar norm_max (const vector_n<scalar,n>& u)
  {
    scalar m = u(1);
    for (unsigned i = 0; i < n; ++i)
      m = std::max(m, std::abs(u(i)));
    return m;
  }

  /// Squared norm of vector.
  template<typename scalar, unsigned n>
  scalar norm2 (const vector_n<scalar,n>& u)
  {
    return dot(u,u);
  }

  /// Norm of vector.
  template<typename scalar, unsigned n>
  double norm (const vector_n<scalar,n>& u)
  {
    return std::sqrt(norm2(u));
  }

  /// Squared distance.
  template<typename scalar, unsigned n>
  scalar distance2 (const vector_n<scalar,n>& u, const vector_n<scalar,n>& v)
  {
    return norm2(u - v);
  }

  /// Distance.
  template<typename scalar, unsigned n>
  double distance (const vector_n<scalar,n>& u, const vector_n<scalar,n>& v)
  {
    return std::sqrt(distance2(u, v));
  }

  /// Vector sum.
  template<typename scalar, unsigned n>
  vector_n<scalar,n> operator+ (const vector_n<scalar,n>& u,
				const vector_n<scalar,n>& v)
  {
    vector_n<scalar,n> result;
    for (unsigned i = 0; i < n; ++i)
      result(i) = u(i) + v(i);
    return result;
  }

  /// Vector negation.
  template<typename scalar, unsigned n>
  vector_n<scalar,n> operator- (const vector_n<scalar,n>& u)
  {
    vector_n<scalar,n> result;
    for (unsigned i = 0; i < n; ++i)
      result(i) = -u(i);
    return result;
  }

  /// Vector subtract.
  template<typename scalar, unsigned n>
  vector_n<scalar,n> operator- (const vector_n<scalar,n>& u,
				const vector_n<scalar,n>& v)
  {
    vector_n<scalar,n> result;
    for (unsigned i = 0; i < n; ++i)
      result(i) = u(i) - v(i);
    return result;
  }

  /// Scalar-vector multiplication.
  template<typename scalar, unsigned n>
  vector_n<scalar,n> operator* (const scalar& a,
				const vector_n<scalar,n>& v)
  {
    vector_n<scalar,n> result;
    for (unsigned i = 0; i < n; ++i)
      result(i) = a * v(i);
    return result;
  }

  /// Scalar-vector multiplication.
  template<typename scalar, unsigned n>
  vector_n<scalar,n> operator* (const vector_n<scalar,n>& v,
				const scalar& a)
  {
    return a * v;
  }

  /// Dot product.
  template<typename scalar, unsigned n>
  scalar operator* (const vector_n<scalar,n>& u,
		    const vector_n<scalar,n>& v)
  {
    return dot(u,v);
  }

  /// Elementwise product.
  template<typename scalar, unsigned n>
  vector_n<scalar,n> mul (const vector_n<scalar,n>& u,
			  const vector_n<scalar,n>& v)
  {
    vector_n<scalar,n> result;
    for (unsigned i = 0; i < n; ++i)
      result(i) = u(i) * v(i);
    return result;
  }
  
  /// Return direction vector.
  template<typename scalar, unsigned n>
  vector_n<scalar,n> normalized (const vector_n<scalar,n>& v)
  {
    vector_n<scalar, n> nv;
    double _n = 1.0 / norm (v);
    for (unsigned i = 0; i < n; ++i)
      nv(i) = static_cast<scalar>(_n * v(i));
    return nv;
  }

  /// Normalize vector in-place. Return norm.
  template<typename scalar, unsigned n>
  double normalize (vector_n<scalar,n>& v)
  {
    double nv = norm(v);
    double _n = 1.0 / nv;
    for (unsigned i = 0; i < n; ++i)
      v(i) *= _n;
    return nv;
  }

  /// Return direction vector.
  template<typename scalar, unsigned n>
  scalar sum (const vector_n<scalar,n>& v)
  {
    scalar s = 0;
    for (unsigned i = 0; i < n; ++i)
      s += v(i);
    return s;
  }
}
#endif
