#ifndef MIRROR_SHADER_HH
#define MIRROR_SHADER_HH

#include <cmath>
#include "sampling.hh"
#include "surface_point.hh"
#include "shader.hh"

/// \brief mirrorian reflection model.
class mirror_shader : public shader
{
public:

  mirror_shader ();
  
  virtual cgmath::vector_3d bsdf (const surface_point&     point,
				  const cgmath::vector_3d& in_dir,
				  const cgmath::vector_3d& /* out_dir */);

  virtual void sample_bsdf (const surface_point&     point,
			    unsigned                 num,
			    const cgmath::vector_3d& out_dir,
			    bsdf_samples&            samples);

};

#endif
