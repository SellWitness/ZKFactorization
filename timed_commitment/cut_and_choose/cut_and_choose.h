#ifndef _CUT_AND_CHOOSE_H
#define _CUT_AND_CHOOSE_H

#include <vector>

namespace cut_and_choose {

template <typename A>
class Prover {
	public:
	std::vector<A> v;
};

template <typename B>
class Verifier {

	public:
	std::vector<B> v;
	int i;
	virtual int getI(){ return i; }
	bool res;
};

// P - protocol
// A - first party of P
// B - second party of P
// T - security parameter - how many times protocol will be executed
// cheat - if different than -1 prover will not reveal real data in 
// ith instance - for testing
template <typename P, typename A, typename B, unsigned T>
void cut_and_choose(Prover<A> *p, Verifier<B> *v, int cheat = -1){
	P protocol;
	for(int i = 0; i < T; ++i){
		protocol.init(&p->v[i], &v->v[i]);
		protocol.exec(&p->v[i], &v->v[i]);
	}

	if (cheat != -1) {
		p->v[cheat].cheat();
	}

	int chosen_i = v->getI();
	v->res = true;
	for(int i = 0; i < T; ++i){
		if (i != chosen_i){
			protocol.open(&p->v[i], &v->v[i]);
			if (!v->v[i].getOpenVerified()){
				v->res = false;
				return;
			}
		}
	}
}

}

#endif 
