CFLAGS= -Wall -Wextra -pedantic -std=c99 -Werror -Iinclude/
CPPFLAGS=
LDLIBS=-lm
LDFLAGS=

NNSRC=$(wildcard src/neuralnet/*.c)
NNOBJ=$(NNSRC:.c=.o)

IMGSRC=$(wildcard src/imgprocessing/*.c)
IMGOBJ=$(IMGSRC:.c=.o)

MISCSRC=$(wildcard src/misc/*.c)
MISCOBJ=$(MISCSRC:.c=.o)

opti: CFLAGS += -O3
opti: all

all: neuralnet imgprocessing

neuralnet: $(NNOBJ) $(MISCOBJ)
	$(LINK.o) $^ -o $@ $(LDLIBS)

imgprocessing: CPPFLAGS += `pkg-config --cflags sdl` -MMD
imgprocessing: LDLIBS += `pkg-config --libs sdl` -lSDL_image
imgprocessing: $(IMGOBJ) $(MISCOBJ)
	$(LINK.o) $^ -o $@ $(LDLIBS)

clean:
	rm -f ./neuralnet
	rm -f ./imgprocessing
	rm -f $(SRCOBJ)
	rm -f $(IMGOBJ)
	rm -f $(MISCOBJ)

