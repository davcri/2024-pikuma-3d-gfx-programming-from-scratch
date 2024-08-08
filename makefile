SDL2_FLAGS := `sdl2-config --libs --cflags`

build:
	gcc -Wall -std=c99 ./src/*.c $(SDL2_FLAGS) -o renderer

run:
	./renderer

clean:
	rm renderer