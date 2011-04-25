#include "glass_shader.hh"

using namespace std;
using namespace cgmath;

glass_shader::glass_shader (double _refr)
  : refractive_index (_refr)
{}

vector_3d glass_shader::bsdf (const surface_point& point,
				const vector_3d&     in_dir,
				const vector_3d&  /* out_dir */)
{
  //our glass is black in itself
  cgmath::vector_3d c(0.0);
  return c;
}
 /*a perfect glass that doesn't have reflection at all*/ 
void glass_shader::sample_bsdf (const surface_point& point,
				  unsigned             num_samples,
				  const vector_3d&     out_dir,
				  bsdf_samples& samples)
{
  samples.resize (num_samples);
  double r_i = refractive_index;
  vector_3d od = -out_dir;
  vector_3d pn = point.normal;
  //formula straight from wikipedia http://en.wikipedia.org/wiki/Snell%27s_law
  double cos_1 = dot(-od, pn);
  if (cos_1 >= 0) //we are inside object, invert refraction
      r_i = 1/r_i;
  double cos_2_part = 1 - (r_i*r_i)*(1-(cos_1*cos_1));
//  if (cos_2_part < 0)
//      cos_2_part = 0;
  double cos_2;
  cos_2 = sqrt(cos_2_part);
  vector_3d v_reflect = od + (2*cos_1)*pn;
  vector_3d v_refract;
  if (cos_1 >=0)
  v_refract = (r_i*od)+
    ((r_i*cos_1-cos_2)*pn);
  else
  v_refract = (r_i*od) +
    ((r_i*cos_1+cos_2)*pn);
  
  for (unsigned i = 0; i < samples.size(); ++i)
      if (cos_2_part >= 0 )
        samples[i] = bsdf_sample (v_refract, vec(1.0, 1.0, 1.0));
        else 
        samples[i] = bsdf_sample (v_reflect, vec(1.0, 1.0, 1.0));
}
