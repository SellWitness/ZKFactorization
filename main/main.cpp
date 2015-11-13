#include "../bitcoin/bitcoin.h"
#include "sell_information.h"

#include <cryptopp/integer.h>
#include <cryptopp/osrng.h>
#include <cryptopp/ecp.h>
#include <cryptopp/oids.h>
#include <cryptopp/eccrypto.h>

using sell_information::Seller;
using sell_information::Buyer;
using sell_information::SellInformationProtocol;

using namespace CryptoPP;
using namespace std;

int main(){
	Integer p = 17;
	Integer q = 19;
	unsigned price = 20;

	BitcoinAddress seller_bitcoin_addr;
	BitcoinAddress buyer_bitcoin_addr;

	Seller seller(p, q, price, seller_bitcoin_addr);
	Buyer buyer(p*q, price, buyer_bitcoin_addr);

	SellInformationProtocol sell_information;

	sell_information.init(&seller, &buyer);
	sell_information.exec(&seller, &buyer);

	return 0;
}

