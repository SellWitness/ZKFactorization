

#include "../sha_commitment/sha_commitment.h"
#include "cut_and_choose.h"

#include <cryptopp/osrng.h>
#include <vector>
#include <iostream>

using namespace std;

const int T = 10;

int main(){

	CryptoPP::AutoSeededRandomPool rng;

	for(int i = -1; i < 10; ++i){

		sha_commitment::Sender sender(&rng);
		sha_commitment::Receiver receiver(&rng);

		vector<sha_commitment::Sender> senders(T, sender);
		vector<sha_commitment::Receiver> receivers(T, receiver);
		cut_and_choose::Prover<sha_commitment::Sender> prover;
		prover.v = senders;
		cut_and_choose::Verifier<sha_commitment::Receiver> verifier;
		verifier.v = receivers;
		verifier.i = 2;

		cut_and_choose::cut_and_choose< sha_commitment::ShaCommitment, 
			sha_commitment::Sender,
			sha_commitment::Receiver,
			T > (&prover, &verifier, i);

		cout << verifier.res << endl;

	}

	return 0;
}
