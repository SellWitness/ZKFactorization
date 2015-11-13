#ifndef _SIGNATURE_KEY_
#define _SIGNATURE_KEY_

#include "../bitcoin/bitcoin.h"
#include "../shared_signature/shared_sig.h"
#include "../timed_commitment/timed_commitment.h"
#include "../sha_commitment/sha_commitment.h"
#include "../protocol.h"
#include "../common/common.h"

#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>

namespace sell_information {

class SingleSeller {
	BitcoinTransaction T2;

	std::array<common::TSHA256Digest, common::L> keys;

	public:
	shared_signature::S shared_signature_s;
	timed_commitment::Commiter timed_commitment_commiter;

	std::array<sha_commitment::Sender, common::L> keys_commits;
	sha_commitment::Sender signature_commit;

	SingleSeller() {}

	SingleSeller(shared_signature::S shared_signature_s):
			shared_signature_s(shared_signature_s), timed_commitment_commiter(), keys_commits() {}
	
	void setT2(BitcoinTransaction T2){
		this->T2 = T2;
	}
	
	void cheat(){
		// for testing
		throw ProtocolException("Cheating not implemented!");
	}

	void genKeysAndSetupCommits();
};

class SingleBuyer {
	
	BitcoinTransaction T1;
	BitcoinTransaction T2;

	bool open_verified;

	public:
	shared_signature::B shared_signature_b;
	timed_commitment::Receiver timed_commitment_receiver;

	std::array<sha_commitment::Receiver, common::L> keys_commits;
	sha_commitment::Receiver signature_commit;

	SingleBuyer() {}

	SingleBuyer(shared_signature::B shared_signature_b):
			open_verified(false), shared_signature_b(shared_signature_b), timed_commitment_receiver() {}
	
	void createT1(){
		/* TODO */
	}

	void createT2(){
		/* TODO */
	}
	
	BitcoinTransaction getT2(){
		return T2;
	}

	void setOpenVerified(bool open_verified){
		this->open_verified = open_verified;
	}

	bool getOpenVerified() {
		return open_verified;
	}

	void verifyKeys();
};

class SingleSellInformationProtocol : public Protocol<SingleSeller, SingleBuyer> {
	public:
	void init(SingleSeller *, SingleBuyer *);
	void exec(SingleSeller *, SingleBuyer *);
	void open(SingleSeller *, SingleBuyer *);
};

}

#endif
