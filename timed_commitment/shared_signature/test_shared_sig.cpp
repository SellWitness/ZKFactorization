#include "shared_sig.h"

#include <cryptopp/ecp.h>
#include <cryptopp/oids.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace CryptoPP;
using namespace shared_signature;

static const int MESSAGE_LENGTH = 100;
static const int INIT_TEST_COUNT = 20;
static const int SIG_TEST_COUNT = 20;

// Manually verify the signature
int manual_test(ECP ec, ECPPoint G, Integer n, ECPPoint Q, Integer r, Integer s, byte *message, unsigned message_length){
  Integer z = m_to_int(message, message_length, n.ByteCount());
  // verify
  if (Q == ec.Identity()){
    cerr << "Q == O" << endl;
    return 1;
  }
  if (!(ec.Multiply(n, Q) == ec.Identity())){
    cerr << "n x Q != O" << endl;
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
  Integer u1 = a_times_b_mod_c(z, w, n);
  Integer u2 = a_times_b_mod_c(r, w, n);
  ECPPoint P2 = ec.Add(ec.Multiply(u1, G), ec.Multiply(u2, Q));

  if (P2.x != r){
    // cerr << "FAIL manual" << endl;
    return 1;
  }
  return 0;
}

// verify the signature using crypto++ verifier
int cryptopp_test(ECPPoint Q, Integer r, Integer s, byte *message, unsigned message_length){
  vector<byte> signature(64);
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

int test(){
  ECPPoint Q; // public key
  byte message[MESSAGE_LENGTH];

  S s;
  B b(s.get_paillier_n(), s.get_paillier_g());

  s.start_init();
  b.start_init();

  s.finish_init(b.get_Qb());
  b.finish_init(s.get_Qs());

  if (!(s.get_Q() == b.get_Q())){
    cerr << "ERR init" << endl;
    return 1;
  }
  Q = s.get_Q();

#ifdef print
  ECPPoint sQ = s.get_Q();
  ECPPoint bQ = b.get_Q();
  cout << "sQ: " << sQ.x << " " << sQ.y << endl;
  cout << "bQ: " << bQ.x << " " << bQ.y << endl;
  cout << (sQ == bQ ? "OK" : "FAIL") << endl;
#endif

  for(int i = 0; i < SIG_TEST_COUNT; ++i){

    common::rng().GenerateBlock(message, MESSAGE_LENGTH);
		b.set_data(message, MESSAGE_LENGTH);

    s.start_sig();

    b.cont_sig(s.get_Ks(), s.get_cs(), message, MESSAGE_LENGTH);

    s.finish_sig(b.get_r(), b.get_cb());

    if (cryptopp_test(Q, s.get_r(), s.get_s(), message, MESSAGE_LENGTH)){
      return 1;
    }

    if (manual_test(
			common::ec_parameters().GetCurve(), 
			common::ec_parameters().GetSubgroupGenerator(),
			common::ec_parameters().GetGroupOrder(),
			Q, s.get_r(), s.get_s(), message, MESSAGE_LENGTH)){
      return 1;
    }
  }
  return 0;
}

int test2(){
	S s;
	B b(s.get_paillier_n(), s.get_paillier_g());

	SharedSignature shared_signature;

	shared_signature.init(&s, &b);
	if (!(s.get_Q() == b.get_Q())){
		cerr << "Q err" << endl;
		return 1;
	}

	for(int i = 0; i < SIG_TEST_COUNT; ++i){

		byte message[MESSAGE_LENGTH];

    common::rng().GenerateBlock(message, MESSAGE_LENGTH);
		
		b.set_data(message, MESSAGE_LENGTH);

		shared_signature.exec(&s, &b);

		if (cryptopp_test(s.get_Q(), s.get_r(), s.get_s(), message, MESSAGE_LENGTH)){
      return 1;
    }

    if (manual_test(
			common::ec_parameters().GetCurve(), 
			common::ec_parameters().GetSubgroupGenerator(),
			common::ec_parameters().GetGroupOrder(),
			s.get_Q(), s.get_r(), s.get_s(), message, MESSAGE_LENGTH)){
      return 1;
    }
	}
	return 0;
}

int test_cheat(){
	S s;
	B b(s.get_paillier_n(), s.get_paillier_g());

	SharedSignature shared_signature;

	shared_signature.init(&s, &b);
	if (!(s.get_Q() == b.get_Q())){
		cerr << "Q err" << endl;
		return 1;
	}

	for(int i = 0; i < SIG_TEST_COUNT; ++i){

		byte message[MESSAGE_LENGTH];

    common::rng().GenerateBlock(message, MESSAGE_LENGTH);
		
		b.set_data(message, MESSAGE_LENGTH);

		shared_signature.exec(&s, &b);

		s.cheat();

		if (!cryptopp_test(s.get_Q(), s.get_r(), s.get_s(), message, MESSAGE_LENGTH)){
      return 1;
    }

    if (!manual_test(
			common::ec_parameters().GetCurve(),
			common::ec_parameters().GetSubgroupGenerator(),
			common::ec_parameters().GetGroupOrder(),
			s.get_Q(), s.get_r(), s.get_s(), message, MESSAGE_LENGTH)){
      return 1;
    }
	}
	return 0;
}



int main(){

#if 0
	cout << "a: " << ec.GetA() << endl;
	cout << "b: " << ec.GetB() << endl;
	cout << "field size " << ec.FieldSize() << endl;
	cout << "G: " << G.x << " " << G.y << endl;
	cout << "n: " << n << endl;
#endif

	for(int i = 0; i < INIT_TEST_COUNT; ++i){
		cout << "init test 3: " << i + 1 << "/" << INIT_TEST_COUNT << endl;
		if (test_cheat()){
			cerr << "3 err" << endl;
			return 1;
		}
	}

	for(int i = 0; i < INIT_TEST_COUNT; ++i){
		cout << "init test: " << i + 1 << "/" << INIT_TEST_COUNT << endl;
		if (test()){
			cerr << "1 err" << endl;
			return 1;
		}
	}

	for(int i = 0; i < INIT_TEST_COUNT; ++i){
		cout << "init test 2: " << i + 1 << "/" << INIT_TEST_COUNT << endl;
		if (test2()){
			cerr << "2 err" << endl;
			return 1;
		}
	}

	cout << "OK" << endl;
	return 0;
}

