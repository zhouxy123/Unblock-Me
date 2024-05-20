CXX = g++
CPPFLAGS = -Wall -O3 -D_XOPEN_SOURCE_EXTENDED
LDFLAGS = -lncurses
all: 
	$(CXX) $(CPPFLAGS) -o unblock ./src/ui.cpp ./src/hash_lookup3.cc ./src/unblock.cpp ./cge/cge.c ./cge/cge_timer.c ./cge/cge_emitter.c ./cge/cge_box.c ./cge/log.c $(LDFLAGS)
clean:
	rm -fr *.o unblock


