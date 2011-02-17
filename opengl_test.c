#include <GL/glew.h>
#include <GL/glut.h>
//#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h> 
#include <math.h>
#include "./lib.h"

#define DEBUG

//Triangle indices for the bunny
int * bunny_indices;
int vertex_ID;
int * vertex_count;
GLuint p; //program


//bunny as global so that it can be accessed from any function
ply_object bunny;
projection_data proj;
modelview_data mod;

//sets, loads and whatnot shaders
void set_shaders(){
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
    printf("compiled.any errors are above. running shaders.\n");
    #endif
    
    glAttachShader (p, v);
    glAttachShader (p, f);
    glLinkProgram (p);
    glUseProgram (p);

    
    
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
    proj.height = 600;
    proj.width = 600;

    proj.fovy = 45;
    proj.aspect = 1.0;
    proj.near = 1;
    proj.far = 1000;
   
    update_projection();
    update_modelview();
    
    // load shaders
    set_shaders(); 
   
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

    
    unsigned int number_of_buffers = 3;
    unsigned int vertex_buffer_object_ID[number_of_buffers];
    glGenBuffers (number_of_buffers, vertex_buffer_object_ID);
            
    unsigned int elements_per_vertex = 3;

    unsigned int elements_per_triangle = 3 * elements_per_vertex;
    //this monster actually binds data
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_object_ID[0]);
    glBufferData (GL_ARRAY_BUFFER, 
        elements_per_vertex* bunny.amount_of_vertices * sizeof (float), 
        bunny.vertices, GL_STATIC_DRAW);
 
    int vertex_position_location = 0;
    glBindAttribLocation(p, vertex_position_location, "vertex_position");

    glVertexAttribPointer (vertex_position_location, elements_per_vertex,
        GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray (vertex_position_location);


    //adding the indices to buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_ID[1]); 
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, 
        bunny.amount_of_faces*3*sizeof(int),
        bunny.faces_indices,
        GL_STATIC_READ);
    
    
    //normal buffer
    unsigned int location_normal = glGetUniformLocation(p, "vertex_normal");

    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_object_ID[2]);
    glBufferData(GL_ARRAY_BUFFER, 
        elements_per_triangle*bunny.amount_of_faces,
        bunny.normals,
        GL_STATIC_DRAW);
    glVertexAttribPointer(location_normal, elements_per_vertex, GL_FLOAT,
        GL_FALSE, 0,0);
    glEnableVertexAttribArray(location_normal);


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
    
    #ifdef DEBUG
/*    printf("modelview_matrix\n");
    for (int i = 0; i < 16; i++)
    {
        printf("%f, ", mv[i]);
        if((i+1)%4 == 0)
            printf("\n");
    }
    printf("projection_matrix\n");
    for (int i = 0; i < 16; i++)
    {
        printf("%f, ", mp[i]);
        if((i+1)%4 == 0)
            printf("\n");
    } 
*/ 
   #endif
	//here goes actual drawing code
    //flip this CCW/CW to see other side of bunny for now
	glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_SMOOTH);

    glBindVertexArray (vertex_ID);

	//glutSolidTeapot(0.5);
	glScalef(2,2,2);

   // unsigned int offset = 0;
    //unsigned int count = elements_per_triangle * number_of_triangles;
    //ToDo switch to glDrawElements for smarter drawing
    //glDrawElements requires more stuff
     
    glDrawElements(GL_TRIANGLES, bunny.amount_of_faces, GL_UNSIGNED_INT, bunny_indices);

    glFrontFace(GL_CCW);


    //glDrawArrays(GL_TRIANGLES,0,3*bunny.amount_of_vertices);

	glFlush();
	glutSwapBuffers();

}
   
  
