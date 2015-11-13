#ifndef _LOG_H_
#define _LOG_H_

#include <string>
#include <vector>
#include <cryptopp/integer.h>

void Log(int);
void Log(unsigned);
void Log(const char*);
void Log(const std::string&);
void Log(const CryptoPP::Integer&);

template <typename T>
void Log(const std::vector<T> &v) {
	Log("vector of len:");
	Log((unsigned)v.size());
	for(const T& elem: v) {
		Log(elem);
	}
}

template <typename T, size_t L>
void Log(const std::array<T, L> &a) {
	Log("array of len:");
	Log((unsigned)a.size());
	for(const T& elem: a) {
		Log(elem);
	}
}

#endif
