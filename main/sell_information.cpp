#include "sell_information.h"
#include "../cut_and_choose/cut_and_choose.h"
#include "../timed_commitment/bit_utils.h"
#include "../timed_commitment/timed_commitment.h"
#include "../sha_commitment/sha_commitment.h"
#include "../common/aes.h"
#include "../common/log.h"
#include "../common/common.h"
#include <vector>
#include <set>
#include <iostream>
#include <numeric>
#include <thread>
#include <unistd.h>
#include <chrono>

using namespace CryptoPP;
using namespace std;
using namespace std::chrono;

namespace sell_information {

using common::L;
using common::T;
using common::R;
using common::integer2string;

void Seller::findRoots(unsigned j) {
	for(unsigned i = 0; i < common::L; ++i){
		array<Integer, 4> roots = square_root.all_square_roots(y[j][i]);
		r1[j][i] = roots[0];
		r2[j][i] = roots[1];
		if (common::rng().GenerateBit() == 1) {
			swap(r1[j][i], r2[j][i]);
		}
		if (r1[j][i] == -1) {
			// TODO random
		}
		if (r2[j][i] == -1) {
			// TODO random
		}
	}
}

void Seller::encryptRoots(unsigned j) {
	for(unsigned i = 0; i < L; ++i) {
		c1[j][i] = common::enc(single_sellers[j].keys_commits[i].m, common::to_bytes(r1[j][i]));
		c2[j][i] = common::enc(single_sellers[j].keys_commits[i].m, common::to_bytes(r2[j][i]));
	}
}

void Seller::setupCommit(unsigned j) {
	for(unsigned i = 0; i < L; ++i) {
		d1[j][i].m = c1[j][i];
		d2[j][i].m = c2[j][i];
	}
}

array<unsigned, L/2> Seller::acceptSubset(
													unsigned j,
													const std::array<unsigned, common::L/2>& indices, 
													const std::array<CryptoPP::Integer, common::L/2>& values) {
	array<unsigned, L/2> commitment_indices;	// which of c1, c2 shoudl be opened ?

	for(unsigned i = 0; i < L/2; ++i) {
		unsigned ind = indices[i];
		Integer val = values[i];
		if (val > n/2) {
      Log("val:");
      Log(val);
      Log("n/2");
      Log(n/2);
			throw ProtocolException("xi > n/2");
		}
		if ((val*val) % n != y[j][ind]) {
			throw ProtocolException("xi is not a square root of yi");
		}
		commitment_indices[i] = 5; // TODO
		if (val == r1[j][ind]) {
			commitment_indices[i] = 1;
		}
		if (val == r2[j][ind]) {
			commitment_indices[i] = 2;
		}
	}
	return commitment_indices;
}

void Seller::accept_T1(const Transaction& T1) {
  Log("accept t1");
	this->T1 = T1;

	vector<string> xs, ys;
	for (unsigned i = 0; i < R; ++i) {
  	xs.push_back(common::integer2string(single_sellers[i].shared_signature_s.get_Q().x));
	  ys.push_back(common::integer2string(single_sellers[i].shared_signature_s.get_Q().y));
	}

  bool res = SingleSeller::base_party.client.verifyTransaction(T1, T1_output_script, xs, ys, price);

  if (!res) {
    throw ProtocolException("verify T1 failed!");
  } else {
    Log("verify transaction worked!");
  }
  
  /* TODO verify T2 */
  res = SingleSeller::base_party.client.waitForTransaction(T1, 6); // TODO parametrise
  if (!res) {
    throw ProtocolException("wait for t1 failed!");
  } else {
    Log("wait for t1 worked!");
  }
}

// TODO change name
void Seller::verifyT2Signature() {
  Transaction res;
	vector<string> xs, ys, rs, ss;
	for (unsigned i = 0; i < R; ++i) {
    xs.push_back(integer2string(single_sellers[i].shared_signature_s.get_Q().x));
    ys.push_back(integer2string(single_sellers[i].shared_signature_s.get_Q().y));
    rs.push_back(integer2string(single_sellers[i].shared_signature_s.get_r()));
    ss.push_back(integer2string(single_sellers[i].shared_signature_s.get_s()));
	}
  SingleSeller::base_party.client.getSignedTransaction(res, T2, xs, ys, rs, ss, T1_output_script, original_T1_output_script);

  if (res.size() == 0) {
    throw ProtocolException("verify T2 signature in bitcoinj failed");
  }
  T2 = res;
}

void Seller::accept_payment() {
  if (cheat){
    return ;
  }
  bool res = SingleSeller::base_party.client.broadcastTransaction(T2);
   
  if (!res) {
    throw ProtocolException("accept payment failed!");
  } else {
    Log("accept payment worked!");
  }
}

void Buyer::createT1AndT2() {
	vector<string> xs, ys;
	for (unsigned i = 0; i < R; ++i) {
    xs.push_back(integer2string(single_buyers[i].shared_signature_b.get_Q().x));
    ys.push_back(integer2string(single_buyers[i].shared_signature_b.get_Q().y));
	}

  bitcoin_utils::TransactionAndScripts out;
  SingleBuyer::base_party.client.createSend(out, xs, ys, price);
	T1 = out.tx;
	original_T1_output_script = out.original_script;
	T1_output_script = out.script;
  
  if (T1.size() == 0) {
    throw ProtocolException("T1 is empty");
  }

  SingleBuyer::base_party.client.createSendTransactionToAddress(T2, T1, xs, ys, seller_address);
//  SingleBuyer::base_party.client.getOutputScript(T1_output_script, T1, xs, ys);
  SingleBuyer::base_party.client.hashForSignature(T2_hash, T2, xs, ys, original_T1_output_script);
}

void Buyer::pickR(){
	set<int> chosen;
	while (chosen.size() < R) {
		chosen.insert(Integer(common::rng(), 0, T-1).ConvertToLong());
	}
	r.assign(chosen.begin(), chosen.end());
}

void Buyer::pickSubset(unsigned j) {
	indices[j].resize(L);
	std::iota(indices[j].begin(), indices[j].end(), 0);
	common::rng().Shuffle(indices[j].begin(), indices[j].end());
	indices[j].resize(L/2);
}

array<unsigned, L/2> Buyer::getSubsetIndices(unsigned j) {
	array<unsigned, L/2> res;
	copy(indices[j].begin(), indices[j].end(), res.begin());
	return res;
}

array<Integer, L/2> Buyer::getSubsetValues(unsigned j) {
	array<Integer, L/2> res;
	for(unsigned i = 0; i < L/2; ++i) {
		res[i] = x[j][indices[j][i]];
	}
	return res;
}

void Buyer::verifyRoot(unsigned j, unsigned ind, const vector<byte>& key, const vector<byte>& c) {
		vector<byte> x_encoded = common::dec(key, c);
		Integer x(x_encoded.data(), x_encoded.size());
		if (x != this->x[j][ind]) {
			throw ProtocolException("verifyRoot failed");
		}
}

void Buyer::make_payment() {
  Log("make payment");
  bool res = SingleBuyer::base_party.client.broadcastTransaction(
			T1
  );
  if (!res) {
    throw ProtocolException("make payment failed");
  } else {
    Log("payment succesfull");
  }
}

void Buyer::solve_time_lock(atomic_bool &finished) {
	bool res = false;

	for (unsigned j = 0; j < R && !res; ++j) {
  	Log("solve time lock started");
		high_resolution_clock::time_point start_time = high_resolution_clock::now();

		auto ds_bytes = this->single_buyers[j].timed_commitment_receiver.force_open();

		high_resolution_clock::time_point end_time = high_resolution_clock::now();
		duration<double> time_span = duration_cast<duration<double>>(end_time - start_time);
	  Log("solve time lock finished");
  	LogTime(time_span);
  
	  BaseParty base_party(9092);

  	Integer ds = BitUtils::bits_to_integer(ds_bytes);
	  Integer d = ds * this->single_buyers[j].shared_signature_b.get_db();
  	d = d % single_buyers[j].shared_signature_b.get_n();

	  res = base_party.client.redeemTransaction(
			T1,
			original_T1_output_script,
    	common::integer2string(d),
	    common::integer2string(single_buyers[j].shared_signature_b.get_Q().x),
  	  common::integer2string(single_buyers[j].shared_signature_b.get_Q().y),
			R
  	);
	}

  if (!res) {
    // TODO what should happen at this point?
    Log("solve time lock - redeem transaction failed");
    Log("solve time sleeping forever!!!");
    while(true) {
      sleep(1);
    }
  }

  Log("Hurray!");
  Log("solved time lock and got money back");

	finished = true;	
}

void Buyer::get_signature(atomic_bool &finished) {
  Log("get signature");
  bool res;
  BaseParty base_party(9092);
  res = base_party.client.waitForTransactionByOutput(seller_address, 6); // TODO parametrise
  if (!res) {
    throw ProtocolException("wait for transaction by output failed");
  }

	high_resolution_clock::time_point start_time = high_resolution_clock::now();

  vector<bitcoin_utils::IntegerPair> signatures;
  SingleBuyer::base_party.client.getSignatures(signatures, seller_address, R);

  Log("-------------------------------------------");
  Log("Signature(s) from btc");
	for (unsigned i = 0; i < R; ++i) {
  	Integer r = Integer((byte*) signatures[i].r.data(), signatures[i].r.size());
  	Integer s = Integer((byte*) signatures[i].s.data(), signatures[i].s.size());
  	this->signatures[i] = shared_signature::encode_signature(r, s);

		if (i > 0) {
			Log("");
		}
  	Log(common::string2hex(common::v2string(this->signatures[i])));
  	Log("-- r s");
  	Log(r);
  	Log(s);
	}
  Log("-------------------------------------------");
	
	factorise();

	high_resolution_clock::time_point end_time = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(end_time - start_time);
  Log("factorising using the signature finished");
  LogTime(time_span);
	finished = true;
}

void Buyer::wait_for_signature_or_time_lock() {
	atomic_bool signature_finished(false);
	atomic_bool time_lock_finished(false);

	thread solve_time_lock(&Buyer::solve_time_lock, this, ref(time_lock_finished));
	thread get_signature(&Buyer::get_signature, this, ref(signature_finished));
	
	Log("wait_for_signature_or_time_lock waiting");
	while(!signature_finished && !time_lock_finished) {
		sleep(1);
	}
	Log("wait_for_signature_or_time_lock finished");

	if (signature_finished) {
		get_signature.join();
		solve_time_lock.detach();
		Log("signature finished");
	} else {
		Log("time lock finished");
		get_signature.detach();
		solve_time_lock.join();
	}
}

void Buyer::factorise() {
	array<array<common::TSHA256Digest, L>, R> keys;
	for (unsigned i = 0; i < R; ++i) {
  	vector<byte> sig(signatures[i].begin(), signatures[i].end());
		keys[i] = genKeys(sig);
	}
	// TODO negate signature 
	for(unsigned j = 0; j < R; ++j) {
		for(unsigned i = 0; i < L; ++i) {
			if (d1[j][i].m.empty() || d2[j][i].m.empty())
				continue;

			auto r1_bytes = common::dec(keys[j][i], d1[j][i].m);
			auto r2_bytes = common::dec(keys[j][i], d2[j][i].m);
			Integer r1(r1_bytes.data(), r1_bytes.size());
			Integer r2(r2_bytes.data(), r2_bytes.size());
			if (r1 < 0)
				r1 = -r1;
			if (r2 < 0)
				r2 = -r2;
			Integer p = GCD(n + r1 - r2, n);
			Integer q = GCD(r1 + r2, n);;
			if (p * q == n) {
				this->p = p;
				this->q = q;
				return;
			}
		}
	}
	throw ProtocolException("Factorization failed!");
}

void SellInformationProtocol::init(Seller *seller, Buyer *buyer) {
	if (seller->get_n() != buyer->get_n()){
		throw ProtocolException("Not matching n");
	}

	if (seller->get_price() != buyer->get_price()){
		throw ProtocolException("Not matching price");
	}

	/*
  if (!verify(seller->get_address())){
		throw ProtocolException("Seller has invalid bitcoin address");
	}
  */

  /*
	if (!verify(buyer->get_address())){
		throw ProtocolException("Buyer has invalid bitcoin address");
	}
  */
}

void SellInformationProtocol::exec(Seller *seller, Buyer *buyer){
	Log("sell information protocol exec");
	high_resolution_clock::time_point start_time = high_resolution_clock::now();
  
	SignatureKeyProtocol single_sell_information;

	shared_signature::S shared_signature_s;
	shared_signature::B shared_signature_b(
		shared_signature_s.get_paillier_n(),
		shared_signature_s.get_paillier_g()
	);

	SingleSeller single_seller(shared_signature_s, buyer->getT2Hash());
	SingleBuyer single_buyer(shared_signature_b, buyer->getT2Hash());

	vector<SingleSeller> single_sellers(T, single_seller);
	vector<SingleBuyer> single_buyers(T, single_buyer);

	cut_and_choose::Prover<SingleSeller> prover;
	prover.v = single_sellers;
	cut_and_choose::Verifier<SingleBuyer> verifier;
	verifier.v = single_buyers;

  shared_signature::SharedSignature shared_signature_protocol;
  /* Generate shared secret key for ECDSA */
	for (unsigned i = 0; i < T; ++i) {
  	shared_signature_protocol.init(&prover.v[i].shared_signature_s, &verifier.v[i].shared_signature_b);
	}

	buyer->pickR();
	verifier.i = buyer->getR();

	for (unsigned i = 0; i < R; ++i) {
		unsigned ix = verifier.i[i];
		buyer->addSingleBuyer(verifier.v[ix]);
	}

	buyer->setSellerAddress(seller->getAddress());
	buyer->createT1AndT2();

	cut_and_choose::cut_and_choose<SignatureKeyProtocol, 
		SingleSeller,
		SingleBuyer,
		T,
	  R	> (&prover, &verifier);

	if (!verifier.res) {
		throw ProtocolException("Generating signatures and keys failed!");
	}

	buyer->clearSingleBuyers();
	for (unsigned i = 0; i < R; ++i) {
		unsigned ix = verifier.i[i];
		seller->addSingleSeller(prover.v[ix]);
		buyer->addSingleBuyer(verifier.v[ix]);
	}

	seller->setT1OutputScript(buyer->getT1OutputScript());
	seller->setOriginalT1OutputScript(buyer->getOriginalT1OutputScript());
	seller->setT2(buyer->getT2());
	seller->verifyT2Signature();

	high_resolution_clock::time_point end_time = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(end_time - start_time);
	start_time = high_resolution_clock::now();
	Log("finished first Cut and Choose (generating shared keys and signatures)");
  LogTime(time_span);

  Log("-------------------------------------------");
  Log("Generated signature(s):");
	for (unsigned i = 0; i < R; ++i) {
		if (i > 0) {
			Log("");
		}
	  Log(
  	  common::string2hex(
    	  common::v2string(
      	  seller->single_sellers[i].shared_signature_s.get_signature()
    	  )
  	  )
	  );
  	Log(seller->single_sellers[i].shared_signature_s.get_r());
	  Log(seller->single_sellers[i].shared_signature_s.get_s());
	}
  Log("-------------------------------------------");

	for (unsigned j = 0; j < R; ++j) {
		buyer->genSquares(j);
		seller->acceptSquares(j, buyer->getSquares(j));
		seller->findRoots(j);
		seller->encryptRoots(j);
		seller->setupCommit(j);

		sha_commitment::ShaCommitment sha_commitment_protocol;

		for(unsigned i = 0; i < L; ++i) {
			sha_commitment_protocol.init(&seller->d1[j][i], &buyer->d1[j][i]);
			sha_commitment_protocol.init(&seller->d2[j][i], &buyer->d2[j][i]);

			sha_commitment_protocol.exec(&seller->d1[j][i], &buyer->d1[j][i]);
			sha_commitment_protocol.exec(&seller->d2[j][i], &buyer->d2[j][i]);
		}
	
		buyer->pickSubset(j);
		auto indices = buyer->getSubsetIndices(j);
		auto values = buyer->getSubsetValues(j);

		set<unsigned> indices_set(indices.begin(), indices.end());
		if (indices_set.size() != L/2) {
			Log(indices);
			throw ProtocolException("invalid indices size");
		}

		for(unsigned ind: indices_set) {
			if (ind >= L) {
				throw ProtocolException("invalid commitment_indices value");
			}
		}

		auto commitment_indices = seller->acceptSubset(j, indices, values);

		for(unsigned i = 0; i < L/2; ++i) {
			unsigned ind = indices[i];
			Integer val = values[i];
			unsigned commit_ind = commitment_indices[i];
			sha_commitment_protocol.open(&seller->single_sellers[j].keys_commits[ind], &buyer->single_buyers[j].keys_commits[ind]);
			vector<byte> key = buyer->single_buyers[j].keys_commits[ind].m;
			vector<byte> c;
			switch (commit_ind) {
				case 1:
					sha_commitment_protocol.open(&seller->d1[j][ind], &buyer->d1[j][ind]);
					c = buyer->d1[j][ind].m;
					break;
				case 2:
					sha_commitment_protocol.open(&seller->d2[j][ind], &buyer->d2[j][ind]);
					c = buyer->d2[j][ind].m;
					break;
				default:
					throw ProtocolException("matching roots to values didn't work");
			}
			buyer->verifyRoot(j, ind, key, c);
		}

		for(unsigned ind = 0; ind < L; ++ind){
			if (indices_set.find(ind) == indices_set.end()) {
				sha_commitment_protocol.open(&seller->d1[j][ind], &buyer->d1[j][ind]);
				sha_commitment_protocol.open(&seller->d2[j][ind], &buyer->d2[j][ind]);
			}
		}
	}

	end_time = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(end_time - start_time);
	Log("finished second Cut and Choose (generating squares and responses)");
  LogTime(time_span);
  Log("off blockchain part finished");

	buyer->make_payment();

	seller->accept_T1(buyer->getT1());

	seller->accept_payment();	 // last function for seller
 
	buyer->wait_for_signature_or_time_lock(); // last function for buyer
}

void SellInformationProtocol::open(Seller *seller, Buyer *buyer){

}

}
