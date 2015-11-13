

#include "../shared_signature/shared_sig.h"
#include "cut_and_choose.h"

#include <cryptopp/osrng.h>
#include <cryptopp/ecp.h>
#include <cryptopp/oids.h>
#include <cryptopp/eccrypto.h>

#include <vector>
#include <iostream>

const int MESSAGE_LENGTH = 10;
const int T = 10;

using namespace std;

int main(){
	CryptoPP::AutoSeededRandomPool rng;

	CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP> ec_parameters;
	ec_parameters.Initialize(CryptoPP::ASN1::secp256k1());

	for(int i = -1; i < 10; ++i){

		shared_signature::S s(&rng, ec_parameters);
		shared_signature::B b(&rng, ec_parameters, s.get_paillier_n(), s.get_paillier_g());

		byte message[MESSAGE_LENGTH];

		rng.GenerateBlock(message, MESSAGE_LENGTH);

		b.set_data(message, MESSAGE_LENGTH);

		shared_signature::SharedSignature shared_signature;

		vector<shared_signature::S> senders(T, s);
		vector<shared_signature::B> receivers(T, b);

		cut_and_choose::Prover<shared_signature::S> prover;
		prover.v = senders;
		cut_and_choose::Verifier<shared_signature::B> verifier;
		verifier.v = receivers;
		verifier.i = 3;

		shared_signature.init(&s, &b);
		if (!(s.get_Q() == b.get_Q())){
			cerr << "Q err" << endl;
			return 1;
		}

		cut_and_choose::cut_and_choose<
			shared_signature::SharedSignature, 
			shared_signature::S,
			shared_signature::B,
			T > (&prover, &verifier, i);

		cout << verifier.res << endl;
	}
}
