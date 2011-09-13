CXX = g++
CXXFLAGS = -O2 -march=native -mno-avx
LDFLAGS = 

CXXFLAGS += $(shell sdl-config --cflags)
LDFLAGS += -lSDL_gfx $(shell sdl-config --libs)

objects = pixels.o exceptions.o unique.o actor.o dispatcher.o

pixels: $(objects)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(objects) : %.o : %.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	-rm -f pixels $(objects) Makefile.depend

Makefile.depend: Makefile
	$(CXX) -MM $(objects:.o=.cc) > $@

.PHONY: clean

include Makefile.depend
