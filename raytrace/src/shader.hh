#ifndef SHADER_HH
#define SHADER_HH

#include<cgmath/cgmath.hh>
#include<vector>

class surface_point;

/// \brief Random scatter direction of ray with weight.
///
/// A set of samples should be generated so that
/// the sum of weighted light contributions is
/// correct on the average. 
///
/// For example if you generate directions randomly,
/// the weight should come from the bsdf and if you
/// use the bsdf to generate samples (importance sampling), 
/// then the weight should be the probability of non-absorption.
/// Different sample generation strategies results in 
/// different variances.
///
/// For the project work, you probably need to generate a 
/// single sample, not multiple.
///
class bsdf_sample 
{
public:
  bsdf_sample ()
  {}
  
  bsdf_sample (const cgmath::vector_3d& _direction,
	       const cgmath::vector_3d& _weight)
    : direction(_direction), weight (_weight)
  {}
  
  /// Incoming direction of light to point.
  cgmath::vector_3d direction;  
  /// Weight of sample.
  cgmath::vector_3d weight;  
};

typedef std::vector<bsdf_sample> bsdf_samples;

/// \brief Scatter simulation.
///
/// The shader's job is to simulate light scattering from a point.
/// It evaluates the relative amount of light reflecting from
/// a certain direction to another and generates samples from
/// the BSDF function.
class shader
{
public:

  /// Evaluate BSDF at surface point.
  virtual cgmath::vector_3d bsdf (const surface_point&     point, 
				  const cgmath::vector_3d& in_dir, 
				  const cgmath::vector_3d& out_dir) = 0;

  /// Pick random samples from BSDF given out direction.
  virtual void sample_bsdf (const surface_point&     point, 
			    unsigned                 num_samples,
			    const cgmath::vector_3d& out_dir,
			    bsdf_samples&            samples) = 0;
};

#endif
