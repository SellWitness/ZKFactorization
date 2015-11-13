#ifndef _AES_H_
#define _AES_H_

#include <cryptopp/aes.h>

#include <vector>
#include <array>

namespace common {
	
const unsigned AES_LENGTH = 32;

extern std::vector<byte> enc(const std::array<byte, AES_LENGTH>& key, const std::vector<byte>& msg);
extern std::vector<byte> enc(const std::vector<byte>& key, const std::vector<byte>& msg);

extern std::vector<byte> dec(const std::array<byte, AES_LENGTH>& key, const std::vector<byte>& msg);
extern std::vector<byte> dec(const std::vector<byte>& key, const std::vector<byte>& msg);

}

#endif
