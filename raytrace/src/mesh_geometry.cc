#include "surface_point.hh"
#include "mesh_geometry.hh"

mesh_geometry::mesh_geometry (const char *file_name)
{
  //copy mesh object loader here and create array of triangles 
}
  
double mesh_geometry::hit (const cgmath::ray_3d& ray) const
{
  //loop through all triangles and call the hit check which is closest
}
  
void mesh_geometry::hit_surface_point (surface_point& pt) const 
{
  //something
}
