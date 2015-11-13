#include "bit_utils.h"
#include <exception>

using namespace std;
using namespace CryptoPP;

template<typename Cont, typename Elem>
vector<bool> to_bits(const Cont& cont){
	vector<bool> res;
  for(Elem e: cont){
    for(unsigned i = 0; i < 8; ++i){
      res.push_back(e & (1<<i));
    }
  }
  return res;
}

template<typename Elem>
vector<Elem> from_bits(const vector<bool>& bits){
	unsigned l = bits.size();
  if (l % 8 != 0){
		throw exception();
  }
	vector<Elem> res;
  for(unsigned i = 0; i < l; i += 8){
    unsigned char x = 0;
    for(unsigned bit = 0; bit < 8; ++bit){
      if (bits[i+bit])
        x += 1 << bit;
    }
    res.push_back(x);
  }
  return res;
}

vector<bool> BitUtils::string_to_bits(const string& s){
	return to_bits<string, unsigned char>(s);
}

vector<bool> BitUtils::integer_to_bits(const Integer& i){

	if (i < 0) {
		throw exception();	// TODO
	}

	vector<byte> bytes(i.MinEncodedSize());
	i.Encode(bytes.data(), i.MinEncodedSize());
	return to_bits<vector<byte>, byte>(bytes);
}

string BitUtils::bits_to_string(const vector<bool> &bits){
	vector<unsigned char> res = from_bits<unsigned char>(bits);
	return string(res.begin(), res.end());
}

Integer BitUtils::bits_to_integer(const vector<bool> &bits){
	vector<byte> bytes = from_bits<byte>(bits);
	return Integer(bytes.data(), bytes.size());
}
