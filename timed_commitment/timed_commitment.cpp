#include "timed_commitment.h"
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/modes.h>

#include <iostream>

using namespace std;

using namespace CryptoPP;

namespace timed_commitment {

	Integer exp(Integer x, unsigned n){
		Integer res = 1;
		while(n){
			if (n%2 == 1){
				res *= x;
			}
			x *= x;
			n /= 2;
		}
		return res;
	}

	// return random prime p 
	// length of p is bits
	// p = 3 mod 4
	// generate safe prime that is: p = 2*q + 1, q is prime too
	Integer Commiter::gen_prime(unsigned bits){

		PrimeAndGenerator gen_p(1, common::rng(), bits);
		Integer res = gen_p.Prime();
		// sanity check p = 3 mod 4
		if (res % 4 != 3){
			throw SanityException("gen prime");
		}
		return res;
	}

	Integer create_timed_key(const Integer &n, const Integer &phi, const Integer &h, unsigned K) {
		Integer a = a_exp_b_mod_c(2, Integer::Power2(K), phi);
		return a_exp_b_mod_c(h, a, n);
	}

	vector<bool> prng_xor(Integer u, const vector<bool> &m) {
		byte seed[48];
		OFB_Mode<AES>::Encryption prng;
		for (unsigned i = 0; i < 48; ++i){
			seed[i] = u.GetByte(i);
		}
		prng.SetKeyWithIV(seed, 32, seed + 32, 16);
    SecByteBlock t(m.size());
		prng.GenerateBlock(t, t.size());
		vector<bool> S(m.size());

		for (unsigned i = 0; i < m.size(); ++i){
			S[i] = m[i] ^ (t.BytePtr()[i] & 1);
		}
		return S;
	}

	vector<bool> Commiter::encode(Integer u, const vector<bool> &m) const {
		return prng_xor(u, m);
	}

	Commitment Commiter::commit(const unsigned K, const vector<bool> &m){
		Integer h, u;
		vector<bool> S;

		// sanity check on m
		for(bool b: m){
			if (b != 0 && b != 1){
				throw SanityException("not binary m");
			}
		}

		h = Integer(common::rng(), 2, n-1);
		u = create_timed_key(n, phi, h, K);

		S = encode(u, m);

		com = Commitment(K, n, h, S, m.size());
		return com;
	}

	Integer Commiter::open(){
		return p;
	}

	void Receiver::accept_commitment(const Commitment &_com){
		//TODO: check K, check n and k size
		this->com = _com;
	}

	vector<bool> Receiver::decode(Integer u) const {
		return prng_xor(u, com.S);
	}

	vector<bool> Receiver::open(Integer p){
		Integer q, r, u, phi;
		Integer::Divide(r, q, com.n, p);
		if (!r.IsZero()){
			throw ProtocolException("p does not divide n");
		}
		phi = (p-1) * (q-1);
		u = create_timed_key(com.n, phi, com.h, com.K);

		return decode(u);
	}

	vector<bool> Receiver::force_open(){
		Integer times = Integer::Power2(com.K);
		Integer u = com.h;
		ModularArithmetic mr(com.n);

		while(!times.IsZero()) {
			times--;
			u = mr.Square(u);
		}

//		for(Integer i = 0; i < Integer::Power2(com.K); ++i){
//			v = a_times_b_mod_c(v, v, com.n);
//		}

		return decode(u);
	}

	Commitment commit(Commiter *c, Receiver *r, const unsigned K, const vector<bool> &m){
		Commitment com = c->commit(K, m);
		r->accept_commitment(com);

		return com;
	}

	vector<bool> open_commitment(Commiter *c, Receiver *r){
		Integer p = c->open();

		vector<bool> res = r->open(p);

		return res;
	}
}

