
FLAGS=-std=c++11 -Wall -Wextra

ALL=test_paillier test_shared_sig  ECDSA-Test-C ecdsa

all: $(ALL)

$(ALL): %: %.cpp paillier.o shared_sig.o ../common/common.o
	g++ $(FLAGS) -o $@ $^ -lcryptopp

#ECDSA-Test-C: ECDSA-Test-C.cpp
#	g++ $(FLAGS) -o $@ $^ -lcryptopp

#test_paillier: test_paillier.cpp paillier.o
#	g++ $(FLAGS) -o $@ $^ -lcryptopp

#shared_sig: shared_sig.cpp paillier.o
#	g++ $(FLAGS) -o $@ $^ -lcryptopp

%.o: %.cpp
	g++ -c $(FLAGS) -o $@ $^ -lcryptopp
	
# g++ -g3 -ggdb -O0 -Wall -Wextra -Wno-unused -o $@ $< -lcryptopp

clean:
	rm -rf $(ALL) *.o
