#include <cryptopp/integer.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/osrng.h>
#include <iostream>

using namespace CryptoPP;
using namespace std;

const int BITS = 128;

Integer find_order(Integer p1, Integer p2, Integer g){
  Integer n = p1 * p2;
  Integer phi = (p1-1)*(p2-1);
  Integer q1 = (p1-1)/2;
  Integer q2 = (p2-2)/2;

  Integer order = phi;
  vector<Integer> primes;
  primes.push_back(q1);
  primes.push_back(q2);
  primes.push_back(2);

  cerr << "find order" << endl;

  for(Integer q: primes){
    while(true){
      Integer rest, quotient;
      Integer::Divide(rest, quotient, order, q);
      if (rest != 0)
        break;
      if (a_exp_b_mod_c(g, quotient, n) == 1){
        order = quotient;
        cerr << q << endl;
      }
      else
        break;
    }
  }
  cerr << "order: " << order << endl;
  return order;
}

int main(){
  AutoSeededRandomPool rng;

  PrimeAndGenerator gen_p(1, rng, BITS);
  PrimeAndGenerator gen_q(1, rng, BITS);
  Integer p = gen_p.Prime();
  Integer q = gen_q.Prime();
  Integer n = p * q;

  cerr << (a_exp_b_mod_c(2, (p-1)*(q-1), n) == 1 ? "OK" : "FAIL") << endl;

  int gs[] = {2, 3, 4, 5 };
  for(int g: gs){
    Integer ord = find_order(p, q, g);
    cerr << g << " " << ord << " " << a_exp_b_mod_c(g, ord, n) << endl;
  }
}


