#ifndef _PAILLIER_H_
#define _PAILLIER_H_

#include "cryptopp/integer.h"

class PaillierException: public std::exception {
	std::string msg;
	public:
	PaillierException(const std::string &msg):msg(msg){
	}
	virtual const char* what() const throw(){
		return msg.c_str();
	}
};

class Paillier{

	private:

		CryptoPP::Integer p;
		CryptoPP::Integer q;

		CryptoPP::Integer n;
		CryptoPP::Integer n2; // n2 = n * n;
		CryptoPP::Integer y;  // y = phi(n) = (p-1)(q-1)
		CryptoPP::Integer g;  // g == n + 1
		CryptoPP::Integer u;  // u = y ** (-1) mod n
		CryptoPP::Integer phi;  // phi = (p-1)*(q-1)

		CryptoPP::Integer L(CryptoPP::Integer x);

	public:

		Paillier():n(CryptoPP::Integer::Zero()){
		}

		Paillier(unsigned bits);  // gen new randoms,

		Paillier(CryptoPP::Integer n, CryptoPP::Integer g); // client -pubkey only

		CryptoPP::Integer get_n();
		CryptoPP::Integer get_n2();

		CryptoPP::Integer get_g();

		CryptoPP::Integer enc(CryptoPP::Integer m);

		CryptoPP::Integer dec(CryptoPP::Integer c);
};

#endif 
