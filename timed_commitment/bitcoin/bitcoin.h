#ifndef _BITCOIN_
#define _BITCOIN_

#include <vector>



class BitcoinAddress {
	
};

class BitcoinTransaction {
	typedef unsigned char byte;
	public:

	std::vector<byte> get_bytes(){
		std::vector<byte> res = { 0, 1, 2, 3, 4, 5, 6, 7 };
		return res;
	}
};

bool verify(BitcoinAddress address);

#endif 
