CC=clang
CFLAGS= -Wall -Wextra -pedantic -std=c99 -Werror -g
CPPFLAGS= `pkg-config --cflags sdl` -MMD
LDLIBS= `pkg-config --libs sdl` -lSDL_image
LDFLAGS= -lm

NNSRC=$(wildcard sources/neuralnet/*.c)
IMGSRC=$(wildcard sources/imgprocessing/*.c)
MISCSRC=$(wildcard sources/misc/*.c)

all: neuralnet imgprocessing

neuralnet: $(NNSRC) $(MISCSRC)
	$(CC) -o $@.out $^ $(CFLAGS) $(LDFLAGS)

imgprocessing: $(IMGSRC) $(MISCSRC)
	$(CC) -o $@.out $^ $(CFLAGS) $(CPPFLAGS) $(LDLIBS) $(LDFLAGS)

clean:
	rm -f ./*.out
	rm -f ./*.d
