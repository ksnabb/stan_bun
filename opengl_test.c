#include <GL/glew.h>
#include <GL/glut.h>
//#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h> 
#include <math.h>

#define DEBUG

float MOVE_STEP = 0.1;

void display_cb(void);
void keyb_cb(unsigned char key, int x, int y);
void sp_keyb_cb(int key, int x, int y);
void mouse_motion_cb(int x, int y);
void mouse_cb(int button, int state, int x, int y);
void timer_cb(int unused);
void window_resize_cb(int width, int height);
void update_modelview();
void update_projection();

//Triangle indices for the bunny
int * bunny_indices;
int vertex_ID;
int * vertex_count;
int LINE_LENGTH = 100;
GLuint p; //program

//describes the information in a ply file
typedef struct {
    float * vertices;
    int amount_of_vertices;
    int * faces;
    int ** faces_indices;
    int amount_of_faces;
    float * normals;
} ply_object;

typedef struct {
    float fovy;
    float aspect;
    float near;
    float far;
    
} projection_data;

typedef struct {
    float x;
    float y;
    float z;
    } scale;

typedef struct {
    float camera_x;
    float camera_y;
    float camera_z;
    float lookat_x; 
    float lookat_y; 
    float lookat_z;
    float up_x;
    float up_y;
    float up_z;
    float angle;
    
} modelview_data;



//bunny as global so that it can be accessed from any function
ply_object bunny;
projection_data proj;
modelview_data mod;


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


//todo
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
        
        //allocate memory for the amount of vertex indexes that the face includes
        bunny.faces_indices[i] = (int *) malloc(amount_of_face_indices * sizeof(int *));
        
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

//ToDo, enable Z-buffering at some point
int main (int argc, char **argv)
{
    #ifdef DEBUG
    printf("starting main function\n");
    #endif


	//initialize glut, create a window
    glutInit (&argc, argv);
     
    glutInitDisplayMode (GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize (600, 600);
    glutCreateWindow ("OpenGL Test");
	
    //glewinit error reporting straight from glew example
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));  
        
    printf ("Supported OpenGL version: %s\n",
	glGetString (GL_VERSION));
	glutKeyboardFunc(keyb_cb);
	glutMotionFunc(mouse_motion_cb);
	glutSpecialFunc(sp_keyb_cb);
    glutTimerFunc(1000, timer_cb, 0);
    glutReshapeFunc(window_resize_cb);

    //initialize modelview and projection matrices
    mod.camera_x = 0.0;
    mod.camera_y = 0.0;
    mod.camera_z = 0.0;
    mod.lookat_x = 0.0;
    
    mod.lookat_y = 0.0;
    mod.lookat_z = -1.0;
    mod.up_x = 0.0;
    mod.up_y = 1.0;
    mod.up_z = 0.0;

    mod.angle = 0.0;

    proj.fovy = 45;
    proj.aspect = 1.0;
    proj.near = 1;
    proj.far = 1000;
   
    update_projection();
    update_modelview();
    
    // load shaders
    
    #ifdef DEBUG
    printf("starting to load shaders\n");
    #endif
    GLuint f, v;
    p = glCreateProgram ();
    
    GLchar *vs, *fs;
    v = glCreateShader (GL_VERTEX_SHADER);
    f = glCreateShader (GL_FRAGMENT_SHADER);
    GLint vlen = 0;
    GLint flen = 0;
    
    
    vs = simple_fileread("simple_shader.vert", &vlen);

    #ifdef DEBUG
    printf("loaded vertex shader with %i chars\n", vlen);
    #endif
    
    fs = simple_fileread("simple_shader.frag", &flen);
    
    #ifdef DEBUG
    printf("loaded fragment shader with %i characters\n",flen);
    #endif
    

    const GLchar * vv = vs;
    const GLchar * ff = fs;
    const GLint flenc = flen;
    const GLint vlenc = vlen;
    glShaderSource (v, 1, &vv, &vlenc);
    glShaderSource (f, 1, &ff, &flenc);

    
    //compile shaders

    
    #ifdef DEBUG
    printf("shaders loaded. compiling next\n");
    #endif
    int compiled;
    int infoLogLen = 1000, charsWritten = 0;
    char *infoLog = 0;
    glCompileShader (v);
    glGetShaderiv (v, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        //glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            printf("Errors compling vertex shader:\n");
            infoLog = (char *)malloc (infoLogLen);
            glGetShaderInfoLog (v, infoLogLen, &charsWritten,
            infoLog);
            printf ("%s\n", infoLog);
            free (infoLog);
        }
    }
     
    compiled = 0;
    glCompileShader(f);
    glGetShaderiv (f, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        //glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            printf("Errors compling fragment shader:\n");
            infoLog = (char *)malloc (infoLogLen);
            glGetShaderInfoLog (f, infoLogLen, &charsWritten,
            infoLog);
            printf ("%s\n", infoLog);
            free (infoLog);
        }
    }


// run shaders

    #ifdef DEBUG
    printf("compiled. running shaders.\n");
    #endif
    
    glAttachShader (p, v);
    glAttachShader (p, f);
    glLinkProgram (p);
    glUseProgram (p);

    
    #ifdef DEBUG
    printf("beginning read from file.\n");
    #endif
   
    char * filename = "bunny/reconstruction/bun_zipper.ply"; 
    read_ply_from_file(filename);
        
    //read file here
    //todo add parameter for normals data
   
    
    //todo add index data as glBufferData of type 
    //GL_ELEMENT_ARRAY_BUFFER
    
    
    //and normal data the same way as vertex data
    printf("read file %s that had \n%i vertices and \n%i faces.\n",
    filename, bunny.amount_of_vertices, bunny.amount_of_faces);
    
    
    //lecture example
    //using objects instead of nice old GL_vertex-calls :)
    unsigned int vertex_array_object_ID;
    unsigned int number_of_arrays = 1;
    glGenVertexArrays (number_of_arrays, &vertex_array_object_ID);
    glBindVertexArray (vertex_array_object_ID);
    vertex_ID = vertex_array_object_ID;

    
    unsigned int vertex_buffer_object_ID[2];
    unsigned int number_of_buffers = 2;
    glGenBuffers (number_of_buffers, vertex_buffer_object_ID);
            
    unsigned int elements_per_vertex = 3;
