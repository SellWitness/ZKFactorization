#ifndef _SIGNATURE_KEY_
#define _SIGNATURE_KEY_

#include "../shared_signature/shared_sig.h"
#include "../timed_commitment/timed_commitment.h"
#include "../sha_commitment/sha_commitment.h"
#include "../protocol.h"
#include "../common/common.h"
#include "../common/log.h"

#include "../bitcoin/cpp/gen-cpp/BitcoinUtils.h"
#include "../bitcoin/cpp/gen-cpp/bitcoin_utils_types.h"
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h> 
#include <protocol/TBinaryProtocol.h>

#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>


namespace sell_information {

std::array<common::TSHA256Digest, common::L> genKeys(const std::vector<byte>& signature);

#include <string>
typedef std::string Transaction;
typedef std::string Address;
typedef std::string Script;
typedef std::string Hash;

class BaseParty {

  private:
  boost::shared_ptr<apache::thrift::transport::TSocket> socket;
  boost::shared_ptr<apache::thrift::transport::TTransport> transport;
  boost::shared_ptr<apache::thrift::protocol::TProtocol> protocol; 

  public:

  BaseParty(int bitcoin_utils_port):
    socket(new apache::thrift::transport::TSocket("localhost", bitcoin_utils_port)),
    transport(new apache::thrift::transport::TBufferedTransport(socket)),
    protocol(new apache::thrift::protocol::TBinaryProtocol(transport)),
    client(protocol)
  {
    transport->open();
  }

  ~BaseParty(){
    transport->close();
  }

  bitcoin_utils::BitcoinUtilsClient client;
};

class SingleSeller {

  const Hash *T2_hash;
	std::array<common::TSHA256Digest, common::L> keys;

	public:
  static BaseParty base_party;
	shared_signature::S shared_signature_s;
	timed_commitment::Commiter timed_commitment_commiter;

	std::array<sha_commitment::Sender, common::L> keys_commits;
	sha_commitment::Sender signature_commit;

	SingleSeller(shared_signature::S shared_signature_s, const Hash *T2_hash):
		T2_hash(T2_hash), shared_signature_s(shared_signature_s), 
		timed_commitment_commiter(), keys_commits() {
  }
	
  const Hash *getT2Hash() {
    return T2_hash;
  }
	
	void cheat(){
		// for testing
		throw ProtocolException("Cheating not implemented!");
	}

	void genKeysAndSetupCommits();
};

class SingleBuyer {

  const Hash *T2_hash;

	bool open_verified;

	public:
  static BaseParty base_party;
	shared_signature::B shared_signature_b;
	timed_commitment::Receiver timed_commitment_receiver;

	std::array<sha_commitment::Receiver, common::L> keys_commits;
	sha_commitment::Receiver signature_commit;

	SingleBuyer(shared_signature::B shared_signature_b, const Hash *T2_hash):
			T2_hash(T2_hash), open_verified(false), shared_signature_b(shared_signature_b), timed_commitment_receiver() {}
	
  const Hash *getT2Hash() {
    return T2_hash;
  }
	
	void setOpenVerified(bool open_verified){
		this->open_verified = open_verified;
	}

	bool getOpenVerified() {
		return open_verified;
	}

	void verifyKeys();
};

class SignatureKeyProtocol: public Protocol<SingleSeller, SingleBuyer> {
	public:
	void init(SingleSeller *, SingleBuyer *);
	void exec(SingleSeller *, SingleBuyer *);
	void open(SingleSeller *, SingleBuyer *);
};

}

#endif
