#include <iostream>
#include <cassert>
#include "raytracer.hh"
#include "surface_point.hh"

using namespace std;
using namespace cgmath;

static void put_pixel (SDL_Surface *surface, int x, int y, const vector_3d& color);

raytracer::raytracer (void)
{}

object* raytracer::hit_object (const cgmath::ray_3d& ray, double max_l)
{
  object* obj = 0;
  for (unsigned i = 0; i < objects.size(); ++i)
    {
      double ti = objects[i]->hit (ray);
      if (ti > 1e-6 && ti < max_l)
	{
	  max_l = ti;
	  obj = objects[i];
	}
    }
  return obj;
}

object* raytracer::hit_surface (const cgmath::ray_3d& ray, 
				double max_l,
				surface_point& sp)
{
  object* obj = hit_object (ray, max_l);
  if (obj)
    obj->hit_surface_point(sp);
  return obj;
}

vector_3d raytracer::trace (const ray_3d& ray)
{
  surface_point sp;
  if (hit_surface (ray, numeric_limits<double>::infinity(), sp))
    return shade (normalized(-ray.d), sp);
  else
    return shade_exiting (ray);
}

vector_3d raytracer::shade (const vector_3d& out, 
			    const surface_point& sp)
{
  vector_3d color (0.0);
  // Handle reflections from lights
  for (unsigned i = 0; i < lights.size(); ++i)
    {
      light_samples samples;
      lights[i]->illuminate (sp.point, 1, samples);
      // Here you should check that the light is not
      // occluded to get shadows.
      color += mul(sp.bsdf(-normalized(samples[0].direction), out), 
		   samples[0].radiance);
    }
  // In addition to direct lighting, the surface should
  // get light from other surfaces.
  return color;
}

vector_3d raytracer::shade_exiting (const cgmath::ray_3d& ray)
{
  // Color of ray that exits the scene.
  return vec(0.05, 0.05, 0.05);
}

void raytracer::trace_image (const matrix_4d& P, SDL_Surface* s)
{
  for (int y = 0; y < s->h; ++y)
    trace_scanline (P, s, y);
}

void raytracer::trace_scanline (const matrix_4d& P, 
				SDL_Surface* s,
				unsigned y)
{
  matrix_4d iP  = inv(P);
  vector_3d eye = cartesian (iP * vec(0.0, 0.0, 1.0, 0.0));
  for (int x = 0; x < s->w; ++x)
    {
      vector_3d p1 = cartesian(iP * vec<double>(x, y, 1.0, 1.0));
      put_pixel (s, x, y, trace (ray_3d (eye, p1 - eye)));
    }
}

/// Sets pixel at x,y in surface to color. Adapted from SDL examples.
void put_pixel (SDL_Surface *surface, int x, int y, const vector_3d& color)
{
  // Gamma correct, clamp, and round color values to 8 bit.
  Uint8 r = round(255 * pow(max(0.0, min(color(0), 1.0)), 1.0/2.2));
  Uint8 g = round(255 * pow(max(0.0, min(color(1), 1.0)), 1.0/2.2));
  Uint8 b = round(255 * pow(max(0.0, min(color(2), 1.0)), 1.0/2.2));
  Uint32 pixel = SDL_MapRGBA (surface->format, r, g, b, 255);
  int bpp = surface->format->BytesPerPixel;
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
  switch(bpp) 
    {
    case 1:
      *p = pixel;
      break;
    case 2:
      *(Uint16 *)p = pixel;
      break;
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	p[0] = (pixel >> 16) & 0xff;
	p[1] = (pixel >> 8) & 0xff;
	p[2] = pixel & 0xff;
      } else {
	p[0] = pixel & 0xff;
	p[1] = (pixel >> 8) & 0xff;
	p[2] = (pixel >> 16) & 0xff;
      }
      break;
    case 4:
      *(Uint32 *)p = pixel;
      break;
    }
}
