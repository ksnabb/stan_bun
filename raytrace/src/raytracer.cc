#include <iostream>
#include <cassert>
#include <stdlib.h>
#include "raytracer.hh"
#include "surface_point.hh"

using namespace std;
using namespace cgmath;

double UNSHADED_LIGHT = 0.1;
bool INDIRECT_ENABLED = true;
int SAMPLES_PER_PIXEL = 4;

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

vector_3d raytracer::trace (const ray_3d& ray, bool fromcamera)
{
  surface_point sp;
  if (hit_surface (ray, numeric_limits<double>::infinity(), sp))
    return shade (normalized(-ray.d), sp, fromcamera);
  else
    return shade_exiting (ray);
}

vector_3d raytracer::shade (const vector_3d& out, 
			    const surface_point& sp, bool fromcamera)
{
  vector_3d color (0.0);
  // Handle reflections from lights

  for (unsigned i = 0; i < lights.size(); ++i)
    {
      light_samples samples;
      lights[i]->illuminate (sp.point, 1, samples);
      // Here you should check that the light is not
      // occluded to get shadows.
      
      vector_3d radiance = vec(0.0, 0.0, 0.0);
      if (fromcamera)
          radiance += samples[0].radiance;
      if (!hit_object(ray_3d(sp.point, samples[0].direction),numeric_limits<double>::infinity()))
            color += mul(sp.bsdf(-normalized(samples[0].direction), 
            out), radiance);
    }
  if (INDIRECT_ENABLED){
      if (fromcamera || rand()%2){//50% chance
            bsdf_samples sample_vecs;
            sp.sample_bsdf(1, out, sample_vecs);
            //no vector * scalar operation so just add twice
            vector_3d result = trace(ray_3d(sp.point, 
                sample_vecs[0].direction), false);
            result = mul(result, sample_vecs[0].weight);
            color += result;
            if (!fromcamera)
                color += result;
      }
    }
  // In addition to direct lighting, the surface should
  // get light from other surfaces.
  return color;
}

vector_3d raytracer::shade_exiting (const cgmath::ray_3d& ray)
{
  // Color of ray that exits the scene.
  return vec(0.1, 0.1, 0.1);
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
      vector_3d color = vec(0.0, 0.0, 0.0);
      for (int i = 0; i < SAMPLES_PER_PIXEL; i++)
          color += trace(ray_3d(eye, p1-eye), true);
      color[0] = color[0]/(double)SAMPLES_PER_PIXEL;
      color[1] = color[1]/(double)SAMPLES_PER_PIXEL;
      color[2] = color[2]/(double)SAMPLES_PER_PIXEL;
      put_pixel (s, x, y, color);
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
