#ifndef SURFACE_POINT_HH
#define SURFACE_POINT_HH

#include "shader.hh"

/// \brief Surface point hit by a ray.
///
/// Surface point encapsulates the local geometry required to compute 
/// light scattering from the point to the direction of the ray. 
/// The ray tracer does the actual sampling of the scattered rays.
class surface_point
{
public:

  surface_point ()
  {}
  
  /// Point in world coordinates.
  cgmath::vector_3d point;
  /// Surface normal at point in world coordinates. Normalized.
  cgmath::vector_3d normal;
  /// U texture coordinate at point. 
  double u;
  /// V texture coordinate at point. 
  double v;
  /// Shader for computing reflections at point.
  ::shader* shader;

  /// Evaluate BSDF from in_dir to out_dir.
  virtual cgmath::vector_3d bsdf (const cgmath::vector_3d& in_dir, 
				  const cgmath::vector_3d& out_dir) const
  {
    return shader->bsdf (*this, in_dir, out_dir);
  }

  /// \brief Pick random samples from BRDF given out direction.
  ///
  /// Use this to generate sample directions for indirect illumination.
  virtual void sample_bsdf (unsigned num_samples,
			    const cgmath::vector_3d& out_dir,
			    bsdf_samples& samples) const
  {
    shader->sample_bsdf (*this, num_samples, out_dir, samples);
  }
  
};

#endif
