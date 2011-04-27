CXX=c++
CXXFLAGS=-O2 -ansi -pedantic -Wall `sdl-config --cflags` -Iinclude -g
LDFLAGS=`sdl-config --libs` -lGL -lGLU
SRC:=$(wildcard src/*.cc)
OBJ:=$(SRC:.cc=.o)
DEP:=$(SRC:.cc=.d)

ALL: bin/raytrace

doc:
	doxygen

clean:
	rm -Rf doc/html doc/latex doc/*~ bin/* src/*~ src/*.d src/*.o *~

%.o: %.cc
	$(CXX) -MM $(CXXFLAGS) $< > $(subst .o,.d,$@)
	$(CXX) -c $(CXXFLAGS) $< -o $@

-include $(DEP)

bin/raytrace: $(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS) -lSDL_image
