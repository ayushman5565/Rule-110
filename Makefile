CC = gcc
CFLAGS = -Wall -Wextra -std=c23
SDL_CFLAGS = -I/usr/include/SDL2 -D_REENTRANT
SDL_LIBS = -lSDL2

SRC = rule110.c
BINARY = bin

$(BINARY): $(SRC)
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $(SRC) -o $(BINARY) $(SDL_LIBS)

run: $(BINARY)
	./$(BINARY)

clean: 
	rm -rf $(BINARY)

