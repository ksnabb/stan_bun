#ifndef SPHERE_GEOMETRY_HH
#define SPHERE_GEOMETRY_HH

#include <cgmath/cgmath.hh>
#include "geometry.hh"

/// Simple sphere geometry.
class sphere_geometry : public geometry
{
public:
  sphere_geometry (const cgmath::vector_3d& c, double r);

  virtual double hit (const cgmath::ray_3d& r) const;
  virtual void   hit_surface_point (surface_point& sp) const;
  
  cgmath::sphere_3d s;
  mutable cgmath::vector_3d hit_point;
};

#endif
