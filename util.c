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


void calc_normal(float * first, float * second, float * third, float * returnme)
{

    
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

float calc_dot_product(float * v1, float * v2) 
{
    return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
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
    printf("file format says %i vertices, %i faces\n", bunny.amount_of_vertices,
    bunny.amount_of_faces);
    #endif
    //read vertices
    int i;
    float x;
    float y;
    float z;

    //create vertex table for pushing into GLSL
    bunny.vertices = (float *) malloc(bunny.amount_of_vertices * 3 * sizeof(float));
    
    //set the vertex coordinates into bunny.vertices
    for(i = 0; i < bunny.amount_of_vertices; i++) {
        if(fgets(line, LINE_LENGTH, ply) != NULL)
        {
            sscanf(line, "%f %f %f", &x, &y, &z);
            bunny.vertices[3*i] = x;
            bunny.vertices[(3*i)+1] = y;
            bunny.vertices[(3*i)+2] = z;
        }
    }

    //the indices of the faces is needed for what?
    bunny.faces_indices = (int *) malloc(bunny.amount_of_faces * 3 * sizeof(int));
    
    //we need the face normal to be able to shade the faces correclty
    bunny.faces_normals = malloc(bunny.amount_of_faces * 3 * sizeof(float));
    
    //the vertex normals
    bunny.vertex_normals = (float *) malloc(bunny.amount_of_vertices * 3 * sizeof(float));
    
    
    //count the face normals
    //and 
    //save the amount of faces per vertex for average calculation of vertex normal
    int amount_of_face_indices;
    int a, b, c;
    int * amount_of_faces_per_vertex = (int *) malloc(bunny.amount_of_vertices * sizeof(int));
    
    for(i = 0; i < bunny.amount_of_faces; i++) {
        fscanf (ply, "%i %i %i %i", &amount_of_face_indices, &a, &b, &c);
        if (amount_of_face_indices == 3) //we're only interested in triangels
        {
            //save the index of each vertex
            bunny.faces_indices[3*i] = a;      
            bunny.faces_indices[(3*i)+1] = b;      
            bunny.faces_indices[(3*i)+2] = c;
            
            //add count to the amount of faces per vertex
            amount_of_faces_per_vertex[a]++;
            amount_of_faces_per_vertex[b]++;
            amount_of_faces_per_vertex[c]++;
            
            //point vectors to the vertices for the triangle
            float v1[3] = {bunny.vertices[(a * 3)], 
                            bunny.vertices[(a * 3) + 1], 
                            bunny.vertices[(a * 3) + 2]};
            float v2[3] = {bunny.vertices[(b * 3)], 
                            bunny.vertices[(b * 3) + 1], 
                            bunny.vertices[(b * 3) + 2]};
            float v3[3] = {bunny.vertices[(c * 3)], 
                            bunny.vertices[(c * 3) + 1], 
                            bunny.vertices[(c * 3) + 2]};

            //calculate the face normal
            float res[3];
            calc_normal(v1, v2, v3, res);
            
            // add the face normal to bunny
            bunny.faces_normals[(i * 3)] = res[0];
            bunny.faces_normals[(i * 3) + 1] = res[1];
            bunny.faces_normals[(i * 3) + 2] = res[2];
            
            //check that the face normal is pointing to the right direction
            if(calc_dot_product(&bunny.faces_normals[(i * 3)], &bunny.vertex_normals[a]) < 0) {
                bunny.faces_normals[(i * 3)] = -res[0];
                bunny.faces_normals[(i * 3) + 1] = -res[1];
                bunny.faces_normals[(i * 3) + 2] = -res[2];
                printf("lesser then zero\n");
            } 
            
            //add the normal to the vertex_normals 
            //(only add, count average later)
            
            bunny.vertex_normals[(a * 3)] += res[0];
            bunny.vertex_normals[(a * 3) + 1] += res[1];
            bunny.vertex_normals[(a * 3) + 2] += res[2];

            bunny.vertex_normals[b] += res[0];
            bunny.vertex_normals[b + 1] += res[1];
            bunny.vertex_normals[b + 2] += res[2];


            bunny.vertex_normals[(c * 3)] += res[0];
            bunny.vertex_normals[(c * 3) + 1] += res[1];
            bunny.vertex_normals[(c * 3) + 2] += res[2];


            }
         else {
            fprintf(stderr, "error, bad index data: not triangle!");    
        }

    }
    
    //count the vertex_normal average
    for(i = 0; i < bunny.amount_of_vertices; i++) {
        bunny.vertex_normals[(i * 3)] = bunny.vertex_normals[(i * 3)] / amount_of_faces_per_vertex[i];
        bunny.vertex_normals[(i * 3) + 1] = bunny.vertex_normals[(i * 3) + 1] / amount_of_faces_per_vertex[i];
        bunny.vertex_normals[(i * 3) + 2] = bunny.vertex_normals[(i * 3) + 2] / amount_of_faces_per_vertex[i];
    }

    fclose(ply); //close file

    //END PLY FILE HANDLING

    return bunny;
}



