ALL = test

CC = gcc

LIBS = rply

FLAGS = -std=c99 -Wall -g

INC = -IGL -IGLU -lglut -L./lib -I./include -I./ -lGLEW

all: test

test: opengl_test.c
	$(CC) $(FLAGS) -o test opengl_test.c $(INC) 

clean: 
	rm -f *.o test
 

fresh: clean all
