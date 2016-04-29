#ifndef _SELL_INFORMATION_
#define _SELL_INFORMATION_

#include "signature_key.h"

#include "../shared_signature/shared_sig.h"
#include "../common/common.h"
#include "../common/aes.h"
#include "../protocol.h"
#include "../common/square_root.h"

#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>

#include <atomic>
#include <vector>

namespace sell_information {

class Seller {
	// p*q = n, Buyer wants to buy p, q
	CryptoPP::Integer p;	
	CryptoPP::Integer q;
	CryptoPP::Integer n;

	unsigned price;
  bool cheat;

	common::SquareRoot square_root;

	std::array<std::array<CryptoPP::Integer, common::L>, common::R> y;
	std::array<std::array<CryptoPP::Integer, common::L>, common::R> r1;
	std::array<std::array<CryptoPP::Integer, common::L>, common::R> r2;

	std::array<std::array<std::vector<byte>, common::L>, common::R> c1;
	std::array<std::array<std::vector<byte>, common::L>, common::R> c2;

	Address address;
	Transaction T1;
	Script T1_output_script;
	Script original_T1_output_script;
	Transaction T2;
	Hash T2_hash;
	public:

	std::vector<SingleSeller> single_sellers;

	std::array<std::array<sha_commitment::Sender, common::L>, common::R> d1;
	std::array<std::array<sha_commitment::Sender, common::L>, common::R> d2;

	Seller(CryptoPP::Integer p, 
				CryptoPP::Integer q, 
				unsigned price,
        bool cheat):
		p(p), q(q), n(p*q), price(price), cheat(cheat), square_root(p, q) {
		  SingleSeller::base_party.client.freshAddress(address);
		}

	CryptoPP::Integer get_n(){
		return p*q;
	}

	unsigned get_price(){
		return price;
	}

	Address getAddress() {
		return address;
	}

	void setAddress(Address address) {
		this->address = address;
	}

  void setT1OutputScript(Script script) {
	  this->T1_output_script = script;
	}

  void setOriginalT1OutputScript(Script script) {
	  this->original_T1_output_script = script;
	}

  void setT2(Transaction T2){
    this->T2 = T2;
  }
	
	void acceptSquares(unsigned j, const std::array<CryptoPP::Integer, common::L>& y){
		this->y[j] = y;
	}

	void findRoots(unsigned);
	
	void encryptRoots(unsigned);

	void addSingleSeller(const SingleSeller& single_seller){
		this->single_sellers.push_back(single_seller);
	}	

	void verifyT2Signature();

	void setupCommit(unsigned);

	std::array<unsigned, common::L/2> acceptSubset(
		unsigned,
		const std::array<unsigned, common::L/2>& indices,
		const std::array<CryptoPP::Integer, common::L/2>& values);

	void accept_T1(const Transaction& T1);
	void accept_payment();
};

class Buyer {
	CryptoPP::Integer n;	// The rsa modulus - would like to buy primes p, q: p*q = n

	Address seller_address;
	Transaction T1;
	Script T1_output_script;
	Script original_T1_output_script;
	Transaction T2;
	Hash T2_hash;
	unsigned price;

	std::array<std::array<CryptoPP::Integer, common::L>, common::R> x;
	std::array<std::array<CryptoPP::Integer, common::L>, common::R> y;

	std::vector<int> r;

	std::array<std::vector<unsigned>, common::R> indices;

	std::array<std::vector<byte>, common::R> signatures;

	public:
	// results
	CryptoPP::Integer p;
	CryptoPP::Integer q;

	std::vector<SingleBuyer> single_buyers;

	std::array<std::array<sha_commitment::Receiver, common::L>, common::R> d1;
	std::array<std::array<sha_commitment::Receiver, common::L>, common::R> d2;

	Buyer(CryptoPP::Integer n, 
				unsigned price):
		n(n), price(price) {}

	CryptoPP::Integer get_n(){
		return n;
	}

	unsigned get_price(){
		return price;
	}

	Address getSellerAddress() {
		return seller_address;
	}

	void setSellerAddress(Address address) {
		seller_address = address;
	}

	void createT1AndT2();

  Transaction getT1(){
    return T1;
  }
	
	Script getT1OutputScript() {
		return T1_output_script;
	}

	Script getOriginalT1OutputScript() {
		return original_T1_output_script;
	}

	const Hash *getT2Hash() {
		return &T2_hash;
	}

  Transaction getT2(){
    return T2;
  }

	std::array<std::vector<byte>, common::R> getSignatures() {
		return signatures;
	}

	void setSignature(std::array<std::vector<byte>, common::R> signatures) {
		this->signatures = signatures;
	}

	void pickR();

	std::vector<int> getR(){
		return r;
	}

	void genSquares(unsigned j){
		for(unsigned i = 0; i < common::L; ++i) {
			x[j][i] = CryptoPP::Integer(common::rng(), 0, n/2);
			y[j][i] = (x[j][i] * x[j][i]) % n;
		}
	}

	std::array<CryptoPP::Integer, common::L> getSquares(unsigned j){
		return y[j];
	}

	void clearSingleBuyers(){
		this->single_buyers.clear();
	}	

	void addSingleBuyer(const SingleBuyer& single_buyer) {
		this->single_buyers.push_back(single_buyer);
	}

	void pickSubset(unsigned);
	std::array<unsigned, common::L/2> getSubsetIndices(unsigned);
	std::array<CryptoPP::Integer, common::L/2> getSubsetValues(unsigned);

	void verifyRoot(unsigned j, unsigned ind, const std::vector<byte>& key, const std::vector<byte>& c);

	void factorise();
	void make_payment();
	void wait_for_signature_or_time_lock();
	void solve_time_lock(std::atomic_bool &finished);
	void get_signature(std::atomic_bool &finished);

	void set_signature(const std::array<std::vector<byte>, common::R> &signatures) {  // for testing
		this->signatures = signatures;
	}
};

class SellInformationProtocol : public Protocol<Seller, Buyer> {
	public:
	void init(Seller *, Buyer *);
	void exec(Seller *, Buyer *);
	void open(Seller *, Buyer *);
};

}

#endif

