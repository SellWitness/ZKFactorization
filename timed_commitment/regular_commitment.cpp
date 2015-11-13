#include "regular_commitment.h"

#include <cryptopp/integer.h>
#include <cryptopp/sha.h>

#include <iostream>

using namespace CryptoPP;
using namespace std;

static RegularCommitment sha256(const Integer &x){
  
  unsigned len = x.MinEncodedSize();
  RegularCommitment res;
  byte m[len];
  x.Encode(m, len);
  SHA256 sha256;
  SHA256().CalculateDigest(res.digest, m, len);
  return res;
}

RegularCommitment regular_commit(const Integer &x){
  return sha256(x);
}

bool regular_verify(const RegularCommitment &c, const Integer &x){
  return c == sha256(x);
}

