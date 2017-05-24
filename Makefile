CC = gcc

CPPFLAGS = `pkg-config --cflags sdl`
CFLAGS = -Wall -Wextra  -std=c99 -O0 -g -pedantic
LDFLAGS =
LDLIBS = `pkg-config --libs sdl` -lm -lSDL_image

SRC =	list.c \
			matrix.c \
			SDL_operations.c \
			symbol.c \
			output.c \
			perceptron.c \
			main.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}
all: main

-include ${DEP}

main: ${OBJ}

clean:
	 rm -f ${OBJ} ${DEP} main

# END

