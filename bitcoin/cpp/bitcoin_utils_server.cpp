#include "gen-cpp/BitcoinUtils.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <cryptopp/ecp.h>
#include <cryptopp/oids.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>

#include <iostream>
#include <vector>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::bitcoin_utils;
using namespace CryptoPP;

class BitcoinUtilsHandler : virtual public BitcoinUtilsIf {
 public:
  BitcoinUtilsHandler() {
    // Your initialization goes here
  }

  void ping() {
    // Your implementation goes here
    printf("ping\n");
  }

  void freshAddress(Address& _return) {
    // Your implementation goes here
    printf("freshAddress\n");
  }

  void createSend(Transaction& _return, const IntegerBytes& x, const IntegerBytes& y, const int32_t value) {
    // Your implementation goes here
    printf("createSend\n");
  }

  void createSendTransactionToAddress(Transaction& _return, const Transaction& tx, const IntegerBytes& x, const IntegerBytes& y, const Address& dst) {
    // Your implementation goes here
    printf("createSendTransactionToAddress\n");
  }

  void getOutputScript(Script& _return, const Transaction& tx, const IntegerBytes& x, const IntegerBytes& y) {
    // Your implementation goes here
    printf("getOutputScript\n");
  }

  void hashForSignature(Hash& _return, const Transaction& tx, const IntegerBytes& x, const IntegerBytes& y, const Script& redeem_script) {
    // Your implementation goes here
    printf("hashForSignature\n");
  }

  bool verifySignature(const Transaction& tx, const IntegerBytes& x, const IntegerBytes& y, const IntegerBytes& r, const IntegerBytes& s, const Script& redeem_script) {
    // Your implementation goes here
    printf("verifySignature\n");
    return true;
  }

  // verify the signature using crypto++ verifier
  int cryptopp_test(ECPPoint Q, Integer r, Integer s, byte *message, unsigned message_length){
    std::vector<byte> signature(64);
    r.Encode(signature.data(), 32);
    s.Encode(signature.data()+32, 32);

    ECDSA<ECP, SHA256>::PublicKey publicKey;
    publicKey.Initialize(ASN1::secp256k1(), Q);

    ECDSA<ECP, SHA256>::Signer signer;
    ECDSA<ECP, SHA256>::Verifier verifier(publicKey);

    bool result = verifier.VerifyMessage( message, message_length, signature.data(), 64);
    if(!result){
      // cerr << "FAIL cryptopp" << endl;
      return 1;
    }
    return 0;
  }

  bool cppVerifySignature(const std::string& data, const IntegerBytes& bx, const IntegerBytes& by, const IntegerBytes& br, const IntegerBytes& bs) {
    // Your implementation goes here
    printf("cppVerifySignature\n");

    Integer x((byte*)bx.data(), bx.size());
    Integer y((byte*)by.data(), by.size());
    Integer r((byte*)br.data(), br.size());
    Integer s((byte*)bs.data(), bs.size());

    std::cout << x << std::endl;
    std::cout << y << std::endl;
    std::cout << r << std::endl;
    std::cout << s << std::endl;

    ECPPoint Q(x, y);
    
    bool res = cryptopp_test(Q, r, s, (byte*)data.data(), data.size());
    std::cout << "Result: "  << res << std::endl;

    return res;
  }


  void getSignedTransaction(Transaction& _return, const Transaction& tx, const IntegerBytes& x, const IntegerBytes& y, const IntegerBytes& r, const IntegerBytes& s, const Script& redeem_script) {
    std::cout << "get signed transaction" << std::endl;
    std::cout << "Not implemented" << std::endl;
  }

  bool broadcastTransaction(const Transaction& tx) {
    std::cout << "broadcast transaction" << std::endl;
    std::cout << "Not implemented" << std::endl;
    return false;
  }

  bool verifyTransaction(const Transaction& tx, const IntegerBytes& x, const IntegerBytes& y, const int32_t value) {
    std::cout << "verify transaction" << std::endl;
    std::cout << "Not implemented" << std::endl;
    return false;
  }

  bool waitForTransaction(const Transaction& tx, const int32_t depth) {
    std::cout << "wait for transaction" << std::endl;
    std::cout << "Not implemented" << std::endl;
    return false;
  }
};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<BitcoinUtilsHandler> handler(new BitcoinUtilsHandler());
  shared_ptr<TProcessor> processor(new BitcoinUtilsProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

