#include "square_root.h"

#include <iostream>

using namespace CryptoPP;
using namespace std;

int main(){

	Integer p = 17;
	Integer q = 19;

	common::SquareRoot sq(p, q);

	int r = 0;
	int nr = 0;

	for(int i = 0; i < p*q; ++i) {

		Integer a = (i*i) % (p*q);
		Integer root = sq.square_root(a);
		if ((root*root) % (p*q) != a){
			cout << "ERR: " << i << endl;
		}

		Integer root2 = sq(a);
		if ((root2*root2) % (p*q) != a){
			cout << "ERR: " << i << endl;
		}
	}

	for(int i = 0; i < p*q; ++i){
		Integer a = (i*i) % (p*q);
		if (a % p == 0 || a % q == 0)
			continue;

		Integer r1 = sq(a, -1, -1);
		Integer r2 = sq(a, 1, -1);
		Integer r3 = sq(a, -1, 1);
		Integer r4 = sq(a, 1, 1);

		// cout << r1 << "\t" << r2 << "\t" << r3 << "\t" << r4 << endl;

		Integer roots[4] = {r1, r2, r3, r4};

		for(int i = 0; i < 4; ++i){
			for(int j = i+1; j < 4; ++j){
				if (roots[i] == roots[j]){
					cerr << "ERR same roots!" << endl;
				}
				if (roots[i] == -1){
					cerr << "no root!" << endl;
				}
			}
		}
	}


	for(int i = 0; i < p*q; ++i){
		Integer a = (i*i) % (p*q);
		if (a % p == 0 || a % q == 0)
			continue;

		array<Integer, 4> roots = sq.all_square_roots(a);
		if (roots[0] > (p*q/2) || roots[1] > (p*q/2)) {
			cerr << "ERR too big root!" << endl;
		}
		if (roots[2] <= (p*q/2) || roots[3] <= (p*q/2)) {
			cerr << "ERR too small root!" << endl;
		}
	}
}
