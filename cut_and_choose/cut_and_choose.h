#ifndef _CUT_AND_CHOOSE_H
#define _CUT_AND_CHOOSE_H

#include <vector>
#include <algorithm>

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
	std::vector<int> i;
	virtual std::vector<int> getI() const {
    if  (i.size() == 0) {
      throw ProtocolException("i is not set");
    }
    return i;
  }
	bool res;
};

// P - protocol
// A - first party of P
// B - second party of P
// T - security parameter - how many times protocol will be executed
// R - security parameter - how many instances will not be checked
// cheat - if different than -1 prover will not reveal real data in 
// ith instance - for testing
template <typename P, typename A, typename B, unsigned T, unsigned R>
void cut_and_choose(Prover<A> *p, Verifier<B> *v, int cheat = -1){
	P protocol;
	for(int i = 0; i < T; ++i){
		protocol.init(&p->v[i], &v->v[i]);
		protocol.exec(&p->v[i], &v->v[i]);
	}

	if (cheat != -1) {
		p->v[cheat].cheat();
	}

	std::vector<int> chosen_i = v->getI();
	if (chosen_i.size() != R) {
      throw ProtocolException("Wrong set of indices");
	}
	std::sort(chosen_i.begin(), chosen_i.end());
	for (unsigned ix = 0; ix < R; ++ix) {
		if (chosen_i[ix] < 0 || chosen_i[ix] >= T) {
      throw ProtocolException("Wrong set of indices");
		}
		if (ix > 0 && chosen_i[ix] == chosen_i[ix-1]) {
      throw ProtocolException("Wrong set of indices");
		}
	}


	unsigned ix = 0;
	v->res = true;
	for(int i = 0; i < T; ++i){
		if (i != chosen_i[ix]){
			protocol.open(&p->v[i], &v->v[i]);
			if (!v->v[i].getOpenVerified()){
				v->res = false;
				return;
			}
		}
		else {
			++ix;
		}
	}
	if (ix != R) {
      throw ProtocolException("Assert failed");
	}
}

}

#endif 
