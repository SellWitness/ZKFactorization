
FLAGS=-std=c++11 -Wall -Wextra -Wno-unused

ALL=main

EXTERNAL_OBJ= ../bitcoin/bitcoin.o ../shared_signature/shared_sig.o ../shared_signature/paillier.o ../common/square_root.o ../timed_commitment/regular_commitment.o ../timed_commitment/timed_commitment.o ../timed_commitment/bit_utils.o ../common/common.o ../sha_commitment/sha_commitment.o ../common/aes.o signature_key.o ../common/log.o

all: $(ALL)

$(ALL): %: %.cpp sell_information.o $(EXTERNAL_OBJ)
	g++ $(FLAGS) -o $@ $^ -lcryptopp

%.o: %.cpp
	g++ -c $(FLAGS) -o $@ $^ -lcryptopp
	
clean:
	rm -rf $(ALL) *.o
