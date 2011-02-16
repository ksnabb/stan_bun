#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "./lib.h"

int LINE_LENGTH=100;
extern ply_object bunny;

//reads a file to a char array
GLchar * simple_fileread(char * file_name, GLint * length)
{
    #ifdef DEBUG
    printf("loading shader from file %s\n", file_name);
    #endif

    GLint chars = 0;
    FILE *src;
    src = fopen (file_name, "rt"); //open shader file 
    if (src == NULL){
        fprintf(stderr, "Fatal error in opening file %s", file_name);
        return NULL;
        }
    int currentsize = 128;
    GLchar * shader = malloc(currentsize*sizeof(char));
    char c;
    while((c = fgetc(src)) != EOF)
    {
        shader[chars] = c;
        chars++;
        if (chars >= currentsize){
            shader = realloc(shader, 2*currentsize*sizeof(char));
            currentsize = 2*currentsize;
        }
    }
    *length = chars;
    #ifdef DEBUG
    printf("done loading shader from file %s\n", file_name);
    #endif

    return shader;
}


void calc_normal(float * first, float * second, float * third, 
    float * returnme){
    float v1[3];
    float v2[3];
    float *r = returnme;
    float normalization;
    //compute vectors
    v1[0] = first[0] - second[0];
    v1[1] = first[1] - second[1];
    v1[2] = first[2] - second[2];

    v2[0] = first[0] - third[0];
    v2[1] = first[1] - third[1];
    v2[2] = first[2] - third[2];

    //cross product
    r[0] = v1[1]*v2[2]- v2[1]*v1[2];
    r[1] = v2[0]*v1[2] - v1[0]*v2[2];
    r[2] = v1[0]*v2[1]- v2[0]*v1[1];

    normalization = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
    r[0] = r[0]/normalization;
    r[1] = r[1]/normalization;
    r[2] = r[2]/normalization;
}

// see here for reading indices & computing normals for every vertex
// http://openglsamples.sourceforge.net/files/glut_ply.cpp
// Saves the information from the file given to the bunny ply_object
ply_object read_ply_from_file(const char *file_name)
{
    //BEGIN PLY FILE HANDLING
    FILE *ply;

    char line[LINE_LENGTH]; //line to be read
    bunny.amount_of_vertices = 0;
    bunny.amount_of_faces = 0;

    ply = fopen (file_name, "rt"); //open ply file

    //read header
    while (fgets(line, LINE_LENGTH, ply) != NULL)
    {
        /* convert the string to a long int */
        if (!strncmp (line, "element vertex", 14)) {
            sscanf(line, "element vertex %i", &bunny.amount_of_vertices);

        } else if (!strncmp (line, "element face", 12)) {
            sscanf(line, "element face %i", &bunny.amount_of_faces);
        }

        else if (!strncmp(line, "end_header", 10)) {
            break;
        }
    }


    #ifdef DEBUG
    printf("file format says %i vertices, %i faces\n",
bunny.amount_of_vertices,
    bunny.amount_of_faces);
    #endif
    //read vertices
    int i;
    float x;
    float y;
    float z;

    //create vertex table for pushing into OpenGL
    //float * vertices;
    //observe that table is 1-dimensional
    bunny.vertices = malloc(3*bunny.amount_of_vertices*sizeof(float*));
    for(i = 0; i < bunny.amount_of_vertices; i++) {
        if(fgets(line, LINE_LENGTH, ply) != NULL)
        {
            sscanf(line, "%f %f %f", &x, &y, &z);
            bunny.vertices[3*i] = x;
            bunny.vertices[(3*i)+1] = y;
            bunny.vertices[(3*i)+2] = z;
        }
    }

    //read the faces
//    bunny.faces = (int *) malloc(bunny.amount_of_faces*sizeof(int*));
    //faces_indices is two dimensional array with [face_index][vertex_index]
    bunny.faces_indices = (int *) malloc(bunny.amount_of_faces*3*sizeof(int));
    int amount_of_face_indices;
    int a, b, c;
    //int vertex_index;
    for(i = 0; i < bunny.amount_of_faces; i++) {
        fscanf (ply, "%i %i %i %i", &amount_of_face_indices, &a, &b, &c);
        if (amount_of_face_indices == 3) //we're only interested in triangels
        {
            bunny.faces_indices[3*i] = a;      
            bunny.faces_indices[3*i+1] = b;      
            bunny.faces_indices[3*i+2] = c;      
        }
         else {
            
        fprintf(stderr, "error, bad index data: not triangle!");    
        }

    }
    
    bunny.normals = malloc(3*bunny.amount_of_faces*sizeof(float));

    for (i = 0; i < bunny.amount_of_faces; i++)
    {
        int i1 = bunny.faces_indices[3*i];
        int i2 = bunny.faces_indices[3*i+1];
        int i3 = bunny.faces_indices[3*i+2];
        
        float v1[3]=
        {bunny.vertices[i1],bunny.vertices[i1+1],bunny.vertices[i1+2]};
        float v2[3] =
        {bunny.vertices[i2],bunny.vertices[i2+1],bunny.vertices[i2+2]};
        float v3[3] =
        {bunny.vertices[i3],bunny.vertices[i3+1],bunny.vertices[i3+2]};

        float res[3];
        calc_normal(v1, v2, v3, res);
        //todo, here could be a test to see if normal is "right"
        //then we could just flip everything else as well
        //apparently flipping v2 and v3 is sufficient

        bunny.vertices[3*i1] = res[0];
        bunny.vertices[3*i1+1] = res[1];
        bunny.vertices[3*i1+2] = res[2];
 
        bunny.vertices[3*i2] = res[0];
        bunny.vertices[3*i2+1] = res[1];
        bunny.vertices[3*i2+2] = res[2];
 
        bunny.vertices[3*i3] = res[0];
        bunny.vertices[3*i3+1] = res[1];
        bunny.vertices[3*i3+2] = res[2];
        
    }

    fclose(ply); //close file

    //END PLY FILE HANDLING

    return bunny;
}


