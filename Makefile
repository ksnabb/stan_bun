ALL = test

CC = gcc

LIBS = rply

FLAGS = -std=c99 -Wall

INC = -IGL -IGLU -lglut -L./lib -I./include -I./ -lglew

all: test

test: opengl_test.c
	$(CC) -o test opengl_test.c $(INC) 

clean: 
	rm -f *.o test
 

fresh: clean all
