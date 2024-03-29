#comment
CC=mpiCC
#CFLAGS=-DDEBUG -O0123
CFLAGS=-g -DDEBUG
LDLIBS=
INCLUDE=-I. -I./include
EXEC=CloudService
OBJS=datatree.o terminal.o diskmanager.o

all: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(LDLIBS) $(OBJS) main.cpp -o $(EXEC)
	$(CC) disk.cpp -o disk
#para cualquier .o que falte, busco sus cpp
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
