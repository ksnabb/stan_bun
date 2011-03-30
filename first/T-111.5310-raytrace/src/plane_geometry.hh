#ifndef PLANE_GEOMETRY_HH
#define PLANE_GEOMETRY_HH

#include <cgmath/cgmath.hh>
#include "geometry.hh"

/// Infinite plane geometry.
class plane_geometry : public geometry
{
public:
  plane_geometry (const cgmath::plane_3d& _plane);
  
  virtual double hit (const cgmath::ray_3d& ray)  const;
  virtual void hit_surface_point (surface_point& pt) const;

  cgmath::plane_3d plane;
  mutable cgmath::vector_3d hit_point;
};

#endif
