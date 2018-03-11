libstruct=libstruct.a
cstruct=struct.c
hstruct=struct.h
cread=read_proc.c
hread=read_proc.h
libread=libread_proc.a

CC=gcc
CFLAG= -c -o


all: cstruct cread


cstruct: hstruct
	$(CC) $(cstruct) $(CFLAG) $(libstruct)

hstruct:
	$(CC) $(hstruct)

test: all
	$(CC) test.c -L. -lstruct -lread_proc -o test;./test

hread:cstruct
	$(CC) $(hread) 
cread:hread
	$(CC) $(cread) $(CFLAG) $(libread)

clean:
	rm *.a;rm *.gch;rm *.o; rm test;rm *~
