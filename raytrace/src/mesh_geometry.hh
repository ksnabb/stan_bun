#ifndef MESH_GEOMETRY_HH
#define MESH_GEOMETRY_HH

#include <cgmath/cgmath.hh>
#include "geometry.hh"
#include "triangle_geometry.hh"

/// Infinite plane geometry.
class mesh_geometry : public geometry
{
public:
  mesh_geometry (const char *);
  
  virtual double hit (const cgmath::ray_3d& ray)  const;
  virtual void hit_surface_point (surface_point& pt) const;

  triangle_geometry *triangles;
  mutable cgmath::vector_3d hit_point;
};

#endif
