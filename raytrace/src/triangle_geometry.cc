#include "surface_point.hh"
#include "triangle_geometry.hh"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <cassert>

using namespace std;
using namespace cgmath;

triangle_geometry::triangle_geometry (const cgmath::triangle_3d& _triangle)
  : triangle(_triangle)
{
    //hit_coords = vec (0.0, 0.0, 0.0);
}
  
double triangle_geometry::hit (const cgmath::ray_3d& ray) const
  {
    double t, a, b, c;
    /*ToDo: figure out why we get hit coords*/
    if (cgmath::intersect (ray, triangle, t, a, b, c))
      {
          printf("ray hit triangle");
	hit_point = ray(t);
    //hit_coords = vec(a, b, c);
	return t;
      }
    else
      return std::numeric_limits<double>::infinity();
  }
  
void triangle_geometry::hit_surface_point (surface_point& pt) const 
{ 
  pt.point  = hit_point;
  //pt.normal = triangle_normal;
  pt.normal = normal(triangle);
  pt.u = 0.0;
  pt.v = 0.0;
  printf("triangle hit at(%f, %f, %f)\n"
  "normal (%f, %f, %f)\n", 
  hit_point[0], hit_point[1],
  hit_point[2], pt.normal[0], pt.normal[1], pt.normal[2]);
}
