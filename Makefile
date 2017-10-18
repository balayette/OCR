CC=clang
CFLAGS= -Wall -Wextra -pedantic -std=c99 -Werror
CPPFLAGS= `pkg-config --cflags sdl` -MMD
LDLIBS= `pkg-config --libs sdl` -lSDL_image
LDFLAGS= -lm
OPT=
DBG=-g

NNSRC=$(wildcard sources/neuralnet/*.c)
IMGSRC=$(wildcard sources/imgprocessing/*.c)
MISCSRC=$(wildcard sources/misc/*.c)

all: neuralnet imgprocessing

neuralnet: $(NNSRC) $(MISCSRC)
	$(CC) -o $@.out $^ $(CFLAGS) $(DBG) $(OPT) $(LDFLAGS)

imgprocessing: $(IMGSRC) $(MISCSRC)
	$(CC) -o $@.out $^ $(CFLAGS) $(DBG) $(OPT) $(CPPFLAGS) $(LDLIBS) $(LDFLAGS)

clean:
	rm -f ./*.out
	rm -f ./*.d
