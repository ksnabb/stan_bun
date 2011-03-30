#include "lambert_shader.hh"

using namespace std;
using namespace cgmath;

lambert_shader::lambert_shader (const vector_3d& _diffuse)
  : diffuse (_diffuse)
{}

vector_3d lambert_shader::bsdf (const surface_point& point,
				const vector_3d&     in_dir,
				const vector_3d&  /* out_dir */)
{
  cgmath::vector_3d c(0.0);
  double cos_theta = -cgmath::dot(in_dir, point.normal);
  if (cos_theta > 0)
    c = (cos_theta / M_PI) * diffuse;
  return c;
}
  
void lambert_shader::sample_bsdf (const surface_point& point,
				  unsigned             num_samples,
				  const vector_3d&     out_dir,
				  bsdf_samples& samples)
{
  samples.resize (num_samples);
  matrix_3d R;
  R = rotate (vec(0.0, 0.0, 1.0), point.normal);
  for (unsigned i = 0; i < samples.size(); ++i)
    samples[i] = bsdf_sample (R * lambert_sample(), diffuse);
}
