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
    printf("loading data from file %s\n", file_name);
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
    printf("done loading data from file %s\n", file_name);
    #endif

    return shader;
}


//reads a 24bit rgb file to a struct
img_data * simple_bmp_read(char * file_name)
{
    #ifdef DEBUG
    printf("loading texture from file %s\n", file_name);
    #endif
    int foo = 0;
    img_data * pic_p;
    pic_p = (img_data*)malloc(sizeof(img_data));
    (*pic_p).whole = simple_fileread(file_name, &foo);

    #ifdef DEBUG
    printf("file is %i bytes \n", *((int*)&(*pic_p).whole[0x0002]));
    #endif
    (*pic_p).rgbstart = *((int*)&(*pic_p).whole[0x000A]); //start of file table
    (*pic_p).width = *((int*)&(*pic_p).whole[0x0012]); 
    (*pic_p).height = *((int*)&(*pic_p).whole[0x0016]); 

    #ifdef DEBUG
    printf("texture is %i by %i pixels\n", (*pic_p).width, (*pic_p).height);
    #endif
    #ifdef DEBUG
    printf("done loading texturefrom file %s\n", file_name);
    #endif

    return pic_p;
}




void calc_normal(float * first, float * second, float * third, float * returnme)
{

    
    float v1[3];
    float v2[3];
    float *r = returnme;
    //float normalization;
    
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

    // just calculate the normal the normalization should happen elsewhere
    // the area of the triangle is important to weight the normal compared
    // to other normals (?)
    //normalization = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
    //r[0] = r[0]/normalization;
    //r[1] = r[1]/normalization;
    //r[2] = r[2]/normalization;

}

void lazy_calc_normal(int a, int b, int c, float*ret){
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

            calc_normal(v1, v2, v3, ret); 
    }

float calc_dot_product(float * v1, float * v2) 
{
    return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}


