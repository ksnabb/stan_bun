// -*- mode: c++ -*-
/// \file oriented_box.hh
/// \brief Oriented (rotated) boxes.

#ifndef CGMATH_ORIENTED_BOX_HH
#define CGMATH_ORIENTED_BOX_HH

namespace cgmath
{
  /// Oriented box. Contains points a <= R x <= b, where R is rotation.
  template<typename scalar, unsigned n>
  class oriented_box
  {
  public:
    oriented_box ()
    {
      R = matrix_mn<scalar,n,n>::identity ();
      if (std::numeric_limits<scalar>::has_infinity)
	for (unsigned i = 0; i < n; ++i)
	  {
	    a(i) =  std::numeric_limits<scalar>::infinity();
	    b(i) = -std::numeric_limits<scalar>::infinity();
	  }
      else
	for (unsigned i = 0; i < n; ++i)
	  {
	    a(i) = std::numeric_limits<scalar>::max();
	    b(i) = std::numeric_limits<scalar>::min();
	  }
    }
    
    oriented_box (const matrix_mn<scalar, n, n>& _R,
		  const vector_n<scalar,n>&      _a,
		  const vector_n<scalar,n>&      _b)
      : R(_R), a(_a), b(_b)
    {}

    /// Check if box is empty.
    bool is_empty (void) const
    {
      bool empty = false;
      for (unsigned i = 0; !empty && i < n; ++i)
	empty = a(i) > b(i);
      return empty;
    }
      
    /// Direction of ith axis.
    vector_n<scalar,n> axis (unsigned i) const
    {
      return R.row(i);
    }

    /// Axis along which the box is largest.
    unsigned largest_axis (void) const
    {
      unsigned m = 0;
      scalar   d = std::abs(b(0) - a(0));
      for (unsigned i = 1; i < n; ++i)
	{
	  scalar d_i = std::abs(b(i) - a(i));
	  if (d_i > d)
	    {
	      m = i;
	      d = d_i;
	    }
	}
      return m;
    }
    
    /// Grow box to contain point.
    void grow (const vector_n<scalar,n>& point)
    {
      vector_n<scalar,n> Rp = R * point;
      for (unsigned i = 0; i < n; ++i)
	{
	  a(i) = std::min(a(i), Rp(i));
	  b(i) = std::max(b(i), Rp(i));
	}
    }

    /// Sum of dimensions. Manhattan (or l1) norm of diameter vector.
    scalar manhattan_diameter () const
    {
      scalar d = 0;
      for (unsigned i = 0; i < n; ++i)
	d += std::abs(a(i) - b(i));
      return d;
    }

    /// Test if box contains point.
    bool contains (const vector_n<scalar,n>& point)
    {
      vector_n<scalar,n> Rp = R * point;
      bool inside = true;
      for (unsigned i = 0; inside && i < n; ++i)
	inside = a(i) <= Rp(i) && Rp(i) <= b(i);
      return inside;
    }


    matrix_mn<scalar, n, n> R; ///< Box orientation, must be rotation matrix.
    vector_n<scalar,n>      a; ///< Min bound
    vector_n<scalar,n>      b; ///< Max bound
  };

  typedef oriented_box<float,  2> oriented_box_2f;
  typedef oriented_box<double, 2> oriented_box_2d;
  typedef oriented_box<float,  3> oriented_box_3f;
  typedef oriented_box<double, 3> oriented_box_3d;

  /// Projects box to an axis, returns resulting interval [a,b].
  template<typename scalar, unsigned n>
  void project (const vector_n<scalar,n>& axis, 
		const oriented_box<scalar, n>& box,
		scalar& a,
		scalar& b)
  {
    // Transform axis to box coordinates.
    // Assuming R is a pure rotation. Otherwise use inverse transpose.
    vector_n<scalar,n> Ra = box.R * axis;
    // Find minimum and maximum corner.
    scalar _a = 0;
    scalar _b = 0;
    for (unsigned i = 0; i < n; ++i)
      {
	scalar ai = Ra(i) * box.a(i);
	scalar bi = Ra(i) * box.b(i);
	_a += std::min (ai, bi);
	_b += std::max (ai, bi);
      }
    a = _a;
    b = _b;
  }

  /// Transform box in euclidean transform (TODO: rename).
  template<typename scalar>
  oriented_box<scalar,3> affine_transform (const matrix_mn<scalar, 4, 4>& A,
					   const oriented_box<scalar, 3>& B)
  {
    // Transform using the transformation rule for planes.
    // Assuming A Euclidean (rotate + scale only).
    matrix_mn<scalar,3,3> R;
    vector_n<scalar, 3>   b;
    decompose_affine (A, R, b);
    matrix_mn<scalar,3,3> Rt   = transpose(R);
    matrix_mn<scalar,3,3> RRt  = B.R * Rt;
    vector_n<scalar, 3>   RRtb = RRt * b;
    return oriented_box<scalar,3>(RRt, B.a + RRtb, B.b + RRtb);
  }
}

#endif
