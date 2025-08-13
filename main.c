/* main.c */

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
#define MAX_ITER 300

typedef struct {
	double real;
	double imag;
} Complex;

Complex comp_add(Complex a, Complex b) {
	return (Complex){a.real + b.real, a.imag + b.imag};
}

Complex comp_mul(Complex a, Complex b) {
	return (Complex){a.real*b.real - a.imag*b.imag, a.real*b.imag + a.imag*b.real};
}

int is_in_set(Complex c) {
	Complex z = {0.0, 0.0};
	int it = 0;
	while ((z.real*z.real + z.imag*z.imag) <= 4.0 && it < MAX_ITER) {
		z = comp_add(comp_mul(z, z), c);
		it++;
	}

	return it;
}

double map_point_to_complex(double val, double in_max, double in_min, double out_max, double out_min) {
	return out_min + (val - in_min) * (out_max - out_min) / (in_max - in_min);
}

// Get color for current pixel based on number of iterations for it
Uint32 get_color_for_pixel(int it) {
	if (it == MAX_ITER) return 0x000000FF; // black when we know it is inside the fractal
	double temp = (double)it / MAX_ITER;
	Uint8 r = (Uint8)(9*(1-temp)*temp*temp*255);
	Uint8 g = (Uint8)(15*(1-temp)*(1-temp)*temp*255);
	Uint8 b = (Uint8)(9*(1-temp)*(1-temp)*(1-temp)*255);
	return ((r << 24) | (g << 16) | (b << 8) | 0xFF);
}

SDL_Window *window;
SDL_Renderer *render;
SDL_Texture *texture;

void cleanup() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL Error: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow("Mandelbrot set", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	Uint32 *pixels = (Uint32 *)malloc(SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(Uint32));
	if (!pixels) {
		fprintf(stderr, "Error allocating pixels array\n");
		cleanup();
		return 1;
	}

	double real_min = -2.0, real_max = 1.0;
	double imag_min = -1.5, imag_max = 1.5;

	for (int x = 0; x < SCREEN_WIDTH; x++) {
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			double px = map_point_to_complex(x, 0, SCREEN_WIDTH, real_min, real_max);
			double py = map_point_to_complex(y, 0, SCREEN_HEIGHT, imag_min, imag_max);
			int iter = is_in_set((Complex){px, py});
			pixels[y*SCREEN_WIDTH+x] = get_color_for_pixel(iter);
		}
	}

	SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));

	SDL_Event ev;
	int running = 1;
	while (running) {
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) {
				running = 0;
			} else if (ev.type == SDL_KEYDOWN) {
				switch (ev.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = 0;
					break;
				default:
					break;
				}
			}
		}
		SDL_RenderClear(render);
		SDL_RenderCopy(render, texture, NULL, NULL); // to draw on the whole screen I guess
		SDL_RenderPresent(render);
	}

	free(pixels);
	cleanup();

	return 0;
}
