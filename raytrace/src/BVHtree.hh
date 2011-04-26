#ifndef BVHTREE_HH
#define BVHTREE_HH
#include <cgmath/cgmath.hh>
#include "geometry.hh"
#include "triangle_geometry.hh"
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cgmath/cgmath.hh>

class BVHtree {
  public:
    BVHtree();
    BVHtree(std::vector<geometry*>);
    BVHtree * children;
    cgmath::box_3d bbox; //bounding box
    std::vector<geometry*>  geoms; //but oh nothing works
};
#endif