//    unsigned int elements_per_triangle = 3 * elements_per_vertex;
    //this monster actually binds data
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_object_ID[0]);
    glBufferData (GL_ARRAY_BUFFER, 
        elements_per_vertex* bunny.amount_of_vertices * sizeof (float), 
        bunny.vertices,
        GL_STATIC_DRAW);
    
    
    //count the bunny triangle indices
    int i;
    int j;
    int indices_amount = 0;
    for(i = 0; i < bunny.amount_of_faces; i++) {
        indices_amount = indices_amount + bunny.faces[i];
    }
    printf("indices amount %i \n", indices_amount);
    bunny_indices = (int *) malloc(indices_amount * sizeof(int*));
    
    for(i=0; i < bunny.amount_of_faces; i++) {
        int lim = bunny.faces[i];
        if(lim == 3) {
            for(j=0; j < lim; j++) {
                bunny_indices[i * 3 + j] = bunny.faces_indices[i][j]; 
            }
        }
    }
    
    //adding the indices to buffer
    
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, 
        sizeof (bunny_indices),
        bunny_indices,
        GL_STATIC_READ);
    

    int vertex_position_location = 0;
    glBindAttribLocation(p, vertex_position_location, "vertex_Position");

    glVertexAttribPointer (vertex_position_location, elements_per_vertex,
        GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray (vertex_position_location);


    #ifdef DEBUG
    printf("binding our only vertex array, attaching display function and\n");
    #endif

	glutDisplayFunc(display_cb);
	glutMainLoop ();
	return 0;
}




void display_cb(void)
{
    
    #ifdef DEBUG
    printf("display callback called\n");
    #endif

    glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
    
    unsigned int location_projection_matrix = glGetUniformLocation(p,
    "projection_matrix");
    unsigned int location_modelview_matrix = glGetUniformLocation(p,
    "modelview_matrix");
    
    GLfloat mp[16];
    GLfloat mv[16];
    glGetFloatv (GL_PROJECTION_MATRIX, mp);
    glGetFloatv (GL_MODELVIEW_MATRIX, mv);
    glUniformMatrix4fv (location_projection_matrix, 1, GL_FALSE, mp);
    glUniformMatrix4fv (location_modelview_matrix, 1, GL_FALSE, mv);
    
    
    glBindVertexArray (vertex_ID);

	//here goes actual drawing code
	glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

	//glutSolidTeapot(0.5);
//	glScalef(5,5,5);

   // unsigned int offset = 0;
    //unsigned int count = elements_per_triangle * number_of_triangles;
    //ToDo switch to glDrawElements for smarter drawing
    //glDrawElements requires more stuff
    
    glDrawElements(GL_TRIANGLES, bunny.amount_of_faces, GL_UNSIGNED_INT, bunny_indices);

	glFrontFace(GL_CCW);


	glFlush();
	glutSwapBuffers();

}

void mouse_motion_cb(int x, int y)
{
	printf("mouse moved x: %i, y: %i\n", x, y);
    
}

void keyb_cb(unsigned char key, int x, int y)
{
	printf("keyboard key pushed %c\n", key );
}

void move_camera(int direction){
    mod.camera_x = mod.camera_x + direction*mod.lookat_x*MOVE_STEP;    
    mod.camera_z = mod.camera_z +  direction*mod.lookat_z*MOVE_STEP;    
    update_modelview();
}


void rotate_camera(float angle){
    mod.lookat_x = sin(angle) ;    
    mod.lookat_z = -1.0*cos(angle) ;   
    update_modelview();
}



void sp_keyb_cb(int key, int x, int y)
{
    switch(key)
    {
    case(GLUT_KEY_UP):
        printf("forward key pressed\n");
        move_camera(1);
        break;
    case (GLUT_KEY_DOWN):
        printf("backward key pressed\n"); 
        move_camera(-1);
        break;
    case (GLUT_KEY_LEFT):
        printf("left key pressed\n"); 
        mod.angle += -0.1 ;
        rotate_camera(mod.angle);
        break;
    case (GLUT_KEY_RIGHT):
        printf("right key pressed\n"); 
        mod.angle += 0.1 ;
        rotate_camera(mod.angle);
        break; 
    }

}



void timer_cb(int value){
//    display_cb();
    }

void window_resize_cb(int width, int height){
    //int old_height, old_width;
    //old_height = 2*proj.near*sin(proj.fovy);
    //old_width = old_height*proj.aspect;
    printf("resize window cb called\n");
    float ratio = width / height;
    proj.aspect = ratio;

    proj.fovy = asin((height/2)/proj.near);
    update_projection();
    glViewport(0,0,width, height);
    }


void update_modelview(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(mod.camera_x, mod.camera_y, mod.camera_z,
    (mod.camera_x + mod.lookat_x), (mod.camera_y+mod.lookat_y), (mod.camera_z+
    mod.lookat_z),
    mod.up_x, mod.up_y, mod.up_z);
    display_cb(); //remove later
}

void update_projection(){
    //glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(proj.fovy, proj.aspect, proj.near, proj.far);
    display_cb();
    }


    
    
