// -*- mode: c++; compile-command: "cd ..; make bin/24.raytrace" -*-
/// \file 24.raytrace.cc
/// \brief Raytracing example.

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <ciso646>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <cgmath/cgmath.hh>
#include "raytracer.hh"
#include "lambert_shader.hh"
#include "mirror_shader.hh"
#include "glass_shader.hh"
#include "phong_shader.hh"
#include "point_light.hh"
#include "sphere_geometry.hh"
#include "plane_geometry.hh"
#include "triangle_geometry.hh"
#include "mesh_object.hh"
#include "slow_mesh_object.hh"

using namespace std;
using namespace cgmath;

static void init_window      (unsigned width, unsigned height);
static void event_loop       (void);
static void handle_event     (SDL_Event& event);
static void handle_keyup     (SDL_Event& event);
static void handle_keydown   (SDL_Event& event);
static void handle_mouse_button_down (SDL_Event& event);
static void handle_mouse_motion      (SDL_Event& event);
static void idle             (void);
static void init_scene       (void);
static void update_scene     (void);
static void render_scene     (void);

static bool exit_event_loop = false;

extern bool INDIRECT_ENABLED;
extern int SAMPLES_PER_PIXEL;
extern bool QUASIRANDOM;
char * FNAME;
bool PHONG;
bool ACCELERATED;
bool MIRROR; //ok implementation
bool GLASS; //bad implementation

time_t start_time;
bool time_shown = false;

raytracer*       tracer;
sphere_geometry* sphere1_geometry;
lambert_shader*  sphere1_shader;
object*          sphere1;
sphere_geometry* sphere2_geometry;
lambert_shader*  sphere2_shader;
object*          sphere2;
sphere_geometry* sphere3_geometry;
glass_shader*  glass1_shader;
object*          sphere3;
triangle_geometry* triangle1_geometry;
lambert_shader* triangle1_shader;
object*          triangle1;
geometry* mesh1_geometry;
lambert_shader* mesh1_shader;
object*          mesh1;
object*         mesh2;

mirror_shader * mirror1_shader;
phong_shader * phong1_shader;

plane_geometry*  plane1_geometry;
lambert_shader*  plane1_shader;
object*          plane1;
point_light*     light1;
point_light*     light2;

matrix_4d        projection_matrix;
SDL_Surface*     surf;
unsigned         scanline = 0;

void printhelp(){
    printf("raytraycer by jsuvileh and ksnabb\n"
            "usage:bin/raytrace [-q] [-i] [-p] [-n number] [-f filename] [-a]\n"
            "-q\t\tdisable quasirandom sampling\n"
            "-i\t\tdisable indirect lightning\n"
            "-p\t\tmake one ball a black phong surface with no diffuse part\n"
            "-a\t\tdisable acceleration structure"
            "-n number\t\tnumber of samples per pixel\n"
            "-f filename\t\tfilename of .ply file to be plotted in the"
            "foreground\n"
            );

}

int main (int argc, char* argv[])
{
    //  cout << "main function called\n";
    //default values
    INDIRECT_ENABLED = true;
    SAMPLES_PER_PIXEL = 4;
    QUASIRANDOM = true;
    FNAME = "./box.ply";
    PHONG = false;
    ACCELERATED = true;
    GLASS = false;
    MIRROR = false;
    for (int i=0; i < argc; i++) {
        if(strcmp(argv[i],"-q") == 0){
            printf("Disabling quasirandom sampling\n");
            QUASIRANDOM = false;
        }
        else if(strcmp(argv[i],"-i") == 0){
            printf("Disabling indirect lightning\n");
            INDIRECT_ENABLED = false;
        }
        else if(strcmp(argv[i],"-f") == 0){
            printf("Using file %s\n", argv[i+1]); 
            FNAME = argv[i+1];
            i++;
        }
        else if(strcmp(argv[i],"-p") == 0){
            printf("Making one sphere a black phong sphere\n");
            PHONG = true;
        } else if (strcmp(argv[i],"-a") == 0){
            printf("Disabling acceleration. Better get coffee now\n");
            ACCELERATED = false;        
        } else if (strcmp(argv[i], "-n") == 0){
            SAMPLES_PER_PIXEL = atoi(argv[i+1]);
            printf("sampling %d rays per pixel\n", SAMPLES_PER_PIXEL);
         } else if (strcmp(argv[i], "-m") == 0){
            MIRROR = true;
            printf("making one ball a mirror\n");
         } else if (strcmp(argv[i], "-g") == 0){
            GLASS = true;
            printf("adding a glass ball\n");
        } else if (strcmp(argv[i], "-h") == 0){
            printhelp();
            return 0;
        }


    }
    init_window (640, 480);
    init_scene ();
    start_time = time(NULL);
    event_loop ();
    return 0;
}

