// -*- mode: c++ -*-
/// \file point_light.hh

#ifndef POINT_LIGHT_HH
#define POINT_LIGHT_HH

#include "light.hh"

/// Simple point light.
class point_light : public light
{
public:
  point_light (const cgmath::vector_3d& _point, 
	       const cgmath::vector_3d& _intensity);

  virtual void illuminate (const cgmath::vector_3d& point,
			   unsigned n,
			   light_samples& samples);

  cgmath::vector_3d point;
  cgmath::vector_3d intensity;
};

#endif
