#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

class MeshObject {

    public:
        MeshObject(const char *);
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
