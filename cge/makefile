CXX = g++
CPPFLAGS = -Wall -O3 -D_XOPEN_SOURCE_EXTENDED
LDFLAGS = -lncurses
all: 
	$(CXX) $(CPPFLAGS) -o cge draw.c cge.c cge_timer.c cge_emitter.c cge_box.c log.c $(LDFLAGS)
clean:
	rm -fr *.o cge


