
FLAGS=-pthread -std=c++11 -Wall -Wextra -Wno-unused

ALL=main

BITCOIN_OBJ := ../bitcoin/cpp/gen-cpp/BitcoinUtils.o ../bitcoin/cpp/gen-cpp/bitcoin_utils_constants.o ../bitcoin/cpp/gen-cpp/bitcoin_utils_types.o
EXTERNAL_OBJ= ../shared_signature/shared_sig.o ../shared_signature/paillier.o ../common/square_root.o ../timed_commitment/regular_commitment.o ../timed_commitment/timed_commitment.o ../timed_commitment/bit_utils.o ../common/common.o ../sha_commitment/sha_commitment.o ../common/aes.o signature_key.o ../common/log.o $(BITCOIN_OBJ)

THRIFT_DIR := /usr/local/include/thrift
BOOST_DIR := /usr/local/include

INC := -I$(THRIFT_DIR) -I$(BOOST_DIR)

all: $(ALL)

$(ALL): %: %.cpp sell_information.o $(EXTERNAL_OBJ)
	g++ $(FLAGS) -o $@ $^ $(INC) -lcryptopp  -L/usr/local/lib -lthrift 

%.o: %.cpp
	g++ -c $(FLAGS) -o $@ $^ $(INC) -lcryptopp
	
clean:
	rm -rf $(ALL) *.o
