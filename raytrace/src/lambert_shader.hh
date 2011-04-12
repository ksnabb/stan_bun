#ifndef LAMBERT_SHADER_HH
#define LAMBERT_SHADER_HH

#include <cmath>
#include "sampling.hh"
#include "surface_point.hh"
#include "shader.hh"

/// \brief Lambertian reflection model.
class lambert_shader : public shader
{
public:

  lambert_shader (const cgmath::vector_3d& _diffuse);
  
  virtual cgmath::vector_3d bsdf (const surface_point&     point,
				  const cgmath::vector_3d& in_dir,
				  const cgmath::vector_3d& /* out_dir */);

  virtual void sample_bsdf (const surface_point&     point,
			    unsigned                 num,
			    const cgmath::vector_3d& out_dir,
			    bsdf_samples&            samples);

  /// Fraction of reflected light for each color sample.
  cgmath::vector_3d diffuse;
};

#endif
