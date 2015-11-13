#include "signature_key.h"	
#include "../timed_commitment/bit_utils.h"

using namespace CryptoPP;
using namespace std;

using common::L;
using common::K;

namespace sell_information {

static array<common::TSHA256Digest, L> genKeys(const vector<byte>& signature) {
	array<common::TSHA256Digest, L> keys;
	for(unsigned i = 0; i < L; ++i) {
		Integer I(i);
		vector<byte> i_encoded(I.MinEncodedSize());
		I.Encode(i_encoded.data(), i_encoded.size());

		vector<byte> tmp = common::concatenate(signature, i_encoded);
		common::sha256().CalculateDigest(keys[i].data(), tmp.data(), tmp.size());

		// keys_commits[i].m.assign(keys[i].begin(), keys[i].end()); // TODO
	}
	// signature_commit.m = signature; TODO
	return keys;
}

void SingleSeller::genKeysAndSetupCommits(){
	vector<byte> signature = shared_signature_s.get_signature();

	keys = genKeys(signature);
	for(unsigned i = 0; i < L; ++i) {
		keys_commits[i].m.assign(keys[i].begin(), keys[i].end());
	}
	signature_commit.m = signature;
}

void SingleBuyer::verifyKeys() {
	vector<byte> signature = signature_commit.m;
	array<common::TSHA256Digest, common::L> exp_keys = genKeys(signature);
	for(unsigned i = 0; i < L; ++i){
		if (!equal(keys_commits[i].m.begin(), keys_commits[i].m.end(), exp_keys[i].begin())) {
			throw ProtocolException("Invalid key");
		}
	}
}


void SingleSellInformationProtocol::init(SingleSeller *, SingleBuyer *) {
	
}

void SingleSellInformationProtocol::exec(SingleSeller *seller, SingleBuyer *buyer) {
	shared_signature::SharedSignature shared_signature_protocol;

	/* Generate shared secret key for ECDSA */
	shared_signature_protocol.init(&seller->shared_signature_s, &buyer->shared_signature_b);

	buyer->createT1();

	buyer->createT2();

	seller->setT2(buyer->getT2());	// TODO verify T2

	auto T2_bytes = buyer->getT2().get_bytes();

	/* Sign T2 */
	buyer->shared_signature_b.set_data(T2_bytes.data(), T2_bytes.size());

	shared_signature_protocol.exec(&seller->shared_signature_s, &buyer->shared_signature_b);

	timed_commitment::commit(&seller->timed_commitment_commiter, &buyer->timed_commitment_receiver, K, BitUtils::integer_to_bits(seller->shared_signature_s.get_ds()));

	seller->genKeysAndSetupCommits();

	sha_commitment::ShaCommitment sha_commitment_protocol;
	
	for(unsigned i = 0; i < L; ++i) {
		sha_commitment_protocol.init(&seller->keys_commits[i], &buyer->keys_commits[i]);
		sha_commitment_protocol.exec(&seller->keys_commits[i], &buyer->keys_commits[i]);
	}

	sha_commitment_protocol.init(&seller->signature_commit, &buyer->signature_commit);
	sha_commitment_protocol.exec(&seller->signature_commit, &buyer->signature_commit);
}

void SingleSellInformationProtocol::open(SingleSeller *single_seller, SingleBuyer *single_buyer) {
	/* verify 
	 * - init
	 * - signing
	 * TODO
	 */

	shared_signature::SharedSignature shared_signature_protocol;
	shared_signature_protocol.open(&single_seller->shared_signature_s, &single_buyer->shared_signature_b);

	timed_commitment::open_commitment(&single_seller->timed_commitment_commiter, &single_buyer->timed_commitment_receiver);

	sha_commitment::ShaCommitment sha_commitment_protocol;

	for(unsigned i = 0; i < L; ++i) {
		sha_commitment_protocol.open(&single_seller->keys_commits[i], &single_buyer->keys_commits[i]);
	}

	sha_commitment_protocol.open(&single_seller->signature_commit, &single_buyer->signature_commit);

	single_buyer->verifyKeys();

	single_buyer->setOpenVerified(single_buyer->shared_signature_b.getOpenVerified());
}

}
