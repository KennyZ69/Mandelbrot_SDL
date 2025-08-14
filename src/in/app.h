/* app.h */

#pragma once

#include "mathutil.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 1000
#define MAX_ITER 1500
#define ZOOM_IN 0.90
#define ZOOM_OUT 1.0/ZOOM_IN
#define PAN 0.10

#define IMG_WIDTH 5000
#define IMG_HEIGHT 4000

typedef short int i8;

typedef struct {
	long double cx, cy;
	long double scale;
} Camera;

#define bool i8
#define true 1
#define false 0

void cleanup(SDL_Renderer *r, SDL_Window *w, SDL_Texture *t);

Uint32 get_color_for_pixel(int it);

int is_in_set(Complex c);

void draw_mandelbrot(Uint32 *pixels, double real_max, double real_min, double imag_max, double imag_min);

bool save_image(double real_max, double real_min, double imag_max, double imag_min);

void reset_cam(Camera *c);

void calc_bound(Camera *c, double *real_max, double *real_min, double *imag_max, double *imag_min);

void zoom_cam(Camera *c, double cx, double cy, double factor);

bool is_inside_bulbs(Complex c);
