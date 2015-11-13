#include "aes.h"

#include <cryptopp/osrng.h>
#include <cryptopp/modes.h>

#include <string>
#include <exception>

#include "../protocol.h"	// TODO remove

using namespace CryptoPP;
using namespace std;

namespace common {

static vector<byte> enc(const byte* key, const vector<byte>& msg){
	string plain(msg.begin(), msg.end());
	string cipher;

	ECB_Mode< AES >::Encryption e;
	e.SetKey(key, AES_LENGTH);

	StringSource(plain, true, new StreamTransformationFilter(e, new StringSink(cipher)));

	return vector<byte>(cipher.begin(), cipher.end());
}

vector<byte> enc(const array<byte, AES_LENGTH>& key, const vector<byte>& msg){
	return enc(key.data(), msg);
}

vector<byte> enc(const vector<byte>& key, const vector<byte>& msg){
	if (key.size() != AES_LENGTH) {
		throw ProtocolException("Invalid length");
	}
	return enc(key.data(), msg);
}

static vector<byte> dec(const byte* key, const vector<byte>& msg) {
	string cipher(msg.begin(), msg.end());
	string plain;

	ECB_Mode< AES >::Decryption d;
	d.SetKey(key, AES_LENGTH);

	StringSource s(cipher, true, new StreamTransformationFilter(d, new StringSink(plain))); 

	return vector<byte>(plain.begin(), plain.end());
}

vector<byte> dec(const array<byte, AES_LENGTH>& key, const vector<byte>& msg) {
	return dec(key.data(), msg);
}

vector<byte> dec(const vector<byte>& key, const vector<byte>& msg) {
	if (key.size() != AES_LENGTH) {
		throw exception();
	}
	return dec(key.data(), msg);
}

}

