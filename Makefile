#comment
CC=mpiCC
#CFLAGS=-DDEBUG -O0123
CFLAGS=-g -DDEBUG
LDLIBS=
INCLUDE=-I. -I./include
EXEC=prueba

all:
	$(CC) $(CFLAGS) $(INCLUDE) $(LDLIBS) terminal.cpp -o $(EXEC)
