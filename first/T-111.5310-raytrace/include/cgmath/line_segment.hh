// -*- mode: c++ -*-
/// \file line_segment.hh
/// \brief Line segments.

#ifndef CGMATH_LINE_SEGMENT_HH
#define CGMATH_LINE_SEGMENT_HH

namespace cgmath
{
  /// General line segment.
  template<typename scalar, unsigned n>
  class line_segment
  {
  public:
    /// Default constructor
    line_segment ()
    {}
    
    /// Copy constructor
    line_segment (const line_segment<scalar,n>& l)
      : a(l.a), b(l.b)
    {}
    
    /// Constructor from ends.
    line_segment (const vector_n<scalar,n>& _a,
		  const vector_n<scalar,n>& _b)
      : a(_a), b(_b)
    {}
    
    /// Scalar type conversion.
    template<typename scalar2>
    operator line_segment<scalar2, n> () const
    {
      return line_segment<scalar2, n> (static_cast< vector_n<scalar2, n> > (a),
				       static_cast< vector_n<scalar2, n> > (b));
    }
    
    /// Extract subsegment corresponding to [t0, t1] when original is [0, 1].
    line_segment subsegment (const scalar& t0, const scalar& t1) const
    {
      return line_segment (a + t0 * (b - a), a + t1 * (b - a));
    }
    
    vector_n<scalar,n> a; ///< start of segment
    vector_n<scalar,n> b; ///< end of segment
  };

  typedef line_segment<float,  2> line_segment_2f;
  typedef line_segment<double, 2> line_segment_2d;
  typedef line_segment<float,  3> line_segment_3f;
  typedef line_segment<double, 3> line_segment_3d;


  /// Transform segment in affine transform.
  template<typename scalar>
  line_segment<scalar,3> affine_transform (const matrix_mn<scalar, 4,4>& A,
					   const line_segment<scalar, 3>& l)
  {
    // TODO: use 3x4 submatrix.
    vector_n<scalar, 4> a4 = A * vec<scalar>(l.a(0), l.a(1), l.a(2), 1);
    vector_n<scalar, 4> b4 = A * vec<scalar>(l.b(0), l.b(1), l.b(2), 1);
    return line_segment<scalar,3>(vec<scalar> (a4(0), a4(1), a4(2)),
				  vec<scalar> (b4(0), b4(1), b4(2)));
  }
}

#endif
