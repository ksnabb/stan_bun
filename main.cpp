#include <utils.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;

GLuint vertex_ID;
GLuint texture_ID[2];
int * vertex_count;
GLuint p; //program
GLuint time_anim = 0; //for animation
GLuint toon = 0; //for toon shading
GLuint twotex = 0; //for toon shading
MeshObject mesh_object = MeshObject("bunny/reconstruction/bun_zipper.ply");;


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
   /*
    glUniform4fv(glGetUniformLocation(p,"light0.diffuse"), 1, lights[0].diffuse);
    glUniform4fv(glGetUniformLocation(p,"light0.position"), 1, lights[0].position);
    glUniform4fv(glGetUniformLocation(p,"light0.specular"),1, lights[0].specular);
    glUniform4fv(glGetUniformLocation(p,"light0.ambient"),1, lights[0].ambient);
    glUniform1i(glGetUniformLocation(p,"light0.type"),
    lights[0].type);

    glUniform4fv(glGetUniformLocation(p, "material.ambient"), 1, material.ambient);
    glUniform4fv(glGetUniformLocation(p, "material.diffuse"), 1, material.diffuse);
    glUniform4fv(glGetUniformLocation(p, "material.specular"), 1, material.specular);
    glUniform4fv(glGetUniformLocation(p, "material.emission"), 1, material.emission);
    glUniform1f(glGetUniformLocation(p, "material.shininess"), material.shininess);
*/

   //time step for animation
//   if (time > 0)
//       time++;
    glUniform1i(glGetUniformLocation(p, "time"),time_anim);
    glUniform1i(glGetUniformLocation(p, "toon"),toon);
    glUniform1i(glGetUniformLocation(p, "twotex"),twotex);

//    printf("uniform locations %i, %i, %i, %i, %i\n", location_base_color,
//
//    location_projection_matrix, location_light_pos, location_light_col,
//    location_material_diffuse);

/*    GLfloat temp[4];
    glGetUniformfv(p, location_light_pos,temp);
    printf("color %f, %f, %f\n", temp[0], temp[1], temp[2]);
*/
    //create samplers
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_ID[0]);
    int texture_sampler = glGetUniformLocation(p, "texture_1");
    glUniform1i(texture_sampler, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_ID[1]);
    int texture_sampler2 = glGetUniformLocation(p, "texture_2");
    glUniform1i(texture_sampler2, 1);


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
        3*mesh_object.amount_of_faces,
        GL_UNSIGNED_INT,
        mesh_object.faces_indices);

	glFlush();
	glutSwapBuffers();

}


int main (int argc, char **argv) {
    cout << "starting main function\n";
    
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
        elements_per_vertex * mesh_object.amount_of_vertices * sizeof (float), 
        mesh_object.vertices, 
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
                mesh_object.amount_of_vertices * 3 * sizeof(float),
                mesh_object.vertex_normals,
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
                mesh_object.amount_of_vertices * 2 * sizeof(float),
                mesh_object.tex_coordinates,
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
    
    //load_textures(argv[2], argv[3]);

    //initialize lights and materials here 
    /*
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
    */    
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


