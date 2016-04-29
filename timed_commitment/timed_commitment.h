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

	const unsigned BITS = 512;  // length in bits of primes p and q


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
		std::vector<bool> S;
		unsigned l;

		Commitment(){}

		Commitment( unsigned K,
				CryptoPP::Integer n, 
				CryptoPP::Integer h,
				std::vector<bool> S,
				unsigned l):
			K(K), n(n), h(h), S(S), l(l){
			}
	};

	class Commiter{
		private:

			std::vector<bool> encode(CryptoPP::Integer, const std::vector<bool>&) const;

			CryptoPP::Integer p;
			CryptoPP::Integer q;
			CryptoPP::Integer n;
			CryptoPP::Integer phi;

			CryptoPP::Integer gen_prime(unsigned bits);

			Commitment com;

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

			CryptoPP::Integer open();
	};

	// one time use 
	// new Receiver should be created for each commitment
	class Receiver {

		private:
			Commitment com;

			std::vector<bool> decode(CryptoPP::Integer) const;

		public:

			Receiver() {}
			
			void accept_commitment(const Commitment &com);

			std::vector<bool> open(CryptoPP::Integer);

			std::vector<bool> force_open();
	};

	Commitment commit(Commiter *c, Receiver *r, const unsigned K, const std::vector<bool> &);
	
	std::vector<bool> open_commitment(Commiter *c, Receiver *r);
}

#endif


