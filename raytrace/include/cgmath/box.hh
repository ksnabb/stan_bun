// -*- mode: c++ -*-
/// \file box.hh
/// \brief Axis-aligned boxes.

#ifndef CGMATH_BOX_HH
#define CGMATH_BOX_HH

namespace cgmath
{
  /// N-dimensional axis aligned box.
  template<typename scalar, unsigned n>
  class box
  {
  public:
    box ()
    {
      if (std::numeric_limits<scalar>::has_infinity)
	for (unsigned i = 0; i < n; ++i)
	  {
	    min_bound(i) =  std::numeric_limits<scalar>::infinity();
	    max_bound(i) = -std::numeric_limits<scalar>::infinity();
	  }
      else
	for (unsigned i = 0; i < n; ++i)
	  {
	    min_bound(i) = std::numeric_limits<scalar>::max();
	    max_bound(i) = std::numeric_limits<scalar>::min();
	  }
    }

    box (const box<scalar,n>& b)
      : min_bound (b.min_bound), max_bound (b.max_bound)
    {}
    
    box (const vector_n<scalar,n>& _min_bound, 
	 const vector_n<scalar,n>& _max_bound)
      : min_bound(_min_bound), 
	max_bound(_max_bound)
    {}

   
    /// Check if box is empty.
    bool is_empty (void) const
    {
      bool empty = false;
      for (unsigned i = 0; !empty && i < n; ++i)
	empty = min_bound(i) > max_bound(i);
      return empty;
    }
      
    /// Grow box to contain point.
    void grow (const vector_n<scalar,n>& point)
    {
      for (unsigned i = 0; i < n; ++i)
	{
	  min_bound(i) = std::min(min_bound(i), point(i));
	  max_bound(i) = std::max(max_bound(i), point(i));
	}
    }

    unsigned largest_axis (void) const
    {
      unsigned m = 0;
      scalar   d = max_bound(0) - min_bound(0);
      for (unsigned i = 1; i < n; ++i)
	{
	  scalar d_i = max_bound(i) - min_bound(i);	
	  if (d_i > d)
	    {
	      m = i;
	      d = d_i;
	    }
	}
      return m;
    }

    vector_n<scalar, n> center (void) const
    {
      return static_cast<scalar>(0.5) * (min_bound + max_bound);
    }

    vector_n<scalar,n> min_bound;
    vector_n<scalar,n> max_bound;
  };

  typedef box<float,  2> box_2f;
  typedef box<double, 2> box_2d;
  typedef box<float,  3> box_3f;
  typedef box<double, 3> box_3d;
}

#endif
