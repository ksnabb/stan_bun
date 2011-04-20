#ifndef TRIANGLE_GEOMETRY_HH
#define TRIANGLE_GEOMETRY_HH

#include <cgmath/cgmath.hh>
#include "geometry.hh"

/// Triangle geometry.
class triangle_geometry : public geometry
{
public:
  triangle_geometry (const cgmath::triangle_3d& _triangle);
  
  virtual double hit (const cgmath::ray_3d& ray)  const;
  virtual void hit_surface_point (surface_point& pt) const;

  cgmath::triangle_3d triangle;
  mutable cgmath::vector_3d hit_point;
  mutable cgmath::vector_3d hit_coords;
};

#endif
