#ifndef GEOMETRY_HH
#define GEOMETRY_HH

class surface_point;

/// Surface geometry of an object.
class geometry
{
public:
  /// Check if ray intersects object. Return infinity if not.
  virtual double hit (const cgmath::ray_3d& r) const = 0;

  /// Fill surface point with local geometry corresponding to 
  /// last hit. You must cache hit query result to compute this.
  virtual void hit_surface_point (surface_point&) const = 0;
};

#endif
