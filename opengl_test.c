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
GLuint vertex_ID;
GLuint texture_ID;
int * vertex_count;
GLuint p; //program
GLuint time = 0; //for animation

//bunny as global so that it can be accessed from any function
ply_object bunny;
projection_data proj;
modelview_data mod;
light_params lights[2];
material_params material;
GLuint num_lights = 1;

//sets, loads and whatnot shaders
void set_shaders(){
     
    #ifdef DEBUG
    printf("starting to load shaders\n");
    #endif
    
    GLuint f, v;
   
    //create the shader program
    p = glCreateProgram ();
    
    GLchar *vs, *fs;
    
    //create shaders
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
    
    //attach shaders
    glAttachShader (p, v);
    glAttachShader (p, f);
    glLinkProgram (p);
    glUseProgram (p);

    
    
}
//loads our only texture
void load_texture(char * filename){
    GLuint n_textures = 1;
//    texture_id is global
    glGenTextures(n_textures, &texture_ID);

    glBindTexture(GL_TEXTURE_2D, texture_ID);

    img_data * tex = simple_bmp_read(filename);
    img_data mytex = *tex;
    #ifdef DEBUG
    printf("attaching an %i by %i texture image from %p\n", mytex.width,
    mytex.height, mytex.whole+mytex.rgbstart);
    #endif

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, mytex.width, mytex.height,
    GL_RGB, GL_UNSIGNED_BYTE, mytex.whole+mytex.rgbstart);
//    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, mytex.width, mytex.height,0,
//    GL_RGBA, GL_UNSIGNED_BYTE, mytex.rgbstart);

    glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
    free(mytex.whole);
    free(tex);
    }

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
	
	//connecting the UI interaction function
	glutKeyboardFunc(keyb_cb);
	glutMotionFunc(mouse_motion_cb);
	glutMouseFunc(mouse_cb);
	glutSpecialFunc(sp_keyb_cb);
    glutTimerFunc(100, timer_cb, 0);
    glutReshapeFunc(window_resize_cb);

    //initialize modelview and projection matrices
    mod.camera_x = 0.0;
    mod.camera_y = 0.0;
    mod.camera_z = -1.0;
    mod.lookat_x = 0.0;
    
    mod.lookat_y = 0.0;
    mod.lookat_z = 0.0;
    mod.up_x = 0.0;
    mod.up_y = 1.0;
    mod.up_z = 0.0;

    mod.angle = 0.0;
    proj.height = 600;
    proj.width = 600;

    proj.fovy = 45;
    proj.aspect = 1.0;
    proj.near = 1;
    proj.far = 2000;
    
   
    update_projection();
    update_modelview();
    
    // load shaders
    set_shaders(); 
   
    #ifdef DEBUG
    printf("beginning read from file.\n");
    #endif
   
    //read bunny from file
    char * filename = "bunny/reconstruction/bun_zipper.ply"; 
    read_ply_from_file(filename);
      
          
    //setting up the vertex array and then buffers
    GLuint vertex_array_object_ID;
    unsigned int number_of_arrays = 1;
    glGenVertexArrays (number_of_arrays, &vertex_array_object_ID);
    glBindVertexArray (vertex_array_object_ID);
    
    vertex_ID = vertex_array_object_ID;

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
        elements_per_vertex * bunny.amount_of_vertices * sizeof (float), 
        bunny.vertices, 
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
                bunny.amount_of_vertices * 3 * sizeof(float),
                bunny.vertex_normals,
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
 
    //texture coordinate buffer
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_object_ID[2]);
    glBufferData(GL_ARRAY_BUFFER,
                bunny.amount_of_vertices * 2 * sizeof(float),
                bunny.tex_coordinates,
                GL_STATIC_DRAW);
                
    unsigned int location_vertex_tex = 2;
    glBindAttribLocation(p, location_vertex_tex, "vertex_texcoord");
    glVertexAttribPointer(location_vertex_tex, 
                    2, 
                    GL_FLOAT,
                    GL_FALSE, 
                    0,
                    0);
    glEnableVertexAttribArray(location_vertex_tex);


    #ifdef DEBUG
    printf("reading texture\n");
    #endif
    
    load_texture("./texture.bmp");

    //initialize lights and materials here 
    material.shininess = 40.0;
    GLfloat ambient[4] = {0.05,0.05,0.05,1.0};
    material.ambient = (GLfloat*)&ambient;
    GLfloat diffuse[4] = {0.5,0.5,0.5,1.0};
    material.diffuse = (GLfloat*)&diffuse;
    GLfloat specular[4] = {0.5,0.5,0.5,1.0};
    material.specular = (GLfloat*)&specular;

    //light 0 is a directed light up high
    GLfloat light0pos[4] = {0.0, 50.0, 5.0, 0.0};
    lights[0].position = (GLfloat*) &light0pos;
    GLfloat light0diffuse[4] = {0.3,0.3,0.3,1.0};
    lights[0].diffuse = (GLfloat*) &light0diffuse;
    GLfloat light0specular[4] = {0.3,0.3,0.3,1.0};
    lights[0].specular = light0specular;
    GLfloat light0ambient[4] = {0.1,0.1,0.1,1.0};
    lights[0].ambient = light0ambient;//do we need this?
    lights[0].type = 0; //directed light
    
    //light 1 is either a point light or a spotlight depending on config
   /* 
    lights[1].position = {0.0,1.0,0.0,0.0}; //in eye coords, just above eye
    lights[1].spot_exponent = 4.0;
    lights[1].spot_cosine_cutoff = 0.2; //how large spot angle
    lights[1].spot_dir = {0.0,0.0,-1.0,0.0}; //straight forward from eye
    lights[1].spot_ambient = {0.0,0.0,0.0,0.0};  
    lights[1].spot_diffuse = {1.0,0.0,0.0,0.0}; //red
    lights[1].type = 1;
*/

    #ifdef DEBUG
    printf(" attaching display function and running main loop\n");
    #endif


	glutDisplayFunc(display_cb);
	glutMainLoop ();
	return 0;
}




