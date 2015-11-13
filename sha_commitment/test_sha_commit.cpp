
#include "sha_commitment.h"
#include "../common/common.h"

#include <iostream>
#include <cryptopp/osrng.h>

using namespace std;
using namespace CryptoPP;
using namespace sha_commitment;

const int T = 100000;

int main(){

	ShaCommitment shaCommitment;

	// test valid
	for(int i = 0; i < T; ++i){
		Sender s;
		Receiver r;
		
		vector<byte> x(200);
		common::rng().GenerateBlock(x.data(), x.size());
		s.m = x;

		shaCommitment.init(&s, &r);
		shaCommitment.exec(&s, &r);

		if (s.com != r.com) {
			cerr << "ERR com not equal" << endl;
			return 1;
		}

		shaCommitment.open(&s, &r);
		if (s.m != r.m){
			cerr << "ERR s.m != r.m" << endl;
			return 1;
		}	
		if (r.getOpenVerified() == false){
			cerr << "ERR r.valid == false" << endl;
			return 1;
		}
		if (!equal(s.seed, s.seed+N, r.seed)){
			cerr << "ERR seed" << endl;
			return 1;
		}
	}

	for(int i = 0; i < T; ++i){
		Sender s;
		Receiver r;

		vector<byte> x(200);
		common::rng().GenerateBlock(x.data(), x.size());
		s.m = x;

		shaCommitment.init(&s, &r);
		shaCommitment.exec(&s, &r);

		if (s.com != r.com){
			cerr << "ERR com not equal" << endl;
			return 1;
		}

		// change m
		(*s.m.begin()) = !(*s.m.begin());
		bool ok = false;
		try {
			shaCommitment.open(&s, &r);
		} catch (ProtocolException &e) {
			ok = true;
		}
		if (!ok) {
			cerr << "didn't throw!" << endl;
			return 1;
		}
		if (s.m != r.m){
			cerr << "ERR s.m != r.m" << endl;
			return 1;
		}	
		if (r.getOpenVerified() == true){
			cerr << "ERR r.valid == true" << endl;
			return 1;
		}
		if (!equal(s.seed, s.seed+N, r.seed)){
			cerr << "ERR seed" << endl;
			return 1;
		}
	}

	for(int i = 0; i < T; ++i){
	
		Sender s;
		Receiver r;

		vector<byte> x(200);
		common::rng().GenerateBlock(x.data(), x.size());
		s.m = x;

		shaCommitment.init(&s, &r);
		shaCommitment.exec(&s, &r);

		if (s.com != r.com){
			cerr << "ERR com not equal" << endl;
			return 1;
		}

		// change seed
		++s.seed[1];
		bool ok = false;
		try {
			shaCommitment.open(&s, &r);
		} catch (ProtocolException &e) {
			ok = true;
		}
		if (!ok) {
			cerr << "ERR didn't throw!" << endl;
			return 1;
		}
		if (s.m != r.m){
			cerr << "ERR s.m != r.m" << endl;
			return 1;
		}	
		if (r.getOpenVerified() == true){
			cerr << "ERR r.valid == true" << endl;
			return 1;
		}
		if (!equal(s.seed, s.seed+N, r.seed)){
			cerr << "ERR seed" << endl;
			return 1;
		}
	}

	cout << "OK" << endl;

	return 0;
}
