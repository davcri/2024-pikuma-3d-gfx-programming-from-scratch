HOMEBREW_LIBS := -I/opt/homebrew/include -L/opt/homebrew/lib
SDL2_FLAGS := `sdl2-config --libs --cflags`
LIBPNG_FLAGS := -lpng
GCC_OPTS = -Wall -std=c99

build:
	gcc $(GCC_OPTS) ./src/*.c $(HOMEBREW_LIBS) $(SDL2_FLAGS) $(LIBPNG_FLAGS) -o renderer

run:
	./renderer

clean:
	rm renderer