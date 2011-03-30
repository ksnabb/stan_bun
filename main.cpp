#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;

class Mesh
{
    private:
        float * vertices; 
        int amount_of_vertices;
        int * faces_indices;
        float * faces_normals;
        int amount_of_faces;
        float * vertex_normals;
        float * tex_coordinates;
    public:
        Mesh(const char*);
        void lazy_calc_normal(int, int, int, float*);
        void calc_normal(float*, float*, float*, float*);
        void recursive_orient(int, float *, int *, int **, bool *);
        float calc_dot_product(float *, float *);
        void draw();
        void findIntersection();
        int get_vertice_amount();
        float * get_vertices();
        float * get_vertex_normals();
        int get_faces_amount();
        int * get_faces_indices();
};

Mesh::Mesh(const char *file_name) 
{
    FILE *ply;
    int LINE_LENGTH=100;
    
    char line[LINE_LENGTH]; //line to be read
    amount_of_vertices = 0;
    amount_of_faces = 0;
    
    ply = fopen (file_name, "rt"); //open ply file

    //read header
    while (fgets(line, LINE_LENGTH, ply) != NULL)
    {
        /* convert the string to a long int */
        if (!strncmp (line, "element vertex", 14)) {
            sscanf(line, "element vertex %i", &amount_of_vertices);

        } else if (!strncmp (line, "element face", 12)) {
            sscanf(line, "element face %i", &amount_of_faces);
        }

        else if (!strncmp(line, "end_header", 10)) {
            break;
        }
    }


    #ifdef DEBUG
    printf("file format says %i vertices, %i faces\n", 
        amount_of_vertices,
        amount_of_faces);
    #endif
  
    //read vertices
    int i;
    float x;
    float y;
    float z;

    //create vertex table for pushing into GLSL
    vertices = (float *) malloc(amount_of_vertices * 3 * sizeof(float));
    tex_coordinates = (float *) malloc(amount_of_vertices * 2 *
    sizeof(float)); //only u and v for texture 
    
    //set the vertex coordinates into bunny.vertices
    for(i = 0; i < amount_of_vertices; i++) {
        if(fgets(line, LINE_LENGTH, ply) != NULL)
        {
            sscanf(line, "%f %f %f", &x, &y, &z);
            vertices[(3*i)] = x;
            vertices[(3*i)+1] = y;
            vertices[(3*i)+2] = z;
            //trivial x=u, y=v mapping
            tex_coordinates[2*i] = x;
            tex_coordinates[2*i+1] = y;
        }
    }

    //the indices of the faces is needed for what?
    faces_indices = (int *) malloc(amount_of_faces * 3 * sizeof(int));
    
    //we need the face normal to be able to shade the faces correclty
    faces_normals = (float *) malloc(amount_of_faces * 3 * sizeof(float));
    
    //the vertex normals
    vertex_normals = (float *) malloc(amount_of_vertices * 3 * sizeof(float));
    
    
    //count the face normals
    int amount_of_face_indices;
    int a, b, c;
    int * amount_of_faces_per_vertex = (int *) malloc(amount_of_vertices * sizeof(int));
    for(i = 0; i < amount_of_vertices; i++) {
        amount_of_faces_per_vertex[i] = 0;
    }
    
    bool * visited = (bool*) malloc(amount_of_faces * sizeof(bool*));
    for(i = 0; i < amount_of_faces; i++) {
        visited[i] = false;
    }

    int ** vertex_in_faces =(int**) malloc(Mesh::amount_of_vertices * sizeof(int*));
    for (i = 0; i < amount_of_vertices; i++){
        vertex_in_faces[i] = (int *) malloc(9 * sizeof(int)); //hope this is enough
        int j;
        for (j = 0; j < 9; j++) {
            vertex_in_faces[i][j] = -1;
        }
    }
    //todoo, free vertex_in_faces and visited after they're not needed
    
    
    for(i = 0; i < amount_of_faces; i++) {
        fscanf (ply, "%i %i %i %i", &amount_of_face_indices, &a, &b, &c);
        if (amount_of_face_indices == 3) //we're only interested in triangels
        {
            //save the index of each vertex
            faces_indices[3*i] = a;      
            faces_indices[(3*i)+1] = b;      
            faces_indices[(3*i)+2] = c;
            
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
   Mesh::lazy_calc_normal(faces_indices[3*vertex_in_faces[0][0]],
                    faces_indices[3*vertex_in_faces[0][0]+1], 
                    faces_indices[3*vertex_in_faces[0][0]+2],
                    reference); 

    //count the face normals
    for(i = 0; i < amount_of_faces; i++) {
            a = faces_indices[3*i];      
            b = faces_indices[(3*i)+1];      
            c = faces_indices[(3*i)+2];
            //point vectors to the vertices for the triangle
            float v1[3] = {vertices[(a * 3)], 
                           vertices[(a * 3) + 1], 
                           vertices[(a * 3) + 2]};
            float v2[3] = {vertices[(b * 3)], 
                           vertices[(b * 3) + 1], 
                           vertices[(b * 3) + 2]};
            float v3[3] = {vertices[(c * 3)], 
                           vertices[(c * 3) + 1], 
                           vertices[(c * 3) + 2]};

            //calculate the face normal
            float res[3];
            calc_normal(v1, v2, v3, res);

            // add the face normal to bunny
            faces_normals[(i * 3)] = res[0];
            faces_normals[(i * 3) + 1] = res[1];
            faces_normals[(i * 3) + 2] = res[2];
            
    }
    
    //check the faces_normals just calculated and turn them around
    //if needed
        
    //choose a reference face (index of the reference face)
    int reference_face_index = 0;
    recursive_orient(reference_face_index, 
                    faces_normals,
                    faces_indices,
                    vertex_in_faces, 
                    visited);
    
    //add the faces normals to the vertex normals
    for(i = 0; i < amount_of_faces; i++) {
            a = faces_indices[3*i];      
            b = faces_indices[(3*i)+1];      
            c = faces_indices[(3*i)+2];
            
            
            float res[3] = {
                faces_normals[(i * 3)],
                faces_normals[(i * 3) + 1],
                faces_normals[(i * 3) + 2]
            };
            
           //(only add, normalize later)
            vertex_normals[(a * 3)] += res[0];
            vertex_normals[(a * 3) + 1] += res[1];
            vertex_normals[(a * 3) + 2] += res[2];
            
            vertex_normals[(b * 3)] += res[0];
            vertex_normals[(b * 3) + 1] += res[1];
            vertex_normals[(b * 3) + 2] += res[2];

            vertex_normals[(c * 3)] += res[0];
            vertex_normals[(c * 3) + 1] += res[1];
            vertex_normals[(c * 3) + 2] += res[2];
    }
    
    //normalize the vertex normals (average)
    for(i = 0; i < amount_of_vertices; i++) {
        float temp[3];
        temp[0] = vertex_normals[(i * 3)];
        temp[1] = vertex_normals[(i * 3) + 1];
        temp[2] = vertex_normals[(i * 3) + 2]; 
        
        float normalization = sqrt(temp[0] * temp[0] + temp[1] * temp[1] + temp[2] * temp[2]);
        temp[0] = temp[0]/normalization;
        temp[1] = temp[1]/normalization;
        temp[2] = temp[2]/normalization;

        vertex_normals[(i * 3)] = temp[0];
        vertex_normals[(i * 3) + 1] = temp[1];
        vertex_normals[(i * 3) + 2] = temp[2];
        
    }

    fclose(ply); //close file

    //END PLY FILE HANDLING

};


void Mesh::lazy_calc_normal(int a, int b, int c, float*ret)
{
             //point vectors to the vertices for the triangle
            float v1[3] = {vertices[(a * 3)], 
                            vertices[(a * 3) + 1], 
                            vertices[(a * 3) + 2]};
            float v2[3] = {vertices[(b * 3)], 
                            vertices[(b * 3) + 1], 
                            vertices[(b * 3) + 2]};
            float v3[3] = {vertices[(c * 3)], 
                           vertices[(c * 3) + 1], 
                           vertices[(c * 3) + 2]};

            calc_normal(v1, v2, v3, ret); 
}

void Mesh::calc_normal(float * first, float * second, float * third, float * returnme)
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

    // just calculate the normal the normalization should happen elsewhere
    // the area of the triangle is important to weight the normal compared
    // to other normals (?)
    normalization = sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
    r[0] = r[0]/normalization;
    r[1] = r[1]/normalization;
    r[2] = r[2]/normalization;
}

void Mesh::recursive_orient(int face_index, 
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
    
    //vertex indices that is connected to the current_face
    int vi1 = faces_indices[(face_index * 3)];
    int vi2 = faces_indices[(face_index * 3) + 1];
    int vi3 = faces_indices[(face_index * 3) + 2];
    
    int i;
    for(i = 0; i < 9; i++) {
        //the face indexes that is connected to the current_face
        int fi1 = vertex_in_faces[vi1][i];
        int fi2 = vertex_in_faces[vi2][i];
        int fi3 = vertex_in_faces[vi3][i];
        
        if(fi1 != -1 && fi1 != face_index && !visited[fi1]) {
            float nface_normal[3] = {
                faces_normals[(fi1 * 3)],
                faces_normals[(fi1 * 3) + 1],
                faces_normals[(fi1 * 3) + 2]  
            };
            if(calc_dot_product(nface_normal, current_face_normal) < -0.1) {
                
                faces_normals[(fi1 * 3)] = -1 * faces_normals[(fi1 * 3)];
                faces_normals[(fi1 * 3) + 1] = -1 * faces_normals[(fi1 * 3) + 1];
                faces_normals[(fi1 * 3) + 2] = -1 * faces_normals[(fi1 * 3) + 2];
            }
        }
        if(fi2 != -1 && fi2 != face_index && !visited[fi1]) {
            float nface_normal[3] = {
                faces_normals[(fi2 * 3)],
                faces_normals[(fi2 * 3) + 1],
                faces_normals[(fi2 * 3) + 2]
            };
            if(calc_dot_product(nface_normal, current_face_normal) < -0.1) {
                faces_normals[(fi2 * 3)] = -1 * faces_normals[(fi2 * 3)];
                faces_normals[(fi2 * 3) + 1] = -1 * faces_normals[(fi2 * 3) + 1];
                faces_normals[(fi2 * 3) + 2] = -1 * faces_normals[(fi2 * 3) + 2];
            }
        }
        if(fi3 != -1 && fi3 != face_index && !visited[fi1]) {
            float nface_normal[3] = {
                faces_normals[(fi3 * 3)],
                faces_normals[(fi3 * 3) + 1],
                faces_normals[(fi3 * 3) + 2]
            };
            if(calc_dot_product(nface_normal, current_face_normal) < -0.1) {  
                faces_normals[(fi3 * 3)] = -faces_normals[(fi3 * 3)];
                faces_normals[(fi3 * 3) + 1] = -faces_normals[(fi3 * 3) + 1];
                faces_normals[(fi3 * 3) + 2] = -faces_normals[(fi3 * 3) + 2];
            }
        }
        if(fi1 != -1) {
            recursive_orient(fi1, 
                    faces_normals, 
                    faces_indices, 
                    vertex_in_faces,
                    visited);
        }
        if(fi2 != -1) {           
            recursive_orient(fi2, 
                    faces_normals, 
                    faces_indices, 
                    vertex_in_faces,
                    visited);
        }
        if(fi3 != -1) {    
            recursive_orient(fi3, 
                    faces_normals, 
                    faces_indices, 
                    vertex_in_faces,
                    visited);
        }
    }
}

float Mesh::calc_dot_product(float * v1, float * v2) 
{
    return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}

void Mesh::draw()
{
    
}

void Mesh::findIntersection()
{
}

int Mesh::get_vertice_amount()
{
    return amount_of_vertices;
}

float * Mesh::get_vertices()
{
    return vertices;
}

float * Mesh::get_vertex_normals()
{
    return vertex_normals;
}

int * Mesh::get_faces_indices()
{
    return faces_indices;
}

int Mesh::get_faces_amount()
{
    return amount_of_faces;
}

GLuint p; //program
GLuint vertex_ID;
Mesh mesh_object = Mesh("bunny.ply");  
    
void display_cb(void)
{
    
    #ifdef DEBUG
//    printf("display callback called\n");
    #endif

    glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

    glUseProgram(p);


	//here goes actual drawing code
	glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    //z-buffering
    glDepthFunc(GL_LESS);
    glDepthRange(0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    //vertex array
    glBindVertexArray (vertex_ID);
    
    //draw the bunny according to indices and the vertices in the server buffer
    glDrawElements(GL_TRIANGLES,
        3*mesh_object.get_faces_amount(),
        GL_UNSIGNED_INT,
        mesh_object.get_faces_indices());

	glFlush();
	glutSwapBuffers();

}


int main(int argc, char **argv) 
{

    #ifdef DEBUG
    printf("starting main function\n");
    #endif

	//initialize glut, create a window
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize (600, 600);
    glutCreateWindow ("OpenGL Test");
	
    //glewinit error reporting
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));  
        
    printf ("Supported OpenGL version: %s\n",
	glGetString (GL_VERSION));
	
    //initialize modelview and projection matrices
    float camera_x = 0.0;
    float camera_y = 0.0;
    float camera_z = -1.0;
    float lookat_x = 0.0;
    
    float lookat_y = 0.0;
    float lookat_z = 0.0;
    float up_x = 0.0;
    float up_y = 1.0;
    float up_z = 0.0;

    float height = 600;
    float width = 600;

    float fovy = 45;
    float aspect = 1.0;
    float near = 1;
    float far = 2000;
    
    glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    
    gluLookAt(camera_x, camera_y, camera_z,
        lookat_x, lookat_y, lookat_z,
       //(mod.camera_x + mod.lookat_x), (mod.camera_y + mod.lookat_y), (mod.camera_z + mod.lookat_z),
        up_x, up_y, up_z);
    

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0, width, height);
    gluPerspective(fovy, aspect, near, far);
 
    #ifdef DEBUG
    printf("beginning read from file.\n");
    #endif
          
    //setting up the vertex array and then buffers
    GLuint vertex_array_object_ID;
    unsigned int number_of_arrays = 1;
    glGenVertexArrays (number_of_arrays, &vertex_array_object_ID);
    glBindVertexArray (vertex_array_object_ID);
   
    // three buffers: vertex, vertex_normal,, TODO textures and colors
    unsigned int number_of_buffers = 3;
    unsigned int vertex_buffer_object_ID[number_of_buffers];
    glGenBuffers (number_of_buffers, vertex_buffer_object_ID);
    
    //what is this?
    //unsigned int index_buffer_object_ID[1];
    //glGenBuffers(1, index_buffer_object_ID);
    
    //bind vertices data to the buffer
    unsigned int elements_per_vertex = 3;
    //unsigned int elements_per_triangle = 3 * elements_per_vertex;
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_object_ID[0]);
    glBufferData (GL_ARRAY_BUFFER,
        elements_per_vertex * mesh_object.get_vertice_amount() * sizeof (float), 
        mesh_object.get_vertices(), 
        GL_STATIC_DRAW);
 
    //vertex position x y z in the buffer
    int vertex_position_location = 0;
    glBindAttribLocation(p, vertex_position_location, "vertex_position");
    glVertexAttribPointer (vertex_position_location, 
                elements_per_vertex,
                GL_FLOAT, 
                GL_FALSE, 
                0, 
                0);
    glEnableVertexAttribArray (vertex_position_location);
    
    
    //vertex normal buffer
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_object_ID[1]);
    glBufferData(GL_ARRAY_BUFFER,
                mesh_object.get_vertice_amount() * 3 * sizeof(float),
                mesh_object.get_vertex_normals(),
                GL_STATIC_DRAW);
                
    unsigned int location_vertex_normal = 1;
    glBindAttribLocation(p, location_vertex_normal, "vertex_normal");
    glVertexAttribPointer(location_vertex_normal, 
                    3, 
                    GL_FLOAT,
                    GL_FALSE, 
                    0,
                    0);
    glEnableVertexAttribArray(location_vertex_normal);

	glutDisplayFunc(display_cb);
	glutMainLoop ();
	return 0;
}
