
FLAGS=-std=c++11 -Wall -Wextra -Wno-unused

ALL=test_timed_commitment test_regular_commit test_bit_utils
#test_safe_prime

all: $(ALL)

$(ALL): %: %.cpp timed_commitment.o regular_commitment.o bit_utils.o ../common/common.o
	g++ $(FLAGS) -o $@ $^ -lcryptopp

%.o: %.cpp
	g++ -c $(FLAGS) -o $@ $^ -lcryptopp
	
clean:
	rm -rf $(ALL) *.o
