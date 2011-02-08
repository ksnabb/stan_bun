#include <GL/glew.h>
#include <GL/glut.h>
//#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DEBUG

void display_cb(void);
void keyb_cb(unsigned char key, int x, int y);
void mouse_motion_cb(int x, int y);
char** load_shader_file(char * file_name, GLint ** lengths, GLint * length);

int vertex_ID;
int * vertex_count;
int LINE_LENGTH = 100;


//TOO COMPLEX, not used todo: remove 
//loads shader file, returns pointer to text data
GLchar** load_shader_file(char * file_name, GLint ** lengths, GLint * length)
{    
    #ifdef DEBUG
    printf("loading shader from file %s\n", file_name);
    #endif

    GLint lines = 0;
    FILE *src;
    src = fopen (file_name, "rt"); //open ply file 
    if (src == NULL){
        fprintf(stderr, "Fatal error in opening file %s", file_name);
        return NULL;
        }
    int currentsize = 128;
    GLchar ** shader = malloc(currentsize*sizeof(char*));
 
   int * line_lengths = malloc(currentsize*sizeof(int));
    char line[LINE_LENGTH];
    printf("starting to read\n");
    while(fgets(line,LINE_LENGTH, src) != NULL)
    {
        if (lines >= currentsize)
        {
            shader = realloc(shader, 2*currentsize*sizeof(char*));
            line_lengths = realloc(line_lengths, 2*currentsize*sizeof(int));
            currentsize = 2*currentsize;
        }
        line_lengths[lines] = strlen(line);
        printf("line %i is %i chars long\n", lines, line_lengths[lines]);
        shader[lines] = (char *) malloc(strlen(line)*sizeof(char));
        strcpy(shader[lines], line);
        lines++;
    }

    fclose(src); //close file
    *length = lines;
    *lengths = line_lengths;
    int h;
    for (h =0; h<lines; h++){
        printf("%i\n", line_lengths[h]);
        }
    
    return shader;
}

