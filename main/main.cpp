#include "sell_information.h"
#include "signature_key.h"

#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/ecp.h>
#include <cryptopp/oids.h>
#include <cryptopp/eccrypto.h>

#include <iostream>
#include <vector>

using sell_information::Seller;
using sell_information::Buyer;
using sell_information::SellInformationProtocol;

using namespace CryptoPP;
using namespace std;

int main(){

	Integer p;
	Integer q;
	unsigned price;

  bool cheat;

  cout << "Input primes" << endl;
  cout << "p: ";
  cin >> p;
  cout << "q: ";
  cin >> q;

  cout << "Input price in satoshis" << endl;
  cout << "price: ";
  cin >> price;

  cout << "Should seller cheat and not accept payment?" << endl;
  cout << "cheat 0/1: ";
  cin >> cheat;

	Seller seller(p, q, price, cheat);
	Buyer buyer(p*q, price);

	SellInformationProtocol sell_information;

  try {

	sell_information.init(&seller, &buyer);
	sell_information.exec(&seller, &buyer);

  } catch (ProtocolException e) {
    cerr << "Protocol exception" << endl;
    cerr << e.what() << endl;
  }

	cout << buyer.p << "\t" << buyer.q << endl;

	return 0;
}

