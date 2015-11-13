
#include "regular_commitment.h"

#include <cryptopp/osrng.h>
#include <cryptopp/integer.h>

#include <iostream>

using namespace std;
using namespace CryptoPP;

int main(){

  AutoSeededRandomPool rng;

  for(int i = 0; i < 1000; ++i){
    Integer x(rng, 0, Integer::Power2(512));

    RegularCommitment commitment = regular_commit(x);
    if (!regular_verify(commitment, x)){
      cerr << "fail " << x << endl;
      return 1;
    }
  }

  cerr << "OK" << endl;
  return 0;
}

