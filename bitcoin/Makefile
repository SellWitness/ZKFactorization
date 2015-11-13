
FLAGS=-std=c++11 -Wall -Wextra -Wno-unused

ALL=bitcoin.o

all: $(ALL)

#$(ALL): %: %.cpp
#	g++ $(FLAGS) -o $@ $^ -lcryptopp

%.o: %.cpp
	g++ -c $(FLAGS) -o $@ $^ -lcryptopp
	
clean:
	rm -rf $(ALL) *.o
