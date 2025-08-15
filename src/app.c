/* app.c */

#include "in/app.h"
#include "in/mathutil.h"
#include <SDL2/SDL_stdinc.h>
#include <stdint.h>
#include <stdio.h>

void draw_mandelbrot(Uint32 *pixels, double real_max, double real_min, double imag_max, double imag_min) {
	#pragma omp parallel for schedule(dynamic, 4)
	for (int x = 0; x < SCREEN_WIDTH; x++) {
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			long double px = map_point_to_complex(x, 0, SCREEN_WIDTH, real_min, real_max); // getting point on the x axis so the real axis from the gotten pixel x
			long double py = map_point_to_complex(y, 0, SCREEN_HEIGHT, imag_min, imag_max); // getting point on the y axis so the imaginary axis from the gotten pixel y
			// int iter = is_in_set((Complex){px, py}); // getting the iterations for the current complex number, whether it spirales or not and then getting color based on that
			int iter = is_in_julia((Complex){px, py}, julia_c);
			pixels[y*SCREEN_WIDTH+x] = get_color_for_pixel(iter);
		}
	}
	

	// static long double *real_axis = NULL;
	// static long double *imag_axis = NULL;
	// static int alloc_w = 0, alloc_h = 0;
	//
	// if (alloc_w != SCREEN_WIDTH) {
	// 	free(real_axis);
	// 	real_axis = (long double*)malloc(SCREEN_WIDTH*sizeof(long double));
	// 	alloc_w = SCREEN_WIDTH;
	// }
	// if (alloc_h != SCREEN_HEIGHT) {
	// 	free(imag_axis);
	// 	imag_axis = (long double*)malloc(SCREEN_HEIGHT*sizeof(long double));
	// 	alloc_h = SCREEN_HEIGHT;
	// }
	//
	// for (int x = 0; x < SCREEN_WIDTH; x++) {
	// 	real_axis[x] = map_point_to_complex(x, 0, SCREEN_WIDTH, real_max, real_min);
	// }
	// for (int y = 0; y < SCREEN_HEIGHT; y++) {
	// 	imag_axis[y] = map_point_to_complex(y, 0, SCREEN_HEIGHT, imag_max, imag_min);
	// }
	//
	// // parallize rows if compiled with -fopenmp
	// #pragma omp parallel for schedule(dynamic, 4)
	// for (int y = 0; y < SCREEN_HEIGHT; y++) {
	// 	long double cy = imag_axis[y];
	// 	Uint32 *row = pixels + (size_t)y * SCREEN_WIDTH;
	// 	for (int x = 0; x < SCREEN_WIDTH; x++) {
	// 		long double cx = real_axis[x];
	// 		// double it = is_in_set((Complex){cx, cy});
	// 		double it = is_in_julia((Complex){cx, cy}, julia_c);
	// 		row[x] = get_color_for_pixel(it);
	// 	}
	// }
}

int is_in_set(Complex c) {
	if(is_inside_bulbs(c)) return MAX_ITER;

	Complex z = {0.0, 0.0};
	int it = 0;
	while ((z.real*z.real + z.imag*z.imag) <= 4.0 && it < MAX_ITER) {
		z = comp_add(comp_mul(z, z), c);
		it++;
	}

	return it;
}

int is_in_julia(Complex z, const Complex c) {
	// probably there is no way to do an early escape check
	int it = 0;
	while ((z.real*z.real + z.imag*z.imag) <= 4.0 && it < MAX_ITER) {
		z = comp_add(comp_mul(z, z), c);
		it++;
	} 
	return it;
}

// Get color for current pixel based on number of iterations for it
Uint32 get_color_for_pixel(int it) {
	if (it == MAX_ITER) return 0x000000FF; // black when we know it is inside the fractal
	// double temp = (double)it / MAX_ITER;
	
	Uint8 r = (Uint8)(3*it%255);
	Uint8 g = (Uint8)(6*it%255);
	Uint8 b = (Uint8)(9*it%255);

	return ((r << 24) | (g << 16) | (b << 8) | 0xFF);
}


void cleanup(SDL_Renderer *r, SDL_Window *w, SDL_Texture *t) {
	SDL_DestroyTexture(t);
	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	SDL_Quit();
}

bool save_image(double real_max, double real_min, double imag_max, double imag_min) {
	// Uint32 *image = (Uint32*)malloc(IMG_HEIGHT*IMG_WIDTH*sizeof(Uint32));
	// if (!image) {
	// 	fprintf(stderr, "Error allocating image buffer\n");
	// 	return false;
	// }

	FILE *f = fopen("mandelbrot.ppm", "wb");
	if (!f) {
		fprintf(stderr, "Failed to open .ppm file\n");
		// free(image);
		return false;
	}
	fprintf(f, "P6\n%d %d\n255\n", IMG_WIDTH, IMG_HEIGHT);

	printf("Making the mandelbrot set...\n");

	for (int y = 0; y < IMG_HEIGHT; y++) {
		for (int x = 0; x < IMG_WIDTH; x++) {
			double cx = map_point_to_complex(x, 0, IMG_WIDTH, real_max, real_min);
			double cy = map_point_to_complex(y, 0, IMG_HEIGHT, imag_max, imag_min);

			int iter = is_in_set((Complex){cx, cy});
			// image[y*IMG_WIDTH+x] = colors_for_image(iter);
			// image[y*IMG_WIDTH+x] = get_color_for_pixel(iter);
			Uint32 color = get_color_for_pixel(iter);

			unsigned char rgb[3] = {
				(color >> 16) & 0xFF,
				(color >> 8) & 0xFF,
				color & 0xFF,
			};
			fwrite(rgb, 1, 3, f);
		}
	}

	// fwrite(image, 3, IMG_HEIGHT * IMG_WIDTH, f);
	fclose(f);

	// free(image);

	return true;
}

void reset_cam(Camera *c) {
	c->cx = -0.5;
	c->cy = 0.0;
	c->scale = 3.0; // full set width -> the x axis is from -2 to 1 so 3
}

void zoom_cam(Camera *c, double cx, double cy, double factor) {
	c->scale *= factor;
	c->cx = cx + (c->cx - cx) * factor;
	c->cy = cy + (c->cy - cy) * factor;
}

void calc_bound(Camera *c, double *real_max, double *real_min, double *imag_max, double *imag_min) {
	long double halfw = c->scale * 0.5;
	long double halfh = c->scale * SCREEN_HEIGHT / SCREEN_WIDTH * 0.5;
	*real_min = c->cx - halfw;
	*real_max = c->cx + halfw;
	*imag_min = c->cy - halfh;
	*imag_max = c->cy + halfh;
}

void change_iter_per_scale(double scale) {
	long double zoom_lvl = log(3.0 / scale) / log(2.0); // halves from intial scale
	int it = 200 + (80.0 * (zoom_lvl > 0.0 ? zoom_lvl : 0.0));
	if (it > MAX_ITER) it = MAX_ITER;
}

bool is_inside_bulbs(Complex c) {
	// check main cardioid
	long double x = c.real - 0.25;
	long double q = x*x + c.imag*c.imag;
	if (q * (q + x) < 0.25 * c.imag * c.imag) return true;
	// period-2 bulb, at (-1; 0); r = 1/4
	long double dx = c.real + 1.0;
	if (dx*dx + c.imag*c.imag < 0.25 * 0.25) return true;
	return false;
}
