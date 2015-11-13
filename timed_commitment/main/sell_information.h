#ifndef _SELL_INFORMATION_
#define _SELL_INFORMATION_

#include "signature_key.h"

#include "../shared_signature/shared_sig.h"
#include "../bitcoin/bitcoin.h"
#include "../common/common.h"
#include "../common/aes.h"
#include "../protocol.h"
#include "../common/square_root.h"

#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>

namespace sell_information {

class Seller {
	// p*q = n, Buyer wants to buy p, q
	CryptoPP::Integer p;	
	CryptoPP::Integer q;
	CryptoPP::Integer n;

	unsigned price;

	BitcoinAddress bitcoin_address;

	common::SquareRoot square_root;

	std::array<CryptoPP::Integer, common::L> y;
	std::array<CryptoPP::Integer, common::L> r1;
	std::array<CryptoPP::Integer, common::L> r2;

	std::array<std::vector<byte>, common::L> c1;
	std::array<std::vector<byte>, common::L> c2;

	public:

	SingleSeller single_seller;

	std::array<sha_commitment::Sender, common::L> d1;
	std::array<sha_commitment::Sender, common::L> d2;

	Seller(CryptoPP::Integer p, 
				CryptoPP::Integer q, 
				unsigned price, 
				BitcoinAddress bitcoin_address):
		p(p), q(q), n(p*q), price(price), bitcoin_address(bitcoin_address), square_root(p, q) {}

	CryptoPP::Integer get_n(){
		return p*q;
	}

	unsigned get_price(){
		return price;
	}
	
	BitcoinAddress get_address(){
		return bitcoin_address;
	}

	void acceptSquares(const std::array<CryptoPP::Integer, common::L>& y){
		this->y = y;
	}

	void findRoots();
	
	void encryptRoots();

	void setSingleSeller(const SingleSeller& single_seller){
		this->single_seller = single_seller;
	}	

	void setupCommit();

	std::array<unsigned, common::L/2> acceptSubset(
		const std::array<unsigned, common::L/2>& indices, 
		const std::array<CryptoPP::Integer, common::L/2>& values);
};

class Buyer {
	CryptoPP::Integer n;	// The rsa modulus - would like to buy primes p, q: p*q = n

	unsigned price;

	BitcoinAddress bitcoin_address;

	std::array<CryptoPP::Integer, common::L> x;
	std::array<CryptoPP::Integer, common::L> y;

	int r;

	std::vector<unsigned> indices;

	public:
	SingleBuyer single_buyer;

	std::array<sha_commitment::Receiver, common::L> d1;
	std::array<sha_commitment::Receiver, common::L> d2;

	Buyer(CryptoPP::Integer n, 
				unsigned price,
				BitcoinAddress bitcoin_address):
		n(n), price(price), bitcoin_address(bitcoin_address) {}

	CryptoPP::Integer get_n(){
		return n;
	}

	unsigned get_price(){
		return price;
	}

	BitcoinAddress get_address(){
		return bitcoin_address;
	}

	void pickR();

	int getR(){
		return r;
	}

	void genSquares(){
		for(unsigned i = 0; i < common::L; ++i) {
			x[i] = CryptoPP::Integer(common::rng(), 0, n/2);
			y[i] = (x[i] * x[i]) % n;
		}
	}

	std::array<CryptoPP::Integer, common::L> getSquares(){
		return y;
	}

	void setSingleBuyer(const SingleBuyer& single_buyer) {
		this->single_buyer = single_buyer;
	}

	void pickSubset();
	std::array<unsigned, common::L/2> getSubsetIndices();
	std::array<CryptoPP::Integer, common::L/2> getSubsetValues();

	void verifyRoot(unsigned ind, const std::vector<byte>& key, const std::vector<byte>& c);
};

class SellInformationProtocol : public Protocol<Seller, Buyer> {
	public:
	void init(Seller *, Buyer *);
	void exec(Seller *, Buyer *);
	void open(Seller *, Buyer *);
};

}

#endif

