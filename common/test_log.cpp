
#include "log.h"

#include <string>
#include <vector>
#include <cryptopp/integer.h>

using namespace std;
using namespace CryptoPP;

int main() {
	string s = "hello string";
	Integer x = 5;
	vector<int> v = {-1, 0, 1};
	array<int, 3> a = {-1, 0, 1};

	Log(0);
	Log("hello c str");
	Log(s);
	Log(x);
	Log<int>(v);
	Log<int>(a);
}