//reads a file to a char array
GLchar * simple_fileread(char * file_name, GLint * length)
{
    #ifdef DEBUG
    printf("loading shader from file %s\n", file_name);
    #endif

    GLint chars = 0;
    FILE *src;
    src = fopen (file_name, "rt"); //open ply file 
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

float * read_ply_from_file(const char *file_name, float * vertices, int *
number_vertices, int * fragments, int * number_fragments)
{
	//BEGIN PLY FILE HANDLING
    FILE *ply;
    
    char line[LINE_LENGTH]; //line to be read
    int amount_of_vertices = 0;
    int amount_of_fragments = 0; 
    ply = fopen (file_name, "rt"); //open ply file

    //read header
    while (fgets(line, LINE_LENGTH, ply) != NULL)
    {
        /* convert the string to a long int */
        if (!strncmp (line, "element vertex", 14)) {
            sscanf(line, "element vertex %i", &amount_of_vertices);
            
        } else if (!strncmp (line, "element face", 12)) {
            sscanf(line, "element face %i", &amount_of_fragments);
            
        } 
        
        else if (!strncmp(line, "end_header", 10)) {
            break;
        }
    }

      
    #ifdef DEBUG
    printf("file format says %i vertices, %i faces\n", amount_of_vertices,
    amount_of_fragments);
    #endif
    //read vertices
    int i;
    float x;
    float y;
    float z;

    //create vertex table for pushing into OpenGL
    //float * vertices;
    //observe that table is 1-dimensional
    vertices = malloc(3*amount_of_vertices*sizeof(float*));

    for(i = 0; i < amount_of_vertices; i++) {
        if(fgets(line, LINE_LENGTH, ply) != NULL)
        {
            sscanf(line, "%f %f %f", &x, &y, &z);
            vertices[3*i] = x;
            vertices[(3*i)+1] = y;
            vertices[(3*i)+2] = z;
        }
    }    
    
    int * frags;
    frags = (int *) malloc(3*amount_of_fragments*sizeof(int*));
    int i0, i1, i2, i3;
    for(i = 0; i < amount_of_fragments; i++) {
        if(fgets(line, LINE_LENGTH, ply) != NULL)
        {
            sscanf(line, "%i %i %i %i", &i0, &i1, &i2, &i3);
            frags[3*i] = i1;
            frags[(3*i)+1] = i2;
            frags[(3*i)+2] = i3;
        }
    }

     

    //got all the vertices (let's hope there's no off by 1 error up there)
    //create fragment table, stupid format gives no assurance of only
    //triangles
    //frags has first number of elements (after [0]) and then elements
    
    //int ** frags;
    //frags = (int**) malloc(amount_of_vertices*sizeof(int*));
/* TODO: simplify to expect number 3 and 3 ints, do nothing if first number
 * isn't 3
 
    int a, b;
    char * linepart = malloc(LINE_LENGTH*sizeof(char));

   for(int i = 0; i < amount_of_vertices; i++){
        if(fgets(line, LINE_LENGTH, ply) != NULL)
        {
            //simplify here
            sscanf(line, "%i %s", &a, linepart); 
            frags[i] = (int*) malloc((a+1)*sizeof(int));
            frags[i][0] = a;
            for (int j = 0; i < a; j++){
            char * anotherpart = malloc(LINE_LENGTH*sizeof(char));
                    sscanf(linepart, "%i %s", &b, anotherpart);
                    free(linepart);
                    linepart = anotherpart;
                    frags[i][j+1] = b;
                    free(anotherpart);
                    //this is ugly and i'm ashamed of myself
                    //but it should work for normal cases
                }
                free(linepart);
        }
        
        
    }
 */
    //great, now we've read the triangle data

    fclose(ply); //close file
    //END PLY FILE HANDLING
    *number_vertices = amount_of_vertices;
    *number_fragments = amount_of_fragments;
    vertex_count = &amount_of_vertices;
    
    
    fragments = frags;
    //what were we supposed to return again?
    
    return vertices;
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
   

    
   
    // load shaders
    
    #ifdef DEBUG
    printf("starting to load shaders\n");
    #endif
    GLuint p, f, v;
    p = glCreateProgram ();
    
    GLchar *vs, *fs;
    v = glCreateShader (GL_VERTEX_SHADER);
    f = glCreateShader (GL_FRAGMENT_SHADER);
    GLint * vlen = malloc(sizeof(int));
    GLint * flen = malloc(sizeof(int));
    
    
    vs = simple_fileread("simple_shader.vert", vlen);

    #ifdef DEBUG
    printf("loaded vertex shader with %i chars\n", *vlen);
    #endif
    
    fs = simple_fileread("simple_shader.frag", flen);
    
    #ifdef DEBUG
    printf("loaded fragment shader with %i characters\n",*flen);
    #endif
    

    const GLchar * vv = vs;
    const GLchar * ff = fs;
    const GLint flenc = *flen;
    const GLint vlenc = *vlen;
    glShaderSource (v, 1, &vv, &vlenc);
    glShaderSource (f, 1, &ff, &flenc);

    
    //compile shaders

    
    #ifdef DEBUG
    printf("shaders loaded. compiling next\n");
    #endif
    int compiled;
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
     
    glCompileShader(f);
    glGetShaderiv (f, GL_COMPILE_STATUS, &compiled);
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
   
    float * vertices;
    int * fragments;
    int nvertices = 0;
    int nfragments = 0;
    char * filename = "bunny/reconstruction/bun_zipper.ply";
    //read file here
    //todo add parameter for normals data
    
    vertices = read_ply_from_file(filename,
        vertices, &nvertices, 
        fragments, &nfragments);
        
    //todo add index data as glBufferData of type 
    //GL_ELEMENT_ARRAY_BUFFER
    
    
    //and normal data the same way as vertex data
    printf("read file %s that had \n%i vertices and \n%i fragments.\n",
    filename, nvertices, nfragments);
    //lecture example
    //using objects instead of nice old GL_vertex-calls :(
    unsigned int vertex_array_object_ID;
    unsigned int number_of_arrays = 1;
    glGenVertexArrays (number_of_arrays, &vertex_array_object_ID);
    glBindVertexArray (vertex_array_object_ID);
    vertex_ID = vertex_array_object_ID;

    unsigned int vertex_buffer_object_ID[2];
    unsigned int number_of_buffers = 2;
    glGenBuffers (number_of_buffers, vertex_buffer_object_ID);
    
    
    
    unsigned int elements_per_vertex = 3;
    unsigned int elements_per_triangle = 3 * elements_per_vertex;
    //this monster actually binds data
    glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_object_ID[0]);
    glBufferData (GL_ARRAY_BUFFER, 
    elements_per_vertex* nvertices * sizeof (float), vertices,
    GL_STATIC_DRAW);
    
    //adding the indices to buffer
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, 
    elements_per_triangle* nfragments * sizeof (float), fragments,
    GL_STATIC_READ);
    
    printf("boo\n");

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
    
    /* not sure if these need to be passed
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


    glBindVertexArray (vertex_ID);

	//here goes actual drawing code
	glFrontFace(GL_CW);
	
	//glutSolidTeapot(0.5);
	glScalef(5,5,5);

    unsigned int offset = 0;
    //unsigned int count = elements_per_triangle * number_of_triangles;
    //ToDo switch to glDrawElements for smarter drawing
    //glDrawElements requires more stuff
    //does this have to be in the callback function?
    int * indices;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER,
                            GL_BUFFER_ACCESS,
                            indices);
    glDrawElements(GL_TRIANGLES, 65000, GL_UNSIGNED_BYTE, indices);
    //glDrawArrays (GL_TRIANGLES, offset, *vertex_count);

    
    //glBegin(GL_POINTS);
	//glEnd();
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


