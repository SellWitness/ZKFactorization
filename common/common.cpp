#include "common.h"

namespace common {


CryptoPP::AutoSeededRandomPool& rng(){
	static CryptoPP::AutoSeededRandomPool _rng;
	return _rng;
};

CryptoPP::SHA256& sha256() {
	static CryptoPP::SHA256 _sha256;
	return _sha256;
}
CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP>& ec_parameters() {
	static CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP> _ec_p (CryptoPP::ASN1::secp256k1());
	return _ec_p;
}

std::vector<byte> to_bytes(const CryptoPP::Integer& x) {
	std::vector<byte> res(x.MinEncodedSize());
	x.Encode(res.data(), res.size());
	return res;
}

}

