#include <utils.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;

int main (int argc, char **argv) {
    cout << "starting main function\n";
    
    MeshObject mesh_object = MeshObject("bunny/reconstruction/bun_zipper.ply");
    
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
	
	
	return 0;
}
