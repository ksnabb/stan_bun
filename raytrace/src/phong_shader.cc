#include "phong_shader.hh"
#include <math.h>

using namespace std;
using namespace cgmath;

phong_shader::phong_shader (const vector_3d& _ambient, const vector_3d& _phong, double _alpha)
  : ambient (_ambient), phong (_phong), alpha (_alpha)
{}

vector_3d phong_shader::bsdf (const surface_point& point,
				const vector_3d&     in_dir,
				const vector_3d&   out_dir )
{
  cgmath::vector_3d c(0.0);
  vector_3d R_m = 2*dot(in_dir, point.normal)*point.normal - in_dir;
  double temp = dot(R_m, out_dir);
  temp = pow(temp, alpha);
    c += temp*phong; 
  return c;
}
  
void phong_shader::sample_bsdf (const surface_point& point,
				  unsigned             num_samples,
				  const vector_3d&     out_dir,
				  bsdf_samples& samples)
{
  samples.resize (num_samples);
  matrix_3d R;
  R = rotate (vec(0.0, 0.0, 1.0), point.normal);
  for (unsigned i = 0; i < samples.size(); ++i){
      //phong_sample needs to work differently
    vector_3d direction = phong_sample();
    samples[i] = bsdf_sample (R * direction, bsdf(point, out_dir, direction));
  }
}
