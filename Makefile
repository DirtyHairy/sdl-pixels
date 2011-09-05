CCX = g++
CXXFLAGS = -O2 -march=native -mno-avx
LDFLAGS = 

CXXFLAGS += $(shell sdl-config --cflags)
LDFLAGS += -lSDL_gfx $(shell sdl-config --libs)

objects = pixels.o

pixels: $(objects)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(objects) : %.o : %.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
	-rm -f pixels $(objects)

.PHONY: clean