void recursive_orient(int face_index, 
                    float * faces_normals, 
                    int * faces_indices, 
                    int ** vertex_in_faces,
                    bool * visited) {
                    
    if(visited[face_index])
        return;
    
    //mark this face as visited
    visited[face_index] = true;
    
    //current_face normal
    float current_face_normal[3] = {
        faces_normals[(face_index * 3)],
        faces_normals[(face_index * 3) + 1],
        faces_normals[(face_index * 3) + 2]
    };
    /*
    float normalization;
    normalization = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
    r[0] = r[0]/normalization;
    r[1] = r[1]/normalization;
    r[2] = r[2]/normalization;
    
    printf("current face normal %f %f %f\n",
        faces_normals[(face_index * 3)],
        faces_normals[(face_index * 3) + 1],
        faces_normals[(face_index * 3) + 2]);
    */
    
    //vertex indices that is connected to the current_face
    int vi1 = faces_indices[(face_index * 3)];
    int vi2 = faces_indices[(face_index * 3) + 1];
    int vi3 = faces_indices[(face_index * 3) + 2];
    
    int i;
    //printf("%i\n", face_index);
    for(i = 0; i < 9; i++) {
        //printf("get the connected faces to face: \n", face_index);
        //the face indexes that is connected to the current_face
        int fi1 = vertex_in_faces[vi1][i];
        int fi2 = vertex_in_faces[vi2][i];
        int fi3 = vertex_in_faces[vi3][i];
        //printf("connected face %i %i %i\n", fi1, fi2, fi3);
        if(fi1 != 0 && !visited[fi1]) {
            float nface_normal[3] = {
                faces_normals[(fi1 * 3)],
                faces_normals[(fi1 * 3) + 1],
                faces_normals[(fi1 * 3) + 2]  
            };
            if(calc_dot_product(nface_normal, current_face_normal) < -0.1) {
                int i1 = bunny.faces_indices[(fi1 *3)+1];
                int i2 = bunny.faces_indices[(fi1 *3)+2];
                bunny.faces_indices[(fi1 *3)+2] = i1;
                bunny.faces_indices[(fi1 *3)+1] = i2;
                
                faces_normals[(fi1 * 3)] = -faces_normals[(fi1 * 3)];
                faces_normals[(fi1 * 3) + 1] = -faces_normals[(fi1 * 3) + 1];
                faces_normals[(fi1 * 3) + 2] = -faces_normals[(fi1 * 3) + 2];
            }
            recursive_orient(fi1, 
                    faces_normals, 
                    faces_indices, 
                    vertex_in_faces,
                    visited);
        }
        if(fi2 != 0 && !visited[fi2]) {
            float nface_normal[3] = {
                faces_normals[(fi2 * 3)],
                faces_normals[(fi2 * 3) + 1],
                faces_normals[(fi2 * 3) + 2]
            };
            if(calc_dot_product(nface_normal, current_face_normal) < -0.1) {
                int i1 = bunny.faces_indices[(fi2 * 3)+1];
                int i2 = bunny.faces_indices[(fi2 * 3)+2];
                bunny.faces_indices[(fi2 * 3)+2] = i1;
                bunny.faces_indices[(fi2 * 3)+1] = i2;
            
                
                faces_normals[(fi2 * 3)] = -faces_normals[(fi2 * 3)];
                faces_normals[(fi2 * 3) + 1] = -faces_normals[(fi2 * 3) + 1];
                faces_normals[(fi2 * 3) + 2] = -faces_normals[(fi2 * 3) + 2];
            }
            recursive_orient(fi2, 
                    faces_normals, 
                    faces_indices, 
                    vertex_in_faces,
                    visited);
        }
        if(fi3 != 0 && !visited[fi3]) {
            float nface_normal[3] = {
                faces_normals[(fi3 * 3)],
                faces_normals[(fi3 * 3) + 1],
                faces_normals[(fi3 * 3) + 2]
            };
            if(calc_dot_product(nface_normal, current_face_normal) < -0.1) {
                int i1 = bunny.faces_indices[(fi3 * 3)+1];
                int i2 = bunny.faces_indices[(fi3 * 3)+2];
                bunny.faces_indices[(fi3 * 3)+2] = i1;
                bunny.faces_indices[(fi3 * 3)+1] = i2;
                
                
                faces_normals[(fi3 * 3)] = -faces_normals[(fi3 * 3)];
                faces_normals[(fi3 * 3) + 1] = -faces_normals[(fi3 * 3) + 1];
                faces_normals[(fi3 * 3) + 2] = -faces_normals[(fi3 * 3) + 2];
            }
            recursive_orient(fi3, 
                    faces_normals, 
                    faces_indices, 
                    vertex_in_faces,
                    visited);
        }
    }
    /*
    for (int i = 0; i < amount_of_faces_per_vertex[vertex_i]; i++){
        int face_i = vertex_in_faces[vertex_i][i]*3;
        int d = bunny.faces_indices[face_i];
        int e = bunny.faces_indices[face_i+1];
        int f = bunny.faces_indices[face_i+2];
        float res[3]; 
        lazy_calc_normal(d, e, f, res);

        if (calc_dot_product(reference, res) < -0.1)
        {
            //normals are over 90 degrees apart, trust first one
            //i.e. flip the latter face around 
            bunny.faces_indices[face_i+1] = f;
            bunny.faces_indices[face_i+2] = e;
            lazy_calc_normal(d, f, e, res);
        }       
        recursive_orient(d, res, vertex_in_faces, visited,
        amount_of_faces_per_vertex);
        recursive_orient(e, res, vertex_in_faces, visited,
        amount_of_faces_per_vertex);
        recursive_orient(f, res, vertex_in_faces, visited,
        amount_of_faces_per_vertex);
    }
    */
}

