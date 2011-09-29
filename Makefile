CXX = g++
CXXFLAGS = -O2 -march=native -mno-avx -g
LDFLAGS = 

CXXFLAGS += $(shell sdl-config --cflags)
LDFLAGS += -lSDL_gfx $(shell sdl-config --libs)

objects = main.o exceptions.o unique.o actor.o dispatcher.o \
	rng.o setup.o global.o viewport.o events.o pixels.o \
	screenmessage.o cursor.o state.o

pixels: $(objects)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(objects) : %.o : %.cc
	$(CXX) -I./include -c -o $@ $< $(CXXFLAGS)

clean:
	-rm -f pixels $(objects) Makefile.depend

Makefile.depend: Makefile
	$(CXX) -I./include -MM $(objects:.o=.cc) > $@

.PHONY: clean

include Makefile.depend
