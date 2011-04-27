#include "surface_point.hh"
#include "sphere_geometry.hh"

using namespace cgmath;

sphere_geometry::sphere_geometry (const cgmath::vector_3d& c, double r) 
  : s(c,r)
{}

double sphere_geometry::hit (const cgmath::ray_3d& r) const
{
  double t;
  if (cgmath::nearest_point_on (r, s, t))
    {
      hit_point = r(t);
      return t;
    }
  else
    return std::numeric_limits<double>::infinity();
}
  
void sphere_geometry::hit_surface_point (surface_point& pt) const 
{ 
  pt.point  = hit_point;
  pt.normal = normalized(hit_point - s.c);
  pt.u = 0.0;
  pt.v = 0.0;
}