/*
This function reads the ply file and returns a 
ply_object that describes the file read
*/
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

    //create vertex table for pushing into GLSL
    bunny.vertices = (float *) malloc(bunny.amount_of_vertices * 3 * sizeof(float));
    bunny.tex_coordinates = (float *) malloc(bunny.amount_of_vertices * 2 *
    sizeof(float)); //only u and v for texture 
    
    //set the vertex coordinates into bunny.vertices
    for(i = 0; i < bunny.amount_of_vertices; i++) {
        if(fgets(line, LINE_LENGTH, ply) != NULL)
        {
            sscanf(line, "%f %f %f", &x, &y, &z);
            bunny.vertices[(3*i)] = x;
            bunny.vertices[(3*i)+1] = y;
            bunny.vertices[(3*i)+2] = z;
            //trivial x=u, y=v mapping
            bunny.tex_coordinates[2*i] = x;
            bunny.tex_coordinates[2*i+1] = y;
        }
    }

    //the indices of the faces is needed for what?
    bunny.faces_indices = (int *) malloc(bunny.amount_of_faces * 3 * sizeof(int));
    
    //we need the face normal to be able to shade the faces correclty
    bunny.faces_normals = malloc(bunny.amount_of_faces * 3 * sizeof(float));
    
    //the vertex normals
    bunny.vertex_normals = (float *) malloc(bunny.amount_of_vertices * 3 * sizeof(float));
    
    
    //count the face normals
    int amount_of_face_indices;
    int a, b, c;
    int * amount_of_faces_per_vertex = (int *) malloc(bunny.amount_of_vertices * sizeof(int));
    
    bool * visited = (bool*) malloc(bunny.amount_of_faces * sizeof(bool*));
    for(i = 0; i < bunny.amount_of_faces; i++) {
        visited[i] = false;
    }

    int ** vertex_in_faces =(int**) malloc(bunny.amount_of_vertices * sizeof(int*));
    for (i = 0; i < bunny.amount_of_vertices; i++){
        vertex_in_faces[i] = malloc(9 * sizeof(int)); //hope this is enough
        int j;
        for (j = 0; j < 9; j++) {
            vertex_in_faces[i][j] = 0;
        }
    }
    //todoo, free vertex_in_faces and visited after they're not needed
    
    
    for(i = 0; i < bunny.amount_of_faces; i++) {
        fscanf (ply, "%i %i %i %i", &amount_of_face_indices, &a, &b, &c);
        if (amount_of_face_indices == 3) //we're only interested in triangels
        {
            //save the index of each vertex
            bunny.faces_indices[3*i] = a;      
            bunny.faces_indices[(3*i)+1] = b;      
            bunny.faces_indices[(3*i)+2] = c;
            
            vertex_in_faces[a][amount_of_faces_per_vertex[a]] = i; 
            vertex_in_faces[b][amount_of_faces_per_vertex[b]] = i; 
            vertex_in_faces[c][amount_of_faces_per_vertex[c]] = i; 

            amount_of_faces_per_vertex[a]++;
            amount_of_faces_per_vertex[b]++;
            amount_of_faces_per_vertex[c]++;

        }
         else {
            fprintf(stderr, "error, bad index data: not triangle!");    
        }
    } 
    
   //just choose the face index where to start recursive calculation
   float reference[3];
   lazy_calc_normal(bunny.faces_indices[3*vertex_in_faces[0][0]],
                    bunny.faces_indices[3*vertex_in_faces[0][0]+1], 
                    bunny.faces_indices[3*vertex_in_faces[0][0]+2],
                    reference); 
//  algorithm doesn't seem to work :(
//   recursive_orient(0, reference, vertex_in_faces, visited, 
//   amount_of_faces_per_vertex);


    //count the face normals
    for(i = 0; i < bunny.amount_of_faces; i++) {
            a = bunny.faces_indices[3*i];      
            b = bunny.faces_indices[(3*i)+1];      
            c = bunny.faces_indices[(3*i)+2];
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
            
    }
    
    //check the faces_normals just calculated and turn them around
    //if needed
    
    //choose a reference face (index of the reference face)
    
    int reference_face_index = 7025;
    recursive_orient(reference_face_index, 
                    bunny.faces_normals,
                    bunny.faces_indices,
                    vertex_in_faces, 
                    visited);
    
    //add the faces normals to the vertex normals
    for(i = 0; i < bunny.amount_of_faces; i++) {
            a = bunny.faces_indices[3*i];      
            b = bunny.faces_indices[(3*i)+1];      
            c = bunny.faces_indices[(3*i)+2];
            
            
            float res[3] = {
                bunny.faces_normals[(i * 3)],
                bunny.faces_normals[(i * 3) + 1],
                bunny.faces_normals[(i * 3) + 2]
            };
            
           //(only add, normalize later)
            bunny.vertex_normals[(a * 3)] += res[0];
            bunny.vertex_normals[(a * 3) + 1] += res[1];
            bunny.vertex_normals[(a * 3) + 2] += res[2];
            
            bunny.vertex_normals[(b * 3)] += res[0];
            bunny.vertex_normals[(b * 3) + 1] += res[1];
            bunny.vertex_normals[(b * 3) + 2] += res[2];

            bunny.vertex_normals[(c * 3)] += res[0];
            bunny.vertex_normals[(c * 3) + 1] += res[1];
            bunny.vertex_normals[(c * 3) + 2] += res[2];
    }
    
    //normalize the vertex normals (average)
    for(i = 0; i < bunny.amount_of_vertices; i++) {
        float temp[3];
        temp[0] = bunny.vertex_normals[(i * 3)];
        temp[1] = bunny.vertex_normals[(i * 3) + 1];
        temp[2] = bunny.vertex_normals[(i * 3) + 2]; 
        
        float normalization = sqrt(temp[0] * temp[0] + temp[1] * temp[1] + temp[2] * temp[2]);
        temp[0] = temp[0]/normalization;
        temp[1] = temp[1]/normalization;
        temp[2] = temp[2]/normalization;

        bunny.vertex_normals[(i * 3)] = temp[0];
        bunny.vertex_normals[(i * 3) + 1] = temp[1];
        bunny.vertex_normals[(i * 3) + 2] = temp[2];
        
    }

    fclose(ply); //close file

    //END PLY FILE HANDLING

    return bunny;
}



