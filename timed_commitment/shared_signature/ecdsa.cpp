/* ecdsa implementation using crypto++ Integer */

#include <cryptopp/ecp.h>
#include <cryptopp/oids.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>

#include <iostream>

#include "shared_sig.h"

using namespace shared_signature;

using namespace CryptoPP;

using namespace std;
/*
Integer m_to_int(byte *m, unsigned m_len, unsigned ret_byte_count){
  SHA256 sha256;
  byte h[sha256.DigestSize()];
  SHA256().CalculateDigest(h, m, m_len);
  Integer hi(h, ret_byte_count);
  return hi;
}
*/

int main(){
  AutoSeededRandomPool rng;

  DL_GroupParameters_EC<ECP> ec_parameters;
  ECP ec;
  ECPPoint G;
  Integer n;
  ec_parameters.Initialize(CryptoPP::ASN1::secp256k1());
  ec = ec_parameters.GetCurve();
  G = ec_parameters.GetSubgroupGenerator();
  n = ec_parameters.GetGroupOrder();

  cout << hex << "a: " << ec.GetA() << endl;
  cout << hex << "b: " << ec.GetB() << endl;
  cout << hex << "field size " << ec.FieldSize() << endl;
  cout << hex << "G: " << G.x << " " << G.y << endl;
  cout << hex << "n: " << n << endl;

  // key gen
  Integer da(rng, Integer::One(), n);
  ECPPoint Qa = ec.Multiply(da, G);
  
  // sign
  string message = "to be or not to be";
  Integer z = m_to_int((byte*) message.data(), message.length(), n.ByteCount());

  Integer k(rng, Integer::One(), n);
  ECPPoint P1 = ec.Multiply(k, G);
  Integer r = P1.x % n;
  Integer s = (k.InverseMod(n) * (z+r*da) ) % n;
  
  // verify
  if (Qa == ec.Identity()){
    cerr << "Qa == O" << endl;
    return 1;
  }
  if (!(ec.Multiply(n, Qa) == ec.Identity())){
    cerr << "n x Qa != O" << endl;
    return 1;
  }
  if (r <= 0 || r >= n){
    cerr << "incorrect r" << endl;
    return 1;
  } 
  if (s <= 0 || s >= n){
    cerr << "incorrect s" << endl;
    return 1;
  } 
  Integer w = s.InverseMod(n);
  Integer u1 = (z*w) % n;
  Integer u2 = (r*w) % n;
  ECPPoint P2 = ec.Add(ec.Multiply(u1, G), ec.Multiply(u2, Qa));

  if (P2.x == r){
    cout << "OK" << endl;
  } else {
    cerr << "FAIL" << endl;
  }
  
  // verify using crypto++ 
  ECDSA<ECP, SHA256>::PublicKey publicKey;
  publicKey.Initialize(CryptoPP::ASN1::secp256k1(), Qa);

  ECDSA<ECP, SHA256>::Signer signer;
  ECDSA<ECP, SHA256>::Verifier verifier(publicKey);

  byte signature[64];
  r.Encode(signature, 32);
  s.Encode(signature+32, 32);
  bool result = verifier.VerifyMessage( (const byte*)message.data(), message.size(), signature, 64 );
  if(result)
    cout << "Verified signature on message" << endl;
  else
    cerr << "Failed to verify signature on message" << endl;
}

