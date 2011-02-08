#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void display_cb(void);
void keyb_cb(unsigned char key, int x, int y);
void mouse_motion_cb(int x, int y);


void draw_vertices_from_file(const char *file_name)
{
	//BEGIN PLY FILE HANDLING
    FILE *ply;
    
    char line[100]; //line to be read
    int amount_of_vertices;
    
    ply = fopen (file_name, "rt"); //open ply file

    //read header
    while (fgets(line, 100, ply) != NULL)
    {
        /* convert the string to a long int */
        if (!strncmp (line, "element vertex", 14)) {
            sscanf(line, "element vertex %i", &amount_of_vertices);
            
        } else if (!strncmp(line, "end_header", 10)) {
            break;
        }
    }
    
    //read vertices
    int i;
    float x;
    float y;
    float z;

    //glBegin(GL_TRIANGLES);
    for(i = 0; i < amount_of_vertices; i++) {
        while(fgets(line, 100, ply) != NULL)
        {
            sscanf(line, "%f %f %f", &x, &y, &z);
            glVertex3f(x, y, z);
        }
    }
    //glEnd();
    
    fclose(ply); //close file
    //END PLY FILE HANDLING
}


int main (int argc, char **argv)
{
	//initialize glut, create a window
    glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("OpenGL Test");
    
    // load shaders
    /*
    GLuint p, f, v;
    char *vs, *fs;
    v = glCreateShader (GL_VERTEX_SHADER);
    f = glCreateShader (GL_FRAGMENT_SHADER);
    GLint vlen;
    GLint flen;
    vs = loadFile ("example_shader.vert", vlen);
    fs = loadFile ("example__shader.frag", flen);
    const char *vv = vs;
    const char *ff = fs;
    glShaderSource (v, 1, &vv, &vlen);
    glShaderSource (f, 1, &ff, &flen);

    */
    //compile shaders

    /*
    bool compiled;
    glCompileShader (v);
    glGetShaderiv (v, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        int infoLogLen = 0, charsWritten = 0;
        char *infoLog = 0;
        //glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            infoLog = (char *)malloc (infoLogLen);
            glGetShaderInfoLog (v, infoLogLen, &charsWritten,
            infoLog);
            printf ("%s\n", infoLog);
            free (infoLog);
        }
    }
    */
    // run shaders

    /*
    p = glCreateProgram ();
    glAttachShader (p, v);
    glAttachShader (p, f);
    glLinkProgram (p);
    glUseProgram (p);

    unsigned int location_projection_matrix = glGetUniformLocation(p,
    "projectionMatrix");
    unsigned int location_modelview_matrix = gelGetUniformLocation(p,
    "modelviewMatrix");
    
    GLfloat mp[16];
    GLfloat mv[16];
    glGetFloatv (GL_PROJECTION_MATRIX, mp);
    glGetFloatv (GL_MODELVIEW_MATRIX, mv);
    glUniformMatrix4fv (location_projection_matrix, 1, GL_FALSE, mp);
    glUniformMatrix4fv (location_modelview_matrix, 1, GL_FALSE, mv);

   */

    printf ("Supported OpenGL version: %s\n",
	glGetString (GL_VERSION));
	glutKeyboardFunc(keyb_cb);
	glutMotionFunc(mouse_motion_cb);

	
	glutDisplayFunc(display_cb);
	glutMainLoop ();
	return 0;
}




void display_cb(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	


	//here goes actual drawing code
	glFrontFace(GL_CW);
	
	//glutSolidTeapot(0.5);
	glScalef(5,5,5);
    glBegin(GL_POINTS);
	draw_vertices_from_file("bunny/reconstruction/bun_zipper.ply");
	//draw_vertices_from_file("bunny/reconstruction/bun_zipper_res2.ply");
	//draw_vertices_from_file("bunny/reconstruction/bun_zipper_res3.ply");
	//draw_vertices_from_file("bunny/reconstruction/bun_zipper_res4.ply");
	//draw_vertices_from_file("bunny/data/bun000.ply");
	//draw_vertices_from_file("bunny/data/bun045.ply");
	//draw_vertices_from_file("bunny/data/bun090.ply");
	//draw_vertices_from_file("bunny/data/bun180.ply");
	//draw_vertices_from_file("bunny/data/bun270.ply");
	//draw_vertices_from_file("bunny/data/bun315.ply");
    glEnd();
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


