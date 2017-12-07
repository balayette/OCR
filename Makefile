CFLAGS= -Wall -Wextra -pedantic -std=c99 -Werror -Iinclude/
CPPFLAGS=
LDLIBS=-lm
LDFLAGS=

MATRIXSRC=$(wildcard src/matrix/*.c)
MATRIXOBJ=$(MATRIXSRC:.c=.o)

NNSRC=$(wildcard src/neuralnet/*.c)
NNOBJ=$(NNSRC:.c=.o)

IMGSRC=$(wildcard src/imgprocessing/*.c)
IMGOBJ=$(IMGSRC:.c=.o)

MISCSRC=$(wildcard src/misc/*.c)
MISCOBJ=$(MISCSRC:.c=.o)

GENSRC=$(wildcard src/generate/*.c)
GENOBJ=$(GENSRC:.c=.o)

opti: CFLAGS += -O3
opti: all

debug: CFLAGS += -g
debug: all

all: build/neuralnet build/imgprocessing

build/neuralnet: build/neuralnet.o $(NNOBJ) $(MISCOBJ)
	$(LINK.o) $^ -o $@ $(LDLIBS)

build/imgprocessing: CPPFLAGS += `pkg-config --cflags sdl`
build/imgprocessing: LDLIBS += `pkg-config --libs SDL_image`
build/imgprocessing: build/imgprocessing.o $(IMGOBJ) $(MISCOBJ) $(MATRIXOBJ)
	$(LINK.o) $^ -o $@ $(LDLIBS)


generate: CPPFLAGS += `pkg-config --cflags SDL_ttf SDL_image`
generate: LDLIBS += `pkg-config --libs SDL_ttf SDL_image`
generate: $(GENOBJ) $(MISCOBJ) $(IMGOBJ) $(MATRIXOBJ)
	$(LINK.o) $^ -o $@ $(LDLIBS)


.PHONY: doc
doc:
	doxygen doc/doxyconfig
	+$(MAKE) -C doc/latex/
	cp doc/latex/refman.pdf doc/documentation.pdf
	rm -rf doc/latex
	rm -rf doc/html

clean:
	rm -f ./build/*.o
	rm -f ./build/neuralnet
	rm -f ./build/imgprocessing
	rm -f nn.save
	rm -f $(NNOBJ)
	rm -f $(IMGOBJ)
	rm -f $(MISCOBJ)
	rm -f $(GENOBJ)
	rm -f $(MATRIXOBJ)
	rm -f ./generate
