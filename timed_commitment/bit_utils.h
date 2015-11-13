
#ifndef _BIT_UTILS_H_
#define _BIT_UTILS_H_

#include <vector>
#include <string>
#include <cryptopp/integer.h>

class BitUtils {
	public:
	static std::vector<bool> string_to_bits(const std::string&);
	static std::vector<bool> integer_to_bits(const CryptoPP::Integer&);

	static std::string bits_to_string(const std::vector<bool> &);
	static CryptoPP::Integer bits_to_integer(const std::vector<bool> &);
};

#endif
