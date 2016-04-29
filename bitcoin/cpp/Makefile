GEN_SRC := gen-cpp/BitcoinUtils.cpp gen-cpp/bitcoin_utils_constants.cpp gen-cpp/bitcoin_utils_types.cpp
GEN_OBJ := $(patsubst %.cpp,%.o, $(GEN_SRC))

THRIFT_DIR := /usr/local/include/thrift
BOOST_DIR := /usr/local/include

INC := -I$(THRIFT_DIR) -I$(BOOST_DIR)

.PHONY: all generate clean 

all: generate test_bitcoin_utils 

generate:
	thrift --gen cpp ../bitcoin_utils.thrift

%.o: %.cpp
	$(CXX) -std=c++11 -Wall -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H $(INC) -c $< -o $@ -lcryptopp

test_bitcoin_utils: test_bitcoin_utils.o $(GEN_OBJ)
	$(CXX) $^ -o $@ -std=c++11 -L/usr/local/lib -lthrift -lcryptopp

bitcoin_utils_server: bitcoin_utils_server.o $(GEN_OBJ)
	$(CXX) $^ -o $@ -std=c++11 -L/usr/local/lib -lthrift -lcryptopp

clean:
	rm -rf *.o gen-cpp/*.o test_bitcoin_utils gen-cpp
