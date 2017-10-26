AUTOMAKE_OPTIONS = foreign
SUBDIRS = man doc
CFLAGS= -Wall -Wextra -pedantic -std=c99 -Werror
LDFLAGS=`pkg-config --libs sdl` -lSDL_image `pkg-config --cflags sdl` -MMD -lm

bin_PROGRAMS = neuralnet imgprocessing


neuralnet_SOURCES = src/sources/neuralnet/main.c src/sources/neuralnet/layer.c src/sources/neuralnet/neuralnet.c src/sources/neuralnet/save.c src/sources/misc/bool_matrix.c src/sources/misc/utils.c


imgprocessing_SOURCES = src/sources/imgprocessing/box.c src/sources/imgprocessing/boxes.c src/sources/imgprocessing/display.c src/sources/imgprocessing/drawing.c src/sources/imgprocessing/main.c src/sources/imgprocessing/pixop.c src/sources/imgprocessing/processing.c src/sources/imgprocessing/rxy.c src/sources/imgprocessing/recxy.c src/sources/misc/bool_matrix.c

include_HEADERS = src/headers/neuralnet/layer.h src/headers/neuralnet/neuralnet.h src/headers/neuralnet/save.h src/headers/misc/bool_matrix.h src/headers/misc/matrix.h src/headers/misc/utils.h src/headers/imgprocessing/processing.h src/headers/imgprocessing/recxy.h src/headers/imgprocessing/rxy.h src/headers/imgprocessing/pixop.h src/headers/imgprocessing/boxes.h src/headers/imgprocessing/box.h src/headers/imgprocessing/display.h src/headers/imgprocessing/drawing.h
