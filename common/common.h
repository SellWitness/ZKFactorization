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

const unsigned T = 512; 	// it is called a in the paper
const unsigned K = 37;		// it is log(t) in the paper
const unsigned L = 2048;	// it is 2*lambda in the paper
const unsigned R = 8;			// it is called b in the paper

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

std::string integer2string(const CryptoPP::Integer ix);

std::string string2hex(const std::string& input);

std::vector<byte> string2v(const std::string& input);

std::string v2string(const std::vector<byte>& input);



}

#endif 
