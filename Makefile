CC=clang
CFLAGS= -Wall -Wextra -pedantic -std=c99 -Werror
# CPPFLAGS= `pkg-config --cflags sdl` -MMD
SDL_CFLAGS:=$(shell sdl-config --cflags)
SDL_LDFLAGS:=$(shell sdl-config --libs) -lSDL_image
# LDLIBS= `pkg-config --libs sdl` -lSDL_image
LDFLAGS= -lm
OPT=
DBG=-g

NNSRC=$(wildcard sources/neuralnet/*.c)
IMGSRC=$(wildcard sources/imgprocessing/*.c)
MISCSRC=$(wildcard sources/misc/*.c)

all: neuralnet imgprocessing

neuralnet: $(NNSRC) $(MISCSRC)
	$(CC) -o $@ $^ $(CFLAGS) $(DBG) $(OPT) $(LDFLAGS)

imgprocessing: $(IMGSRC) $(MISCSRC)
	$(CC) -o $@ $^ $(CFLAGS) $(DBG) $(OPT) $(SDL_CFLAGS) $(LDLIBS) $(SDL_LDFLAGS) $(LDFLAGS)

clean:
	rm -f neuralnet
	rm -f imgprocessing
	rm -f ./*.d
	rm -f nn.save
