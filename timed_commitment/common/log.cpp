
#include "log.h"

#include <iostream>

using namespace std;

void Log(int x) {
	cout << x << endl;
}

void Log(unsigned x) {
	cout << x << endl;
}

void Log(const char* s) {
	cout << s << endl;
}
void Log(const std::string& s) {
	cout << s << endl;
}

void Log(const CryptoPP::Integer& x) {
	cout << x << endl;
}

