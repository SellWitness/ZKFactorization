#ifndef _SHA_COMMITMENT_H_
#define _SHA_COMMITMENT_H_

#include "../protocol.h"

#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <array>
#include <vector>

namespace sha_commitment {

const unsigned N = 32; // security parameter in bytes

const unsigned COM_SIZE = 256/8;

typedef std::array<byte, COM_SIZE> TCommitment;

class Sender {

	public:

	std::vector<byte> m;
	byte seed[N];

	TCommitment com;

	void cheat(){
		*m.begin()= !(*m.begin());
	}
};

class Receiver {

	bool open_verified;

	public:

	std::vector<byte> m;
	byte seed[N];
	TCommitment com;

	void setOpenVerified(bool val){
		open_verified = val;
	}

	bool getOpenVerified(){
		return open_verified;
	}
};

class ShaCommitment : public Protocol<Sender, Receiver> {
	public:
	void init(Sender *, Receiver *);
	void exec(Sender *, Receiver *);
	void open(Sender *, Receiver *);
};

}

#endif

