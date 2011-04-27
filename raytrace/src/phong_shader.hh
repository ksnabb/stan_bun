#ifndef PHONG_SHADER_HH
#define PHONG_SHADER_HH

#include <cmath>
#include "sampling.hh"
#include "surface_point.hh"
#include "shader.hh"

/// \brief phongian reflection model.
class phong_shader : public shader
{
public:

  phong_shader (const cgmath::vector_3d& _ambient, const cgmath::vector_3d&
                  _phong, double alpha);
  
  virtual cgmath::vector_3d bsdf (const surface_point&     point,
				  const cgmath::vector_3d& in_dir,
				  const cgmath::vector_3d& /* out_dir */);

  virtual void sample_bsdf (const surface_point&     point,
			    unsigned                 num,
			    const cgmath::vector_3d& out_dir,
			    bsdf_samples&            samples);

  /// Fraction of reflected light for each color sample.
  cgmath::vector_3d ambient;
  cgmath::vector_3d phong;
  double alpha;
};

#endif
