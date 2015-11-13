#ifndef _REGULAR_COMMITMENT_H_
#define _REGULAR_COMMITMENT_H_

#include <cryptopp/integer.h>

struct RegularCommitment{
  static const int COMMITMENT_SIZE = (256/8);
  byte digest[COMMITMENT_SIZE];

  friend bool operator==(const RegularCommitment &x, const RegularCommitment& y){
    for(int i = 0; i < COMMITMENT_SIZE; ++i){
      if (x.digest[i] != y.digest[i])
        return false;
    }
    return true;
  }
};

RegularCommitment regular_commit(const CryptoPP::Integer &);
bool regular_verify(const RegularCommitment &, const CryptoPP::Integer &);

#endif


