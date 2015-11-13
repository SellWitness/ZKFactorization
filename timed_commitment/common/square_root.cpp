#include "square_root.h"
#include <exception>
#include <iostream>
#include <algorithm>

using namespace CryptoPP;
using namespace std;

namespace common {

Integer SquareRoot::square_root(const Integer& a, int sign_x, int sign_y) {
	if (sign_y != -1 && sign_y != 1){
		throw exception();
	}

	if (sign_x != -1 && sign_x != 1){
		throw exception();
	}

	Integer xp = ModularSquareRoot(a, p);
	Integer xq = ModularSquareRoot(a, q);

	if (sign_x == -1) {
		xp = p - xp;
	}
	if (sign_y == -1) {
		xq = q - xq;
	}

	Integer root = CRT(xp, p, xq, q, u);
	if ((root*root) % n != a)
		return -1;
	return root;
}



array<Integer, 4> SquareRoot::all_square_roots(const CryptoPP::Integer& a) {
	array<CryptoPP::Integer, 4> res;
	Integer r1 = square_root(a, -1, -1);
	Integer r2 = square_root(a, 1, -1);
	Integer r3 = square_root(a, -1, 1);
	Integer r4 = square_root(a, 1, 1);

	res[0] = r1;
	res[1] = r2;
	res[2] = r3;
	res[3] = r4;
	sort(res.begin(), res.end());
	return res;
}

}
