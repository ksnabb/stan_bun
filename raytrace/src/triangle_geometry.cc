#include "surface_point.hh"
#include "triangle_geometry.hh"

triangle_geometry::triangle_geometry (const cgmath::triangle_3d& _triangle)
  : triangle(_triangle)
{
}
  
double triangle_geometry::hit (const cgmath::ray_3d& ray) const
  {
    double t, a, b, c;
    /*ToDo: figure out why we get hit coords*/
    if (cgmath::intersect (ray, triangle, t, a, b, c))
      {
	hit_point = ray(t);
	return t;
      }
    else
      return std::numeric_limits<double>::infinity();
  }
  
void triangle_geometry::hit_surface_point (surface_point& pt) const 
{ 
  pt.point  = hit_point;
  pt.normal = normal(triangle);
  pt.u = 0.0;
  pt.v = 0.0;
}
