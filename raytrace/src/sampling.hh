/// \file sampling.hh
/// \brief Random sample generation.

#ifndef SAMPLING_HH
#define SAMPLING_HH

#include <cgmath/cgmath.hh>

/// Generate cosine-weighted random sample from  unit hemisphere z > 0.
cgmath::vector_3d lambert_sample (void);

#endif
