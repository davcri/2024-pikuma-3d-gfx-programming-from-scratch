build:
	gcc -Wall -std=c99 ./src/*.c -o renderer $(sdl2-config --libs --cflags)

run:
	./renderer

clean:
	rm renderer