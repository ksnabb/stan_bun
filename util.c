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
    bunny.faces = (int *) malloc(bunny.amount_of_faces*sizeof(int*));
    //faces_indices is two dimensional array with [face_index][vertex_index]
    bunny.faces_indices = (int **) malloc(bunny.amount_of_faces*sizeof(int*));
    int amount_of_face_indices;
    int vertex_index;
    for(i = 0; i < bunny.amount_of_faces; i++) {
        fscanf (ply, "%i", &amount_of_face_indices);

        bunny.faces[i] = amount_of_face_indices;

        //allocate memory for the amount of vertex indexes that the face
        //includes
        bunny.faces_indices[i] = (int *) malloc(amount_of_face_indices *
sizeof(int *));

        int j;
        for(j = 0; j < amount_of_face_indices; j++) {
            fscanf(ply, "%i", &vertex_index);
            bunny.faces_indices[i][j] = vertex_index;
        }
    }

    fclose(ply); //close file

    //END PLY FILE HANDLING

    return bunny;
}



