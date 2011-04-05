ALL = test

CC = g++

LIBS = rply

FLAGS = -Wall -g

INC = -IGL -IGLU -lglut -L./lib -I./include -I./ -lGLEW

all: test

test: main.cpp utils.cpp
	$(CC) $(FLAGS) -o test main.cpp utils.cpp $(INC) 

clean: 
	rm -f *.o test
 

fresh: clean all