/// Initialize a window for single buffered OpenGL rendering.
void init_window (unsigned width, unsigned height)
{
    cout << "init_window function called\n";
    // Initialize SDL and OpenGL with 8 bits for each RGBA. No double buffering.
    SDL_Init (SDL_INIT_VIDEO);
    atexit (SDL_Quit);
    SDL_GL_SetAttribute (SDL_GL_RED_SIZE,     8);
    SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE,   8);
    SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE,    8);
    SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE,   8);
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
    SDL_Surface* surface;
    surface = SDL_SetVideoMode (width, height, 32, SDL_OPENGL);
    assert (surface);
}

/// Handle events. Do idle processing while there are no events.
void event_loop (void)
{
    cout << "event_loop function called\n";
    SDL_Event event;
    while (not exit_event_loop)
        if (SDL_PollEvent (&event))
            handle_event (event);
        else
            idle ();
}

/// Handle a given event. Dispatches by type to other event handlers.
void handle_event (SDL_Event& event)
{
    // cout << "handle_event function called\n";
    switch (event.type) 
    {
        case SDL_KEYDOWN:         handle_keydown (event); break;
        case SDL_KEYUP:           handle_keyup   (event); break;
        case SDL_MOUSEMOTION:     handle_mouse_motion (event); break;
        case SDL_MOUSEBUTTONDOWN: handle_mouse_button_down (event); break;
    }
}

/// Handle pressing a key on the keyboard. Set quit on Esc pressed.
void handle_keydown (SDL_Event& event)
{
    cout << "handle_keydown function called\n";
    switch (event.key.keysym.sym)
    {
        case SDLK_ESCAPE: exit_event_loop = true;
        default: ;
    }
}

void handle_keyup (SDL_Event& event)
{
    cout << "handle_keyup\n";
}

void handle_mouse_button_down (SDL_Event& event)
{}

void handle_mouse_motion (SDL_Event& event)
{}

