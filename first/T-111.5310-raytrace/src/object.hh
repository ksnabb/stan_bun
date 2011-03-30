#ifndef OBJECT_HH
#define OBJECT_HH

#include <cgmath/cgmath.hh>

class shader;
class geometry;
class surface_point;

/// \brief Shaded object for raytracing. 
///
/// A raytracer object is made of geometry defining the surface 
/// of the object and shader responsible for reflection of light 
/// from the object. The object is movable using a transform.
class object
{
public:
  object (const cgmath::matrix_4d& _T, geometry* _geometry, shader* _shader);

  /// Check if ray hits object. Return infinity if not.
  virtual double hit (const cgmath::ray_3d& r) const;

  /// Fill surface point with details of last hit to object in world
  /// coordinates.
  virtual void hit_surface_point (surface_point& pt) const;

  /// Transformation from object to world coordinates.
  cgmath::matrix_4d T;
  /// Transformation from world to object coordinates.
  cgmath::matrix_4d T_inv;
  /// Surface geometry.
  ::geometry* geometry;
  /// Surface appearance shader.
  ::shader*   shader;
};

#endif
