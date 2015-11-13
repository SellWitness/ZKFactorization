#include "bit_utils.h"

#include <iostream>
#include <cryptopp/integer.h>

using namespace std;
using namespace CryptoPP;

int main(){
	
	vector<string> test_strings = { "", "alalal", "a", "AD#$SAD", "\n" };

	for(string& s: test_strings){
		vector<bool> bits = BitUtils::string_to_bits(s);
		string t = BitUtils::bits_to_string(bits);
		if (s != t) {
			cerr << s << endl;
			throw exception();
		}
	}

	for(int i = 0; i < 1000; ++i){
		vector<bool> bits = BitUtils::integer_to_bits(Integer(i));
		Integer t = BitUtils::bits_to_integer(bits);
		if (i != t) {
			cerr << i << endl;
			throw exception();
		}
	}
}
