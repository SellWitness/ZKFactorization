#include "sha_commitment.h"

#include "../common/common.h"

#include <cryptopp/integer.h>
#include <cryptopp/sha.h>

#include <iostream>

using namespace CryptoPP;
using namespace std;

namespace sha_commitment {

static void compute_sha(byte* dst, const vector<byte>& msg, byte* seed){
	
	vector<byte> vseed(seed, seed + N);
	vector<byte> tmp = common::concatenate<byte>(msg, vseed);
  common::sha256().CalculateDigest(dst, tmp.data(), tmp.size());
}

void ShaCommitment::init(Sender *, Receiver *) {
	
}

void ShaCommitment::exec(Sender *s, Receiver *r) {

	common::rng().GenerateBlock(s->seed, N);
	
	compute_sha(s->com.data(), s->m, s->seed);

	copy(s->com.data(), s->com.data() + COM_SIZE, r->com.data());
}

void ShaCommitment::open(Sender *s, Receiver *r) {
	r->m = s->m;
	copy(s->seed, s->seed + N, r->seed);
	
	TCommitment com;
	compute_sha(com.data(), r->m, r->seed);
	r->setOpenVerified(equal(r->com.data(), r->com.data() + COM_SIZE, com.data()));
	if (!r->getOpenVerified()) {
		throw ProtocolException("sha commitment open failed");
	}
}

}
