#include "sampling.hh"
#include <cstdlib>
#include <cmath>

using namespace cgmath;

vector_3d lambert_sample ()
{
  double x, y, z;
  do
    {
      x = 2.0 * (rand() / (double)RAND_MAX) - 1.0;
      y = 2.0 * (rand() / (double)RAND_MAX) - 1.0;
    }
  while (x*x + y*y > 1.0);
  z = sqrt (1.0 - x*x - y*y);
  return vec (x, y, z);
}

