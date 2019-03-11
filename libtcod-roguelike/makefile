SOURCES=$(wildcard src/*.cpp)
OBJS=$(SOURCES:.cpp=.o)
ifeq ($(shell sh -c 'uname -s'),Linux)
	LIBFLAGS=-L. -ltcod_debug -ltcodxx_debug -Wl,-rpath=.
else
	LIBFLAGS=-Llib -ltcod -static-libgcc -static-libstdc++ -mwindows
endif

witchR : $(OBJS)
	g++ $(OBJS) -o witchR -Wall $(LIBFLAGS) -g

src/main.hpp.gch : src/*.hpp
	g++ src/main.hpp -Iinclude -Wall

src/%.o : src/%.cpp src/main.hpp.gch
	g++ $< -c -o $@ -Iinclude -Wall -g