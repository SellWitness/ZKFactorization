#include "gen-cpp/BitcoinUtils.h" 

#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <string>
#include <iostream>
#include <memory>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace  ::bitcoin_utils;


std::string string_to_hex(const std::string& input)
{
  static const char* const lut = "0123456789ABCDEF";
  size_t len = input.length();

  std::string output;
  output.reserve(2 * len);
  for (size_t i = 0; i < len; ++i)
  {
    const unsigned char c = input[i];
    output.push_back(lut[c >> 4]);
    output.push_back(lut[c & 15]);
  }
  return output;
}

int main(int argc, char **argv) {
  boost::shared_ptr<TSocket> socket(new TSocket("localhost", 9091));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

  BitcoinUtilsClient client(protocol);
  transport->open();


  client.ping();

  std::string address;
  client.freshAddress(address);

  std::cout << string_to_hex(address) << std::endl;

  transport->close();

  return 0;
}
