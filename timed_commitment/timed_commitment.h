#ifndef _TIMED_COMMITMENT_H_
#define _TIMED_COMMITMENT_H_

#include "regular_commitment.h"
#include "../common/common.h"

#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>

#include <string>
#include <vector>
#include <utility>

#include <exception>

namespace timed_commitment {

	const unsigned B = 128;     // how many primes in commit in h exponent
	const unsigned R = 4*B;     // security parametar in zk proofs in commit
	const unsigned BITS = 512;  // length in bits of primes p and q

	typedef std::vector<CryptoPP::Integer> vI;
	typedef std::pair< vI , vI > vIvI;

	CryptoPP::Integer get_g(const CryptoPP::Integer &n, const CryptoPP::Integer &h);

	// exception thrown when something impossible (insane) happens
	class SanityException: public std::exception {
		std::string msg;
		public:
		SanityException(const std::string &msg):msg(msg){
		}
		virtual const char* what() const throw(){
			return msg.c_str();
		}
	};

	class ProtocolException: public std::exception {
		std::string msg;
		public:
		ProtocolException(const std::string &msg):msg(msg){
		}
		virtual const char* what() const throw(){
			return msg.c_str();
		}
	};

	struct Commitment {
		unsigned K;

		CryptoPP::Integer n;
		CryptoPP::Integer h;
		CryptoPP::Integer g;
		CryptoPP::Integer u;
		std::vector<bool> S;
		std::vector<CryptoPP::Integer> W; // < g**2, ..., g ** (2 ** ( 2 ** k ) ) > 
		unsigned l;

		Commitment(){}

		Commitment( unsigned K,
				CryptoPP::Integer n, 
				CryptoPP::Integer h,
				CryptoPP::Integer g,
				CryptoPP::Integer u,
				std::vector<bool> S,
				std::vector<CryptoPP::Integer> W,
				unsigned l):
			K(K), n(n), h(h), g(g), u(u), S(S), W(W), l(l){
			}
	};

	class Commiter{
		private:

			CryptoPP::Integer p;
			CryptoPP::Integer q;
			CryptoPP::Integer n;
			CryptoPP::Integer phi;

			CryptoPP::Integer gen_prime(unsigned bits);

			Commitment com;
			CryptoPP::Integer order;  // order of com.g mod n
			std::vector<RegularCommitment> commits;
			std::vector<CryptoPP::Integer> alpha;

		public:

			Commiter() {
					p = gen_prime(BITS);
					q = gen_prime(BITS);
					n = p*q;
					phi = (p-1)*(q-1);
				}

			CryptoPP::Integer get_n(){
				return n;
			}

			Commitment commit(const unsigned K, const std::vector<bool> &);

			vIvI zk_2(const std::vector<RegularCommitment> &);

			vI zk_4(const vI &commit_values);

			CryptoPP::Integer open();
	};

	// one time use 
	// new Receiver should be created for each commitment
	class Receiver {

		private:
			Commitment com;

			std::vector<CryptoPP::Integer> commit_values;
			vIvI zw;

			std::vector<bool> decode(CryptoPP::Integer v);

		public:

			Receiver() {}
			
			void accept_commitment(const Commitment &com);

			std::vector<RegularCommitment> zk_1();

			std::vector<CryptoPP::Integer> zk_3(const vIvI &zw);

			void zk_5(const vI &y);

			std::vector<bool> open(CryptoPP::Integer vp);

			std::vector<bool> force_open();

			std::vector<bool> force_open_smart(); // twice faster
	};

	Commitment commit(Commiter *c, Receiver *r, const unsigned K, const std::vector<bool> &);
	
	std::vector<bool> open_commitment(Commiter *c, Receiver *r);
}

#endif


