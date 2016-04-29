
FLAGS=-std=c++11 -Wall -Wextra -Wno-unused

ALL=test_cc_sha_commit test_cc_shared_signature

all: $(ALL)

$(ALL): %: %.cpp ../sha_commitment/sha_commitment.o ../shared_signature/shared_sig.o ../shared_signature/paillier.o ../common/common.o
	g++ $(FLAGS) -o $@ $^ -lcryptopp

%.o: %.cpp
	g++ -c $(FLAGS) -o $@ $^ -lcryptopp
	
clean:
	rm -rf $(ALL) *.o
