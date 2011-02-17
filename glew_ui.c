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

float MOVE_STEP = 0.1;

void mouse_motion_cb(int x, int y)
{
    printf("mouse moved x: %i, y: %i\n", x, y);

}

void keyb_cb(unsigned char key, int x, int y)
{
    printf("keyboard key pushed %c\n", key );
}

void move_camera(int direction){
    mod.camera_x = mod.camera_x + direction*(mod.lookat_x)*MOVE_STEP;
    mod.camera_z = mod.camera_z + direction*(mod.lookat_z)*MOVE_STEP;
    update_modelview();
}


void rotate_camera(float angle){
    mod.lookat_z = sin(angle) ;
    mod.lookat_x = -1.0f*cos(angle) ;
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
        mod.angle += 0.1 ;
        rotate_camera(mod.angle);
        break;
    case (GLUT_KEY_RIGHT):
        printf("right key pressed\n");
        mod.angle -= 0.1 ;
        rotate_camera(mod.angle);
        break;
    }

}



void timer_cb(int value){
// display_cb();
    }

//resizes window
//perhaps maintain aspect and limit fov to get biggest posible pic?
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


void print_modelview(){
    printf("camera location: x %f y %f z %f\n", mod.camera_x, mod.camera_y,
    mod.camera_z);
    printf("eye direction: x %f y %f z %f\n", mod.lookat_x, mod.lookat_y,
    mod.lookat_z);
    }

void update_modelview(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(mod.camera_x, mod.camera_y, mod.camera_z,
    (mod.camera_x + mod.lookat_x), (mod.camera_y+mod.lookat_y), (mod.camera_z+
    mod.lookat_z),
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




