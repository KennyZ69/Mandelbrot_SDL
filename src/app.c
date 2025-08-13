/* app.c */

#include "in/app.h"
#include "in/mathutil.h"
#include <SDL2/SDL_stdinc.h>
#include <stdint.h>
#include <stdio.h>

void draw_mandelbrot(Uint32 *pixels, double real_max, double real_min, double imag_max, double imag_min) {
	for (int x = 0; x < SCREEN_WIDTH; x++) {
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			double px = map_point_to_complex(x, 0, SCREEN_WIDTH, real_min, real_max); // getting point on the x axis so the real axis from the gotten pixel x
			double py = map_point_to_complex(y, 0, SCREEN_HEIGHT, imag_min, imag_max); // getting point on the y axis so the imaginary axis from the gotten pixel y
			int iter = is_in_set((Complex){px, py}); // getting the iterations for the current complex number, whether it spirales or not and then getting color based on that
			pixels[y*SCREEN_WIDTH+x] = get_color_for_pixel(iter);
		}
	}
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

// Get color for current pixel based on number of iterations for it
Uint32 get_color_for_pixel(int it) {
	if (it == MAX_ITER) return 0x000000FF; // black when we know it is inside the fractal
	double temp = (double)it / MAX_ITER;
	
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
