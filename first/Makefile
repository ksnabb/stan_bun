ALL = test

CC = g++

LIBS = rply

FLAGS = -Wall -g

INC = -IGL -IGLU -lglut -L./lib -I./include -I./ -lGLEW

all: test

test: opengl_test.c util.c glew_ui.c
	$(CC) $(FLAGS) -o test opengl_test.c util.c glew_ui.c $(INC) 

clean: 
	rm -f *.o test
 

fresh: clean all
