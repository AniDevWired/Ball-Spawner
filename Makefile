CC = gcc

CFLAGS = $(shell pkg-config --cflags raylib) \
	-Ilibs \
	-Wall \
	-Wextra \
	-Werror \
	-Wpedantic \
	-std=c11 \
	-lm

LIBS = $(shell pkg-config --libs raylib)

SRC = src/main.c libs/ll.c
OUT = build/game

all:
	mkdir -p build
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LIBS)

run: all
	./$(OUT)

clean:
	rm -rf build