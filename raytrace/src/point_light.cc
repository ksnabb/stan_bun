#include "point_light.hh"

using namespace std;
using namespace cgmath;

point_light::point_light (const vector_3d& _point, const vector_3d& _intensity)
  : point (_point), 
    intensity (_intensity)
{}

void point_light::illuminate (const vector_3d& surface,
			      unsigned n,
			      light_samples& samples)
{
  vector_3d direction_to_light = point - surface;
  double distance = normalize (direction_to_light);
  // Distance attenuation (inverse square law).
  vector_3d radiance = intensity * (1.0 / (distance * distance));
  // A single sample is always enough for a point light.
  samples.resize(1);
  samples[0] = light_sample (direction_to_light, distance, radiance);
}
