#ifndef _SQUARE_ROOT_H_
#define _SQUARE_ROOT_H_

#include "cryptopp/integer.h"
#include "cryptopp/nbtheory.h"
#include <array>

namespace common {

class SquareRoot {
	CryptoPP::Integer p;
	CryptoPP::Integer q;
	CryptoPP::Integer n;

	CryptoPP::Integer u;

	public:
	SquareRoot(CryptoPP::Integer p, CryptoPP::Integer q): p(p), q(q), n(p*q) {
		u = p.InverseMod(q);
	}

	CryptoPP::Integer square_root(const CryptoPP::Integer& a, int sign_x = 1, int sign_y = 1);
	CryptoPP::Integer operator()(const CryptoPP::Integer& a, int sign_x = 1, int sign_y = 1){
		return square_root(a, sign_x, sign_y);
	}

	std::array<CryptoPP::Integer, 4> all_square_roots(const CryptoPP::Integer&);
};

}

#endif 
