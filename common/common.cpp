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

std::string integer2string(const CryptoPP::Integer ix) {
  std::vector<byte> vx = to_bytes(ix);
  std::string x(vx.begin(), vx.end());
  return x;
}

std::string string2hex(const std::string& input)
{
  static const char* const lut = "0123456789ABCDEF";
  size_t len = input.length();

  std::string output;
  output.reserve(2 * len);
  for (size_t i = 0; i < len; ++i)
  {
    const unsigned char c = input[i];
    output.push_back(lut[c >> 4]);
    output.push_back(lut[c & 15]);
  }
  return output;
}

std::vector<byte> string2v(const std::string& input){
  return std::vector<byte>(input.begin(), input.end());
}

std::string v2string(const std::vector<byte>& input){
  return std::string(input.begin(), input.end());
}

}

