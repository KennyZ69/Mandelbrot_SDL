CFLAGS = -march=native -ffast-math -Wall -Wextra -Iinclude $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lm -fopenmp

SRC = src/*.c main.c
OUT = mandelbrot

all: $(OUT)

$(OUT): $(SRC)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OUT) $(OUT).ppm

run:
	./$(OUT)
