CC = gcc
CFLAGS = -O3 -Wall
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -mwindows

SRCS = main.c algorithms.c
OBJS = main.o algorithms.o
EXEC = visualiser.exe

# Default build
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: main.c algorithms.h
	$(CC) $(CFLAGS) -c main.c -o main.o

algorithms.o: algorithms.c algorithms.h
	$(CC) $(CFLAGS) -c algorithms.c -o algorithms.o

# Windows delete command
clean:
	del /Q *.o
	del /Q $(EXEC)