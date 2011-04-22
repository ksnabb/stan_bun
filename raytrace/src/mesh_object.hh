#ifndef MESH_HH
#define MESH_HH
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cgmath/cgmath.hh>
#include "geometry.hh"
#include <vector>

class mesh_geometry : public geometry {

    public:
        mesh_geometry(const char *);
        virtual double hit (const cgmath::ray_3d& r) const;
        virtual void hit_surface_point(surface_point& sp) const;
        
        /*triangles in mesh object*/
        std::vector<geometry*> geometries;
        mutable int nearest;

/*      These should be private  */
        void lazy_calc_normal(int, int, int, float*);
        void calc_normal(float *, float *, float *, float *);
        void recursive_orient(int, float *, int *, int **, bool *);
        float calc_dot_product(float*, float*);
        
        float * vertices;
        int amount_of_vertices;
        int * faces_indices;
        float * faces_normals;
        int amount_of_faces;
        float * vertex_normals;
        float * tex_coordinates;
};
#endif
