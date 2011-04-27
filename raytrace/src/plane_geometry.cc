#include "surface_point.hh"
#include "plane_geometry.hh"

plane_geometry::plane_geometry (const cgmath::plane_3d& _plane)
  : plane(_plane)
{
  double _nn = 1.0/norm(plane.n);
  plane.n *= _nn;
  plane.d *= _nn;
}
  
double plane_geometry::hit (const cgmath::ray_3d& ray) const
  {
    double t;
    if (cgmath::intersect (ray, plane, t))
      {
	hit_point = ray(t);
	return t;
      }
    else
      return std::numeric_limits<double>::infinity();
  }
  
void plane_geometry::hit_surface_point (surface_point& pt) const 
{ 
  pt.point  = hit_point;
  pt.normal = plane.n;
  pt.u = 0.0;
  pt.v = 0.0;
}
