#ifndef _COMMON_H_
#define _COMMON_H_

#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>

#include <cryptopp/ecp.h>
#include <cryptopp/oids.h>
#include <cryptopp/eccrypto.h>

#include <array>
#include <vector>

namespace common {

const unsigned T = 10;
const unsigned K = 10;	// TODO
const unsigned L = 10;	// TODO

extern CryptoPP::AutoSeededRandomPool& rng();
extern CryptoPP::SHA256& sha256();
extern CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP>& ec_parameters();

typedef std::array<byte, 32> TSHA256Digest;

template <typename T>
std::vector<T> concatenate(const std::vector<T>& x, const std::vector<T>& y) {
	std::vector<T> res;

	res.insert(res.end(), x.begin(), x.end());
	res.insert(res.end(), y.begin(), y.end());
	return res;
}

std::vector<byte> to_bytes(const CryptoPP::Integer& x);

}

#endif 
