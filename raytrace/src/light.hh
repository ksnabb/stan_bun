#ifndef LIGHT_HH
#define LIGHT_HH

#include <vector>
#include <cgmath/cgmath.hh>

/// \brief Random ray of light from light to point.
///
/// Lights illuminate a point by generating light samples.
/// Each light sample has an associated direction and distance
/// to check if the light source is visible along that ray.
/// For a light infinitely far away, set the distance to 
/// infinity. 
///
/// The irradiance should be adjusted for 1/r^2
/// decay from the light surface and account for angle
/// between light surface normal and ray to object point.
class light_sample
{
public:
  light_sample () 
  {}
  
  light_sample (const cgmath::vector_3d& _direction,
		double                   _distance,
		const cgmath::vector_3d& _radiance)
    : direction  (_direction),
      distance   (_distance),
      radiance   (_radiance)
  {}
  
  /// Direction from point to light.
  cgmath::vector_3d direction;
  /// Distance from point to light. 
  double            distance;
  /// Incident radiance from light point.
  cgmath::vector_3d radiance;
};

typedef std::vector<light_sample> light_samples;

/// \brief Abstract light.
///
/// Lights generate light samples to a surface point.
/// The ray tracer is responsible for determining visibility,
/// the light only supplies the direction and intensity of
/// light.
class light 
{
public:
  virtual ~light() {}

  /// Generates a lighting sample from light to point.
  /// n is number of samples requested. Size of vector indicates 
  /// number of returned samples.
  virtual void illuminate (const cgmath::vector_3d& x,
			   unsigned n, 
			   light_samples& samples) = 0;
};

#endif
