#include "aes.h"
#include <iostream>

using namespace CryptoPP;
using namespace std;

int main() {
	
	AutoSeededRandomPool rnd;

	byte key[AES_LENGTH];
	array<byte, AES_LENGTH> akey;
	vector<byte> vkey(AES_LENGTH);

	rnd.GenerateBlock(key, sizeof(key));
	rnd.GenerateBlock(akey.data(), akey.size());
	rnd.GenerateBlock(vkey.data(), vkey.size());

	for(int s = 0; s < 100000; s += 100){
		vector<byte> msg(s);
		rnd.GenerateBlock(msg.data(), msg.size());

		auto cipher = enc(key, msg);
		auto recovered = dec(key, cipher);

		if (recovered != msg) {
			cerr << "ERR key" << endl;
			return;
		}	
	}

	for(int s = 0; s < 100000; s += 100){
		vector<byte> msg(s);
		rnd.GenerateBlock(msg.data(), msg.size());

		auto cipher = enc(akey, msg);
		auto recovered = dec(akey, cipher);

		if (recovered != msg) {
			cerr << "ERR akey" << endl;
			return;
		}	
	}

	for(int s = 0; s < 100000; s += 100){
		vector<byte> msg(s);
		rnd.GenerateBlock(msg.data(), msg.size());

		auto cipher = enc(vkey, msg);
		auto recovered = dec(vkey, cipher);

		if (recovered != msg) {
			cerr << "ERR vkey" << endl;
			return;
		}	
	}


}
