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
    #ifdef DEBUG
/*        printf("vectors [%f,%f,%f][%f,%f,%f][%f,%f,%f]", 
        first[1],first[2],first[3],
        second[1],second[2],second[3],
        third[1],third[2],third[3]);*/
    #endif
    
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

#ifdef DEBUG
//    printf("and their normal is [%f,%f,%f]",r[0], r[1], r[2]);
#endif
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
    bunny.vertices = malloc(3*bunny.amount_of_vertices*sizeof(float));
    for(i = 0; i < bunny.amount_of_vertices; i++) {
        if(fgets(line, LINE_LENGTH, ply) != NULL)
        {
            sscanf(line, "%f %f %f", &x, &y, &z);
            bunny.vertices[3*i] = x;
            bunny.vertices[(3*i)+1] = y;
            bunny.vertices[(3*i)+2] = z;
#ifdef DEBUG
//          if (i == 1656)
//            printf("read vertex %i :%f, %f, %f\n",i, x, y, z);
#endif
        }
    }

    //read the faces
//    bunny.faces = (int *) malloc(bunny.amount_of_faces*sizeof(int*));
    //faces_indices is two dimensional array with [face_index][vertex_index]
    bunny.faces_indices = (int *) malloc(bunny.amount_of_faces*3*sizeof(int));
    bunny.normals = malloc(3*bunny.amount_of_faces*sizeof(float));
    int amount_of_face_indices;
    int a, b, c;
    //int vertex_index;
    for(i = 0; i < bunny.amount_of_faces; i++) {
        fscanf (ply, "%i %i %i %i", &amount_of_face_indices, &a, &b, &c);
        if (amount_of_face_indices == 3) //we're only interested in triangels
        {
            bunny.faces_indices[3*i] = a;      
            bunny.faces_indices[(3*i)+1] = b;      
            bunny.faces_indices[(3*i)+2] = c;      
            
            float v1[3]=
            {bunny.vertices[a],bunny.vertices[a+1],bunny.vertices[a+2]};
            float v2[3] =
            {bunny.vertices[b],bunny.vertices[b+1],bunny.vertices[b+2]};
            float v3[3] =
            {bunny.vertices[c],bunny.vertices[c+1],bunny.vertices[c+2]};
#ifdef DEBUG
            if (i < 50)
            {
//                printf("%i, %i, %i\n", a, b, c);
//           printf("v1 %f, %f, %f \n", v1[0], v1[1], v1[2]); 
//           printf("v2 %f, %f, %f \n", v2[0], v2[1], v2[2]); 
//           printf("v3 %f, %f, %f \n", v3[0], v3[1], v3[2]); 
            }
#endif

            float res[3];
            calc_normal(v1, v2, v3, res);
            //todo, here could be a test to see if normal is "right"
            //then we could just flip everything else as well
            //apparently flipping v2 and v3 is sufficient

            bunny.normals[a] = res[0];
            bunny.normals[a+1] = res[1];
            bunny.normals[a+2] = res[2];

            bunny.normals[b] = res[0];
            bunny.normals[b+1] = res[1];
            bunny.normals[b+2] = res[2];


            bunny.normals[c] = res[0];
            bunny.normals[c+1] = res[1];
            bunny.normals[c+2] = res[2];



            //where should we put this?
#ifdef DEBUG
//            if (i < 50)
//           printf("result %f, %f, %f \n", res[0], res[1], res[2]); 
#endif
            }
         else {
            
        fprintf(stderr, "error, bad index data: not triangle!");    
        }

    }
    

        #ifdef DEBUG
        printf("printing some normals");
#endif
    for (i = 0; i < bunny.amount_of_faces; i++)
    {
   }

    fclose(ply); //close file

    //END PLY FILE HANDLING

    return bunny;
}



