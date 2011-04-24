#include "mirror_shader.hh"

using namespace std;
using namespace cgmath;

mirror_shader::mirror_shader ()
{}

vector_3d mirror_shader::bsdf (const surface_point& point,
				const vector_3d&     in_dir,
				const vector_3d&  /* out_dir */)
{
  cgmath::vector_3d c(0.0);
  return c;
}
  
void mirror_shader::sample_bsdf (const surface_point& point,
				  unsigned             num_samples,
				  const vector_3d&     out_dir,
				  bsdf_samples& samples)
{
  samples.resize (num_samples);
  matrix_3d R;
  R = rotate (vec(0.0, 0.0, 1.0), point.normal);
  for (unsigned i = 0; i < samples.size(); ++i)
    samples[i] = bsdf_sample (R * out_dir, vec(1.0, 1.0, 1.0));
}
