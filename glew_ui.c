/*
* Functions that use GLEW to create the ui and manipulate the view
*/

//today I'm lazy with the includes, some may not be necessary
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

 
extern projection_data proj;
extern modelview_data mod;
extern GLuint time;
extern GLuint toon;

float MOVE_STEP = 0.1;



void mouse_cb(int button, int state, int x, int y)
{
    
//    printf("mouse wheel moved %i %i %i %i\n", button, state, x, y);
    switch(button)
    {
    case(3):
        glMatrixMode(GL_MODELVIEW);
        glScalef(1.1,1.1,1.1);
        display_cb();
        break;
    case(4):
        glMatrixMode(GL_MODELVIEW);
        glScalef(0.9,0.9,0.9);
        display_cb();
        break;
    }
}

int prev_x = -1;
int prev_y = -1;

void mouse_motion_cb(int x, int y)
{
//    printf("mouse moved x: %i, y: %i\n", x, y);
    
    if(prev_x == -1 && prev_y == -1) {
        prev_x = x;
        prev_y = y;
    } else {
        if((prev_x - x) < 0) {
            glMatrixMode(GL_MODELVIEW);
            glRotatef(2,0,1,0);
            display_cb();
        } else {
            glMatrixMode(GL_MODELVIEW);
            glRotatef(-2,0,1,0);
            display_cb();
        }
        
        if((prev_y - y) < 0) {
            glMatrixMode(GL_MODELVIEW);
            glRotatef(2,1,0,0);
            display_cb();
        } else {
            glMatrixMode(GL_MODELVIEW);
            glRotatef(-2,1,0,0);
            display_cb();
        }
        
        prev_x = -1;
        prev_y = -1;
    }
}

void keyb_cb(unsigned char key, int x, int y)
{
//    printf("keyboard key pushed %c\n", key );
    
    switch(key)
    {
    case('z'): //zoom in key
        glMatrixMode(GL_MODELVIEW);
        glScalef(1.1,1.1,1.1);
        display_cb();
        break;
    case('a'): //zoom out key
        glMatrixMode(GL_MODELVIEW);
        glScalef(0.9,0.9,0.9);
        display_cb();
        break;
    case(27): //esc
        exit(0);
        break;
    case(32): //spacebar
        printf("toggling animation\n");
        time = time > 0 ? 0:1;
        break;
    case('t'): //toggle toon shader
        printf("toggling toon shader\n");
        toon = toon > 0 ? 0:1;
        break;
    }
}

void move_camera(int direction){
    printf("move camera called %i\n", direction);
    mod.camera_x = mod.camera_x + direction*(mod.lookat_x)*MOVE_STEP;
    mod.camera_z = mod.camera_z + direction*(mod.lookat_z)*MOVE_STEP;
    
    //proj.fovy = proj.fovy * 1.1;
    
    //glMatrixMode(GL_PROJECTION); 
    //glLoadIdentity();
    //gluPerspective (proj.fovy, proj.aspect, proj.near, proj.far); 
    
    update_modelview();
}


void rotate_camera(float angle){
    printf("rotate camera called %f\n", angle);
    mod.lookat_z = sin(angle) ;
    mod.lookat_x = -1.0f*cos(angle) ;
    update_modelview();
}


void sp_keyb_cb(int key, int x, int y)
{
    switch(key)
    {
    case(GLUT_KEY_UP):
//        printf("forward key pressed\n");
        glMatrixMode(GL_MODELVIEW);
        //glScalef(1.1,1.1,1.1);
        glRotatef(-10,1,0,0);
        display_cb();
        //move_camera(1);
        break;
    case (GLUT_KEY_DOWN):
//        printf("backward key pressed\n");
        glMatrixMode(GL_MODELVIEW);
        //glScalef(0.9,0.9,0.9);
        glRotatef(10,1,0,0);
        display_cb();
        //display_cb();
        //move_camera(-1);
        break;
    case (GLUT_KEY_LEFT):
//        printf("left key pressed\n");
        glMatrixMode(GL_MODELVIEW);
        glRotatef(-10,0,1,0);
        display_cb();
        //mod.angle += 0.1 ;
        //rotate_camera(mod.angle);
        break;
    case (GLUT_KEY_RIGHT):
//        printf("right key pressed\n");
        glMatrixMode(GL_MODELVIEW);
        glRotatef(10,0,1,0);
        display_cb();
        //mod.angle -= 0.1 ;
        //rotate_camera(mod.angle);
        break;
    }

}


void timer_cb(int value){
       if (time > 0)
          time++;
        glutTimerFunc(100, timer_cb, 0);
        display_cb();
    }

//resizes window
//perhaps maintain aspect and limit fov to get biggest posible pic?
void window_resize_cb(int width, int height){
 
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(height == 0)
		height = 1;

	float ratio = 1.0* width / height;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
	glViewport(0, 0, width, height);

	// Set the correct perspective.
	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0,0.0,5.0, 
		      0.0,0.0,-1.0,
			  0.0f,1.0f,0.0f);

    //int old_height, old_width;
    //old_height = 2*proj.near*sin(proj.fovy);
    //old_width = old_height*proj.aspect;
   /*
    printf("resize window cb called\n");
    float ratio = width / height;
    proj.aspect = ratio;

    proj.fovy = asin((height/2)/proj.near);
    update_projection();
    glViewport(0,0,width, height);
    */
    
}


void print_modelview(){
    printf("camera location: x %f y %f z %f\n", mod.camera_x, mod.camera_y,
    mod.camera_z);
    printf("eye direction: x %f y %f z %f\n", mod.lookat_x, mod.lookat_y,
    mod.lookat_z);
    }

void update_modelview(){
    glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    
    gluLookAt(mod.camera_x, mod.camera_y, mod.camera_z,
        mod.lookat_x, mod.lookat_y, mod.lookat_z,
       //(mod.camera_x + mod.lookat_x), (mod.camera_y + mod.lookat_y), (mod.camera_z + mod.lookat_z),
        mod.up_x, mod.up_y, mod.up_z);
    
// print_modelview();
    display_cb(); //remove later
}

void update_projection(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0, proj.width, proj.height);
    gluPerspective(proj.fovy, proj.aspect, proj.near, proj.far);
    display_cb();
    }




