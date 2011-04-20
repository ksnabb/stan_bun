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
#include <cgmath/cgmath.hh>
#include "raytracer.hh"
#include "lambert_shader.hh"
#include "point_light.hh"
#include "sphere_geometry.hh"
#include "plane_geometry.hh"
#include "triangle_geometry.hh"

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

raytracer*       tracer;
sphere_geometry* sphere1_geometry;
lambert_shader*  sphere1_shader;
object*          sphere1;
sphere_geometry* sphere2_geometry;
lambert_shader*  sphere2_shader;
object*          sphere2;
triangle_geometry* triangle1_geometry;
lambert_shader* triangle1_shader;
object*          triangle1;
plane_geometry*  plane1_geometry;
lambert_shader*  plane1_shader;
object*          plane1;
point_light*     light1;
point_light*     light2;

matrix_4d        projection_matrix;
SDL_Surface*     surf;
unsigned         scanline = 0;

int main (int argc, char* argv[])
{
  init_window (640, 480);
  init_scene ();
  event_loop ();
  return 0;
}

/// Initialize a window for single buffered OpenGL rendering.
void init_window (unsigned width, unsigned height)
{
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
  switch (event.key.keysym.sym)
    {
    case SDLK_ESCAPE: exit_event_loop = true;
    default: ;
    }
}

void handle_keyup (SDL_Event& event)
{}

void handle_mouse_button_down (SDL_Event& event)
{}

void handle_mouse_motion (SDL_Event& event)
{}

void init_scene (void)
{
  tracer  = new raytracer;

  sphere1_geometry = new sphere_geometry (vec(0.0, 0.0, 0.0), 1.0);
  sphere1_shader = new lambert_shader (vec(0.6,0.6,0.05));
  sphere1 = new object (translate(vec(-1.0, 0.0, 0.0)),
			sphere1_geometry, 
			sphere1_shader);

  sphere2_geometry = new sphere_geometry(vec(0.0, 0.0, 0.0), 1.0);
  sphere2_shader   = new lambert_shader (vec(0.05, 0.6, 0.6));
  sphere2 = new object (translate(vec(1.0, 0.0, 0.0)),
			sphere2_geometry,
			sphere2_shader);
  
  plane1_geometry = new plane_geometry (plane_3d(vec(0.0, 1.0, 0.0), 1.0));
  plane1_shader = new lambert_shader (vec(0.4,0.4,0.4));
  plane1 = new object(matrix_4d::identity(), 
		      plane1_geometry,
		      plane1_shader);
  
  triangle1_geometry = new triangle_geometry (triangle_3d(vec(0.0, 0.0, 0.0),
  vec(1.0, 1.0, 1.0), vec(1.0, 0.0, 1.0)));
  triangle1_shader = new lambert_shader (vec(0.4,0.4,0.4));
  triangle1 = new object(translate(vec(0.0, 1.0, 0.0)), 
		      triangle1_geometry,
		      triangle1_shader);
  

  light1 = new point_light (vec(3.0, 3.0, 3.0),  vec(20.0, 20.0, 20.0));
  light2 = new point_light (vec(-3.0, 3.0, -3.0), vec( 3.0, 3.0, 10.0));
  
  tracer->objects.push_back (sphere1);
  tracer->objects.push_back (sphere2);
  tracer->objects.push_back (plane1);
  tracer->objects.push_back (triangle1);
  tracer->lights.push_back  (light1);
  tracer->lights.push_back  (light2);
  
  surf = SDL_CreateRGBSurface (SDL_SWSURFACE,
			       640, 480, 32,
			       0x000000ff,
			       0x0000ff00,
			       0x00ff0000,
			       0xff000000);
  
  // Setup viewing transformations as in OpenGL.
  matrix_4d view = lookat(vec(0.0, 1.0, 6.0),
			  vec(0.0, 0.0, 0.0),
			  vec(0.0, 1.0, 0.0));
  matrix_4d P  = perspective (M_PI/4.0, 640.0/480.0, 0.5, 10000.0);
  matrix_4d VP = viewport<double>(0, 0, 640, 480);
  projection_matrix = VP * P * view;
}

/// Called when there are no events to handle. 
void idle (void)
{
  if (scanline < surf->h)
    tracer->trace_scanline (projection_matrix, surf, scanline++);
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
