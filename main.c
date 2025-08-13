/* main.c */

#include "src/in/app.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>

SDL_Window *window;
SDL_Renderer *render;
SDL_Texture *texture;

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
		cleanup(render, window, texture);
		return 1;
	}

	double real_min = -2.0, real_max = 1.0;
	double imag_min = -1.5, imag_max = 1.5;

	draw_mandelbrot(pixels, real_max, real_min, imag_max, imag_min);
	SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));

	SDL_Event ev;
	bool running = true;
	while (running) {
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) {
				running = false;
			} else if (ev.type == SDL_KEYDOWN) {
				switch (ev.key.keysym.sym) {
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_r:
					real_min = -2.0, real_max = 1.0;
					imag_min = -1.5, imag_max = 1.5;
					draw_mandelbrot(pixels, real_max, real_min, imag_max, imag_min);
					SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
					break;
				case SDLK_s:
					printf("Trying to save image of the mandelbrot set...\n");
					if (!save_image(real_max, real_min, imag_max, imag_min)) {
						return 1;
					}
					printf("Image saved\n");
					break;
				default:
					break;
				}
			}
			// } else if (ev.type == SDL_MOUSEWHEEL || ev.type == SDL_MOUSEBUTTONDOWN) {
			// 	int mx, my;
			// 	SDL_GetMouseState(&mx, &my);
			//
			// 	double cx = map_point_to_complex(mx, 0, SCREEN_WIDTH, real_max, real_min);
			// 	double cy = map_point_to_complex(my, 0, SCREEN_HEIGHT, imag_max, imag_min);
			//
			// 	double width = real_max - real_min;
			// 	double height = imag_max - imag_min;
			//
			// 	double zfactor;
			//
			// 	if ((ev.type == SDL_MOUSEWHEEL && ev.wheel.y > 0) || (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT)) {
			// 		zfactor = ZOOM;
			// 	} else if ((ev.type == SDL_MOUSEWHEEL && ev.wheel.y < 0) || (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_RIGHT)) {
			// 		zfactor = 1.0 / ZOOM;
			// 	} else {
			// 		zfactor = 1.0;
			// 	}
			//
			// 	width *= zfactor;
			// 	height *= zfactor;
			//
			// 	real_min = cx - width / 2;
			// 	real_max = cx + width / 2;
			// 	imag_min = cy - height / 2;
			// 	imag_max = cy + height / 2;
			//
			// 	draw_mandelbrot(pixels, real_max, real_min, imag_max, imag_min);
			// 	SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
			// }
		}
		SDL_RenderClear(render);
		SDL_RenderCopy(render, texture, NULL, NULL); // to draw on the whole screen I guess
		SDL_RenderPresent(render);
	}

	free(pixels);
	cleanup(render, window, texture);

	return 0;
}
