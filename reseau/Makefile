CC = gcc

CPPFLAGS = `pkg-config --cflags sdl`
CFLAGS = -Wall -Wextra  -std=c99 -O0 -g -pedantic
LDFLAGS =
LDLIBS = `pkg-config --libs sdl` -lm -lSDL_image

SRC =	../list.c \
			../symbol.c \
			../output.c \
			../matrix.c \
			../SDL_operations.c \
			../perceptron.c \
			reseau.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}
all: reseau

-include ${DEP}

reseau: ${OBJ}

clean:
	 rm -f reseau.o reseau

# END

