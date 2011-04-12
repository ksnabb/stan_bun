// -*- mode: c++ -*-
/// \file raytracer.hh
/// \brief Simple raytracer.

#ifndef RAYTRACER_HH
#define RAYTRACER_HH

#include <SDL/SDL.h>
#include <vector>
#include <cgmath/cgmath.hh>
#include "object.hh"
#include "light.hh"

/// \brief Raytracer
///
/// Contains scene and raytracing logic. 
///
class raytracer
{
public:
  raytracer (void);

  /// Return nearest object hit by ray, if closer than max_l.
  object* hit_object (const cgmath::ray_3d& ray, double max_l);

  /// Return nearest surface point hit by ray, if closer than max_l.
  object* hit_surface (const cgmath::ray_3d& ray, double max_l, surface_point& sp);

  /// Trace single ray
  cgmath::vector_3d trace (const cgmath::ray_3d& ray);
  
  /// Shade surface point as seen from out direction.
  cgmath::vector_3d shade (const cgmath::vector_3d& out, const surface_point& sp);

  /// Shade ray that exits scene. 
  cgmath::vector_3d shade_exiting (const cgmath::ray_3d& ray);

  /// Trace an image into a SDL surface given the projection matrix P.
  void trace_image (const cgmath::matrix_4d& P, SDL_Surface* s);

  /// Trace a scanline of an image into a SDL surface given the 
  /// projection matrix P.
  void trace_scanline (const cgmath::matrix_4d& P, 
		       SDL_Surface* s,
		       unsigned scanline);
  
  /// Objects in scene.
  std::vector<object*> objects;
  /// Lights in scene.
  std::vector<light*>  lights;
};

#endif
