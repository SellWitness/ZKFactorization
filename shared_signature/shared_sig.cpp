#include <cryptopp/ecp.h>
#include <cryptopp/oids.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>

#include <iostream>

#include "shared_sig.h"

#include "paillier.h"

using namespace CryptoPP;

using namespace std;

namespace shared_signature {

	Integer hash_m_to_int(byte *m, unsigned m_len, unsigned ret_byte_count){
		SHA256 sha256;
    if (m_len != sha256.DigestSize()) {
      throw ProtocolException("m_len != sha digest size");
    }
    return Integer(m, ret_byte_count);
	}

	Integer m_to_int(byte *m, unsigned m_len, unsigned ret_byte_count){
		SHA256 sha256;
		byte h[sha256.DigestSize()];
		SHA256().CalculateDigest(h, m, m_len);
		return Integer (h, ret_byte_count);
	}

  bool verify(CryptoPP::ECPPoint Q, byte *message, unsigned message_length, CryptoPP::Integer r, CryptoPP::Integer s){
		auto ec = common::ec_parameters().GetCurve();
	  auto G = common::ec_parameters().GetSubgroupGenerator();
	  auto n = common::ec_parameters().GetGroupOrder();

    Integer z = hash_m_to_int(message, message_length, n.ByteCount());
    // verify
    if (Q == ec.Identity()){
      cerr << "Q == O" << endl;
      return false;
    }
    if (!(ec.Multiply(n, Q) == ec.Identity())){
      cerr << "n x Q != O" << endl;
      return false;
    }
    if (r <= 0 || r >= n){
      cerr << "incorrect r" << endl;
      return false;
    } 
    if (s <= 0 || s >= n){
      cerr << "incorrect s" << endl;
      return false;
    } 
    Integer w = s.InverseMod(n);
    Integer u1 = a_times_b_mod_c(z, w, n);
    Integer u2 = a_times_b_mod_c(r, w, n);
    ECPPoint P2 = ec.Add(ec.Multiply(u1, G), ec.Multiply(u2, Q));

    if (P2.x != r){
      cerr << "P2.x != r" << endl;
      return false;
    }
    return true;
  }

  std::vector<byte> encode_signature(CryptoPP::Integer r, CryptoPP::Integer s) {
    std::vector<byte> signature;
    signature.resize(64);

    r.Encode(signature.data(), 32);
    s.Encode(signature.data() + 32, 32);

    return signature;
  }

	/**************************
	 * S
	 **************************/
	S::S():
		ec(common::ec_parameters().GetCurve()),
		G(common::ec_parameters().GetSubgroupGenerator()),
		n(common::ec_parameters().GetGroupOrder()), 
		paillier(2*n.BitCount() + 3) {
		// check that paillier has good bit length
		if (paillier.get_n() <= 2*n*n*n*n){
			cerr << "too short paillier" << endl;
			cerr << "S ctor" << endl
				<< "n      " << paillier.get_n().BitCount() << endl
				<< "2*p**4 " << (2*n*n*n*n).BitCount() << endl;
		}
		// TODO throw
	}

	void S::start_init(){
		ds.Randomize(common::rng(), Integer::One(), n-1);
		Qs = ec.Multiply(ds, G);
	}

	void S::finish_init(ECPPoint Qb){
		this->Qb = Qb;
		this->Q = ec.Multiply(ds, Qb);
	}

	void S::start_sig(){
		ks.Randomize(common::rng(), Integer::One(), n-1);
		Ks = ec.Multiply(ks, G);
		cs = paillier.enc(ds);
	}

	void S::finish_sig(Integer r, Integer cb){
		this->r = r;
		Integer s2 = paillier.dec(cb) % n;
		s = a_times_b_mod_c(ks.InverseMod(n), s2, n);
		if (s == 0){
			throw ProtocolException("ERR s==0 restart protocol");
		}
    if (s > n - s) {
      s = n - s;
    }

		// bool result = verifier.VerifyMessage(get_data(), get_data_length(), signature, 64);
		bool result = verify(Q, get_data(), get_data_length(), r, s);
		if (!result){
			throw ProtocolException("Invalid signature generated!");
		} else {
    }
	}

	/**************************
	 * B
	 **************************/

	B::B(	Integer paillier_n, 
				Integer paillier_g): 
		ec(common::ec_parameters().GetCurve()),
		G(common::ec_parameters().GetSubgroupGenerator()),
		n(common::ec_parameters().GetGroupOrder()),
		paillier(paillier_n, paillier_g) {

			if (paillier.get_n() <= 2*n*n*n*n)
				throw ProtocolException("too short paillier");
		}

	void B::start_init(){
		db.Randomize(common::rng(), Integer::One(), n-1);
		Qb = ec.Multiply(db, G);
	}

	void B::finish_init(ECPPoint Qs){
		this->Qs = Qs;
		this->Q = ec.Multiply(db, Qs);
	}

	void B::cont_sig(ECPPoint Ks, Integer cs, byte *m, unsigned m_len){
		kb.Randomize(common::rng(), Integer::One(), n-1);
		K = ec.Multiply(kb, Ks);
		r = K.x % n;
		if (r == 0)
			throw ProtocolException("r == 0");

		Integer hi = hash_m_to_int(m, m_len, n.ByteCount());
		Integer c1 = paillier.enc( a_times_b_mod_c(kb.InverseMod(n), hi, n) );
		Integer t = a_times_b_mod_c(a_times_b_mod_c(kb.InverseMod(n), r, n), db, n);
		Integer c2 = a_times_b_mod_c(c1, a_exp_b_mod_c(cs, t, paillier.get_n2()), paillier.get_n2());
		Integer u(common::rng(), Integer::Zero(), n*n - 1);
		cb = a_times_b_mod_c(c2, paillier.enc(u*n), paillier.get_n2());
	}

	void SharedSignature::init(S *s, B *b) {
		s->start_init();
		b->start_init();

		s->finish_init(b->get_Qb());
		b->finish_init(s->get_Qs());

		if (!(s->get_Q() == b->get_Q())){
			throw ProtocolException("Mismatching Q");
		}
	}

	void SharedSignature::exec(S *s, B *b) {
		s->start_sig();

		b->cont_sig(s->get_Ks(), s->get_cs(), b->get_data(), b->get_data_length());

		s->finish_sig(b->get_r(), b->get_cb());
	}

	void SharedSignature::open(S *s, B *b) {

		vector<byte> signature = s->get_signature();

		bool result = verify(b->get_Q(), b->get_data(), b->get_data_length(), s->get_r(), s->get_s());
		b->setOpenVerified(result);
	}
}
