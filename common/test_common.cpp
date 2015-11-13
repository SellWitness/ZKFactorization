#include <cryptopp/osrng.h>
#include "common.h"


CryptoPP::AutoSeededRandomPool& grng(){
	static CryptoPP::AutoSeededRandomPool rng;

	return rng;
}

int main(){
	
	// CryptoPP::AutoSeededRandomPool rng;

	// rng.GenerateBit();
	// grng().GenerateBit();
	common::rng().GenerateBit();
}

