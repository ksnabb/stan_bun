#include "sampling.hh"
#include <cstdlib>
#include <cmath>

using namespace cgmath;

const bool QUASIRANDOM = true;

typedef unsigned int uint;
uint i = 0;
uint seed = rand();

vector_3d lambert_sample ()
{
  if (QUASIRANDOM)
      return quasirandom_lambert_sample();
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
//example taken from Kollig & Keller: Efficient Multidimensional Sampling
double RI_S(uint i, uint r)
{
   for(uint v = 1<<31; i; i >>= 1, v ^= v>>1)
    if(i & 1)
    r ^= v;
    return (double) r / (double) 0x100000000;
}


vector_3d quasirandom_lambert_sample()
{
  double x, y, z;

  do
    {
      x = (2.0* RI_S(i++, seed)) -1.0;
      y = (2.0 * RI_S(i++, seed)) - 1.0;
    }
  while (x*x + y*y > 1.0);
  z = sqrt (1.0 - x*x - y*y);
  return vec (x, y, z);  
}

