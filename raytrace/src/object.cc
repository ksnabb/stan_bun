#include "surface_point.hh"
#include "shader.hh"
#include "geometry.hh"
#include "object.hh"

using namespace cgmath;

object::object (const cgmath::matrix_4d& _T, 
		::geometry* _geometry, 
		::shader*  _shader)
  : T (_T), geometry (_geometry), shader(_shader)
{
  T_inv = inv(T);
}

double object::hit (const cgmath::ray_3d& r) const
{
  cgmath::ray_3d Tr = affine_transform (T_inv, r);
  return geometry->hit (Tr);
}

void object::hit_surface_point (surface_point& pt) const
{
  geometry->hit_surface_point (pt);
  // Transform surface point geometry.
  pt.point  = cgmath::affine_transform(T, pt.point);
  // Normal transformation (inverse transpose multiply)
  pt.normal 
    = cgmath::normalized(cgmath::normal_transform (T_inv, pt.normal));
  // Connect shader to surface point.
  pt.shader = shader;
}
