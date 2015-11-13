#include "paillier.h"
#include "../common/common.h"
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

using CryptoPP::Integer;

// each of the primes will have 'bits' bits, n will have 2*'bits' bits 
Paillier::Paillier(unsigned bits) {
  p.Randomize(common::rng(), Integer::One()<<bits, Integer::One()<<(bits+1), Integer::PRIME);
  q.Randomize(common::rng(), Integer::One()<<bits, Integer::One()<<(bits+1), Integer::PRIME);

  n = p * q;
  n2 = n * n;
  g = n + 1;
  y = (p-1)*(q-1);
  phi = (p-1)*(q-1);
  u = phi.InverseMod(n);
}

Paillier::Paillier(CryptoPP::Integer n, CryptoPP::Integer g){
  this->n = n;
  this->n2 = n*n;
  this->g = g;
  this->p = -1;
  this->q = -1;
}

Integer Paillier::L(Integer x){
  return (x-1)/n;
}

Integer Paillier::get_n(){
	if (n == 0){
		throw PaillierException("not initialized");
	}
  return n;
}

Integer Paillier::get_n2(){
	if (n == 0){
		throw PaillierException("not initialized");
	}
  return n2;
}

Integer Paillier::get_g(){
	if (n == 0){
		throw PaillierException("not initialized");
	}
  return g;
}

Integer Paillier::enc(Integer m){

	if (n == 0){
		throw PaillierException("not initialized");
	}

  if (m >= n || m < 0){
    throw PaillierException("m >= n || m < 0 ");
  }
  Integer r;
	r.Randomize(common::rng(), Integer::One(), n);	// check divisibility by p, q ?
  return a_times_b_mod_c(a_exp_b_mod_c(g, m, n2), a_exp_b_mod_c(r, n, n2), n2);
}

Integer Paillier::dec(Integer c){

	if (n == 0){
		throw PaillierException("not initialized");
	}

  if (p == -1 || q == -1){
		throw PaillierException("client can not decrypt data");
  }
  if (c <= 0 || c >= n2){
    throw PaillierException("c <= 0 || c >= n2");
  }
  if (c % p == 0 || c % q == 0){	// necessary ?
		throw PaillierException("c % p == 0 || c % q == 0");
  }

  return a_times_b_mod_c(L(a_exp_b_mod_c(c, y, n2)), u, n);
}