void init_scene (void)
{

    cout << "init_scene function called\n";
    tracer  = new raytracer;

    mirror1_shader = new mirror_shader();
    glass1_shader = new glass_shader(1.1);
    //an almost black thing that has white reflection
    phong1_shader = new phong_shader(vec(0.0, 0.0, 0.0), vec(1.0, 1.0, 1.0),
            128.0);


    sphere1_geometry = new sphere_geometry (vec(0.0, 0.0, 0.0), 1.0);
    sphere1_shader = new lambert_shader (vec(0.6,0.6,0.05));
    if(!PHONG)
    sphere1 = new object (translate(vec(-1.0, 0.0, 0.0)),
            sphere1_geometry, 
            sphere1_shader);
    else
    sphere1 = new object (translate(vec(-1.0, 0.0, 0.0)),
            sphere1_geometry, 
            phong1_shader);

    sphere2_geometry = new sphere_geometry(vec(0.0, 0.0, 0.0), 1.0);
    sphere2_shader   = new lambert_shader (vec(0.05, 0.6, 0.6));
    if(!MIRROR)
    sphere2 = new object (translate(vec(1.0, 0.0, 0.0)),
            sphere2_geometry,
            sphere2_shader);
    else
    sphere2 = new object (translate(vec(1.0, 0.0, 0.0)),
            sphere2_geometry,
            mirror1_shader);

    sphere3_geometry = new sphere_geometry(vec(0.0, 0.0, 0.0), 1.0);
    sphere3 = new object (translate(vec(0.0, 0.0, 2.0)),
            sphere3_geometry,
            glass1_shader);

    plane1_geometry = new plane_geometry (plane_3d(vec(0.0, 1.0, 0.0), 1.0));
    plane1_shader = new lambert_shader (vec(0.4,0.04,0.4));
    plane1 = new object(matrix_4d::identity(), 
            plane1_geometry,
            plane1_shader);
    //              mirror1_shader);

    triangle1_geometry = new triangle_geometry (triangle_3d(
                vec(0.0, 0.0, 0.0),
                vec(-1.0, 1.0, 1.0),
                vec(1.0, 1.0, 1.0)
                ));
    triangle1_shader = new lambert_shader (vec(0.5,0.2,0.2));
    triangle1 = new object(translate(vec(1.0, 1.0, 0.0)), 
            triangle1_geometry,
            triangle1_shader);

    mesh1_geometry = new mesh_geometry (FNAME, ACCELERATED);

    matrix_4d obj_trans = translate(vec(1.0, 1.1, 1.0));
    if (strncmp(FNAME, "./bun", 4) == 0)
        obj_trans = obj_trans * scale(6.0, 6.0, 6.0);

    mesh1_shader = new lambert_shader (vec(0.5,0.2,0.2));
    mesh1 = new object(obj_trans, 
            mesh1_geometry,
            mesh1_shader);
    mesh2 = new object(translate(vec(0.5, 2.0, 2.0)),
            mesh1_geometry,
            mesh1_shader);

    light1 = new point_light (vec(3.0, 3.0, 3.0), vec(20.0, 20.0, 20.0));
    light2 = new point_light (vec(-3.0, 3.0, -3.0), vec( 3.0, 3.0, 10.0));


    tracer->objects.push_back (sphere1);
    tracer->objects.push_back (sphere2);
    if(GLASS)
        tracer->objects.push_back (sphere3);
    tracer->objects.push_back (plane1);
    //  tracer->objects.push_back (triangle1);
    tracer->objects.push_back (mesh1);
//    tracer->objects.push_back (mesh2);
    tracer->lights.push_back  (light1);
    tracer->lights.push_back  (light2);

    cout << "create SDL surface\n";
    //this one blocking execution ??
    surf = SDL_CreateRGBSurface (SDL_SWSURFACE, 
            640, 480, 32,
            0x000000ff,
            0x0000ff00,
            0x00ff0000,
            0xff000000);

    cout << "created sdl surface\n";

    // Setup viewing transformations as in OpenGL.
    matrix_4d view = lookat(vec(0.0, 1.0, 6.0),
            vec(0.0, 0.0, 0.0),
            vec(0.0, 1.0, 0.0));
    matrix_4d P  = perspective (M_PI/4.0, 640.0/480.0, 0.5, 10000.0);
    matrix_4d VP = viewport<double>(0, 0, 640, 480);
    projection_matrix = VP * P * view;

    cout << "init_scene function done\n";
}

/// Called when there are no events to handle. 
void idle (void)
{
    if (scanline < surf->h)
        tracer->trace_scanline (projection_matrix, surf, scanline++);
    else if (!time_shown){
        time_shown = true;
        int duration = (int)time(NULL)-start_time;
        printf("Rendered scene with %d rays in %d seconds with %f rays/sec\n",
        surf->h*surf->w*SAMPLES_PER_PIXEL, duration,
        surf->h*surf->w*SAMPLES_PER_PIXEL/(1.0*duration));
        }
    render_scene ();
}

void update_scene (void)
{
    tracer->trace_image(projection_matrix, surf);
}

/// Shows the raytraced image using OpenGL.
void render_scene     (void)
{
    glClear (GL_COLOR_BUFFER_BIT);
    // This is a bit crappy way to do this. 
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glViewport (0, 0, 640, 480);
    glRasterPos2s (-1, -1);
    glDrawPixels (surf->w, surf->h, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
    SDL_GL_SwapBuffers ();
}