void display_cb(void)
{
    
    #ifdef DEBUG
//    printf("display callback called\n");
    #endif

    glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

    glUseProgram(p);

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
   
    GLfloat base_color[4] = {0.05, 0.05, 0.05, 1.0}; //light grey by default
    


    unsigned int location_base_color = glGetUniformLocation(p, "base_color");


    //glUniform1iv(location_lights,2, lights);
    //glUniform4fv(location_material, 1, material);
    glUniform4fv(location_base_color, 1, base_color);
   //crap, can't send a struct as a struct, need to send piece by piece
    glUniform4fv(glGetUniformLocation(p,"light0.diffuse"),1,
    lights[0].diffuse);
    glUniform4fv(glGetUniformLocation(p,"light0.position"),1,
    lights[0].position);
    glUniform4fv(glGetUniformLocation(p,"light0.specular"),1,
    lights[0].specular);
    glUniform4fv(glGetUniformLocation(p,"light0.ambient"),1,
    lights[0].ambient);
    glUniform1i(glGetUniformLocation(p,"light0.type"),
    lights[0].type);

    glUniform4fv(glGetUniformLocation(p,"material.ambient"),1,
    material.ambient);
    glUniform4fv(glGetUniformLocation(p,"material.diffuse"),1,
    material.diffuse);
    glUniform4fv(glGetUniformLocation(p,"material.specular"),1,
    material.specular);
     glUniform4fv(glGetUniformLocation(p,"material.emission"),1,
    material.emission);
    glUniform1f(glGetUniformLocation(p,"material.shininess"),
     material.shininess);


   //time step for animation
//   if (time > 0)
//       time++;
    glUniform1i(glGetUniformLocation(p, "time"),time);

//    printf("uniform locations %i, %i, %i, %i, %i\n", location_base_color,
//
//    location_projection_matrix, location_light_pos, location_light_col,
//    location_material_diffuse);

/*    GLfloat temp[4];
    glGetUniformfv(p, location_light_pos,temp);
    printf("color %f, %f, %f\n", temp[0], temp[1], temp[2]);
*/
    //create sampler
    int texture_sampler = glGetUniformLocation(p, "my_texture");
    glUniform1i(texture_sampler, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_ID);

	//here goes actual drawing code
	glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    //vertex array
    glBindVertexArray (vertex_ID);
    
    //draw the bunny according to indices and the vertices in the server buffer
    glDrawElements(GL_TRIANGLES,
        3*bunny.amount_of_faces,
        GL_UNSIGNED_INT,
        bunny.faces_indices);

	glFlush();
	glutSwapBuffers();

}
   
  
