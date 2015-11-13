#include "timed_commitment.h"
#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/nbtheory.h>

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

	// computes g as h ** ( mult (qi ** n) ) 
	// where qi are all primes smaller than B
	Integer get_g(const Integer &n, const Integer &h){

		const word16 *primes;
		unsigned available_primes;

		// get the array of primes
		primes = GetPrimeTable(available_primes);
		if (available_primes <= B){
			throw ProtocolException("not enough primes");
		}

		Integer mult = 1;
		for(int i = 0; primes[i] < B; ++i){
			mult *= exp(primes[i], BITS);
		}
		return a_exp_b_mod_c(h, mult, n);
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

	// find order of a element mod (p1*p2)
	// p1 and p2 are safe primes
	static Integer find_order(const Integer &p1, const Integer &p2, const Integer &g){
		Integer n = p1 * p2;
		Integer phi = (p1-1)*(p2-1);
		Integer q1 = (p1-1)/2;
		Integer q2 = (p2-2)/2;

		Integer order = phi;
		vector<Integer> primes;
		primes.push_back(q1);
		primes.push_back(q2);
		primes.push_back(2);

		for(Integer q: primes){
			while(true){
				Integer rest, quotient;
				Integer::Divide(rest, quotient, order, q);
				if (rest != 0)
					break;
				if (a_exp_b_mod_c(g, quotient, n) == 1){
					order = quotient;
				}
				else
					break;
			}
		}

		return order;
	}

	Commitment Commiter::commit(const unsigned K, const vector<bool> &m){
		unsigned available_primes;
		Integer g, h;
		Integer a0, a, u;
		vector<bool> S;
		vector<Integer> W;
		unsigned l;

		// sanity check on m
		for(bool b: m){
			if (b != 0 && b != 1){
				throw SanityException("not binary m");
			}
		}
		l = m.size();

		h = Integer(common::rng(), 2, n-1);
		g = get_g(n, h);
		order = find_order(p, q, g);

		a0 = a_exp_b_mod_c(2, Integer::Power2(K) - l, phi);
		a = a_exp_b_mod_c(2, Integer::Power2(K), phi);
		u = a_exp_b_mod_c(g, a0, n);

		S.resize(l);
		for(unsigned i = 0; i < l; ++i){
			S[i] = m[i] ^ u.GetBit(0);
			u = a_times_b_mod_c(u, u, n);
		}

		// sanity check on u
		if (u != a_exp_b_mod_c(g, a, n)){
			throw SanityException("u != g ** a");
		}

		W.resize(K+1);
		for(unsigned i = 0; i <= K; ++i){
			Integer exp = a_exp_b_mod_c(2, Integer::Power2(i) , phi);
			W[i] = a_exp_b_mod_c(g, exp, n);
		}

		// sanity check on W
		if (W[0] != (g*g % n) || W[1] != (g*g*g*g % n) || W[K] != u){
			throw SanityException("invalid W");
		}
		com = Commitment(K, n, h, g, u, S, W, m.size());
		return com;
	}

	vIvI Commiter::zk_2(const vector<RegularCommitment> &commits){

		if (commits.size() != com.K+1){
			throw ProtocolException("ERR zk_2 commits size");
		}

		this->commits = commits;

		vector<Integer> z, w;

		alpha.resize(com.K+1);
		for(unsigned i = 1; i <= com.K; ++i){ 
			alpha[i].Randomize(common::rng(), Integer::Zero(), order-1);
		}
		z.resize(com.K+1);
		w.resize(com.K+1);
		for(unsigned i = 1; i <= com.K; ++i){ 
			z[i] = a_exp_b_mod_c(com.g, alpha[i], n);
			w[i] = a_exp_b_mod_c(com.W[i-1], alpha[i], n);
		}

		return vIvI(z, w);
	}

	vI Commiter::zk_4(const vI &commit_values){

		if (commit_values.size() != com.K+1){
			throw ProtocolException("invalid commit_values size");
		}

		for(unsigned i = 1; i <= com.K; ++i){
			if (!regular_verify(commits[i], commit_values[i])){
				throw ProtocolException("regular commit didn't verify");
			}
		}

		vI y(com.K+1);
		for(unsigned i = 1; i <= com.K; ++i){
			Integer yi = a_times_b_mod_c(
					commit_values[i],
					a_exp_b_mod_c(2, Integer::Power2(i-1), order),
					order);
			yi += alpha[i];
			yi %= order;
			y[i] = yi;
		}

		return y;
	}

	Integer Commiter::open(){
		Integer vp; 

		vp = a_exp_b_mod_c(
				com.h,
				a_exp_b_mod_c(2, Integer::Power2(com.K) - com.l, phi),
				n);

		return vp;
	}

	void Receiver::accept_commitment(const Commitment &_com){
		this->com = _com;
		if (com.g != get_g(com.n, com.h)){
			throw ProtocolException("g != get_g");
		}
		if (com.W.size() != com.K+1){
			throw ProtocolException("W.size() != K+1");
		}

		if (com.W[0] != (com.g*com.g % com.n) || com.W[com.K] != com.u) {
			throw ProtocolException("invalid W");
		}
	}

	vector<RegularCommitment> Receiver::zk_1(){

		vector<RegularCommitment> res;
		commit_values.resize(com.K+1);
		res.resize(com.K+1);

		for(unsigned i = 1; i <= com.K; ++i){
			commit_values[i].Randomize(common::rng(), Integer::Zero(), R);
			res[i] = regular_commit(commit_values[i]);
		}
		return res;
	}

	vector<Integer> Receiver::zk_3(const vIvI &zw){

		if (zw.first.size() != com.K+1 || zw.second.size() != com.K+1){
			throw ProtocolException("invalid zw size");
		}

		this->zw = zw;

		return commit_values;
	}

	void Receiver::zk_5(const vI &y){
		if (y.size() != com.K+1){
			throw ProtocolException("y size != K+1");
		}

		for(unsigned i = 1; i <= com.K; ++i){

			Integer zi = a_times_b_mod_c(
					a_exp_b_mod_c(com.g, y[i], com.n),
					a_exp_b_mod_c(com.W[i-1].InverseMod(com.n), commit_values[i], com.n),
					com.n);
			if (zi != zw.first[i]){
				throw ProtocolException("zi verification failed");
			}

			Integer wi = a_times_b_mod_c(
					a_exp_b_mod_c(com.W[i-1], y[i], com.n),
					a_exp_b_mod_c(com.W[i].InverseMod(com.n), commit_values[i], com.n),
					com.n);
			if (wi != zw.second[i]){
				throw ProtocolException("wi verification failed");
			}
		}
	}


	vector<bool> Receiver::decode(Integer v){
		vector<bool> res(com.l);
		for(unsigned i = 0; i < com.l; ++i){
			res[i] = com.S[i] ^ v.GetBit(0);
			v = a_times_b_mod_c(v, v, com.n);
		} 

		if (v != com.u){
			throw SanityException("v != com.u");
		}
		return res;
	}

	vector<bool> Receiver::open(Integer vp){

		Integer v = get_g(com.n, vp);

		Integer u = a_exp_b_mod_c(v, Integer::Power2(com.l), com.n);

		if (u != com.u){
			throw ProtocolException("u != com.u");
		}

		return decode(v);
	}

	vector<bool> Receiver::force_open(){

		Integer v = com.g;

		for(Integer i = 0; i < Integer::Power2(com.K) - com.l; ++i){
			v = a_times_b_mod_c(v, v, com.n);
		}

		Integer u = a_exp_b_mod_c(v, Integer::Power2(com.l), com.n);

		if (u != com.u){
			throw ProtocolException("u != com.u");
		}

		return decode(v);
	}

	vector<bool> Receiver::force_open_smart(){

		Integer v = com.W[com.K-1];

		for(Integer i = 0; i < Integer::Power2(com.K-1) - com.l; ++i){
			v = a_times_b_mod_c(v, v, com.n);
		}

		Integer u = a_exp_b_mod_c(v, Integer::Power2(com.l), com.n);

		if (u != com.u){
			throw ProtocolException("u != com.u");
		}

		return decode(v);
	}


	Commitment commit(Commiter *c, Receiver *r, const unsigned K, const vector<bool> &m){
		Commitment com = c->commit(K, m);
		r->accept_commitment(com);

		for(int i = 0; i < 10; ++i){
			vector<RegularCommitment> commits = r->zk_1();
			vIvI zw = c->zk_2(commits);
			vI commit_values = r->zk_3(zw);
			vI y = c->zk_4(commit_values);
			r->zk_5(y);
		}
		return com;
	}

	vector<bool> open_commitment(Commiter *c, Receiver *r){
		Integer vp = c->open();

		vector<bool> res = r->open(vp);

		return res;
	}
}

