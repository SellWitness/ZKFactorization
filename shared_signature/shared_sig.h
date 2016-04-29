#ifndef _SHARED_SIG_H_
#define _SHARED_SIG_H_


#include "paillier.h"
#include "../protocol.h"
#include "../common/common.h"

#include <cryptopp/ecp.h>
#include <cryptopp/oids.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>

namespace shared_signature {
	// hash message
	// take ret_byte_count leftmost bytes
	// convert to Integer
	CryptoPP::Integer m_to_int(byte *m, unsigned m_len, unsigned ret_byte_count);

	CryptoPP::Integer hash_m_to_int(byte *m, unsigned m_len, unsigned ret_byte_count);
  
  std::vector<byte> encode_signature(CryptoPP::Integer r, CryptoPP::Integer s);

  bool verify(CryptoPP::ECPPoint Q, byte *, unsigned len, CryptoPP::Integer r, CryptoPP::Integer s);

  class S {
		private:
			CryptoPP::ECP ec;
			CryptoPP::ECPPoint G; // subgroup generator - base point
			CryptoPP::Integer n;  // subgroup order

			std::vector<byte> data;

			CryptoPP::Integer ds;
			CryptoPP::Integer ks;

			CryptoPP::ECPPoint Qs;
			CryptoPP::ECPPoint Qb;

			CryptoPP::ECPPoint Q;

			CryptoPP::ECPPoint Ks;

			Paillier paillier;

			CryptoPP::Integer cs;

			CryptoPP::Integer r;
			CryptoPP::Integer s;

		public:

			S();

			byte *get_data(){
				return data.data();
			}

			unsigned get_data_length(){
        if (data.size() == 0) {
          throw ProtocolException("Data is empty!");
        }
				return data.size();
			}	

			void set_data(byte *data, unsigned length){
				this->data.resize(length);
				copy(data, data + length, this->data.begin());
			}

			CryptoPP::Integer get_ds(){
				return ds;
			}

			CryptoPP::ECPPoint get_Qs(){
				return Qs;
			}

			CryptoPP::ECPPoint get_Qb(){
				return Qb;
			}

			CryptoPP::ECPPoint get_Q(){
				return Q;
			}

			CryptoPP::Integer get_paillier_n(){
				return paillier.get_n();
			}

			CryptoPP::Integer get_paillier_g(){
				return paillier.get_g();
			}

			CryptoPP::ECPPoint get_Ks(){
				return Ks;
			}

			CryptoPP::Integer get_cs(){
				return cs;
			}

			CryptoPP::Integer get_r(){
				return r;
			}

			CryptoPP::Integer get_s(){
				return s;
			}
			
			std::vector<byte> get_signature(){
        return encode_signature(r, s);
      }

			void cheat(){
				r = CryptoPP::Integer(common::rng(), 1, n);
				s = CryptoPP::Integer(common::rng(), 1, n);
			}

			void start_init();

			void finish_init(CryptoPP::ECPPoint Qb);

			void start_sig();

			void finish_sig(CryptoPP::Integer r, CryptoPP::Integer cb);


	};

	class B {
		private:
			CryptoPP::ECP ec;
			CryptoPP::ECPPoint G; // subgroup generator - base point
			CryptoPP::Integer n;  // subgroup order

			CryptoPP::Integer db;
			CryptoPP::Integer kb;

			CryptoPP::ECPPoint Qs;
			CryptoPP::ECPPoint Qb;

			CryptoPP::ECPPoint Q;

			Paillier paillier;

			CryptoPP::ECPPoint K;
			CryptoPP::Integer r;
			CryptoPP::Integer cb;

			std::vector<byte> data;
			
			bool open_verified;
		public:
			B(){
			}

			B(CryptoPP::Integer paillier_n,
				CryptoPP::Integer paillier_g);

      CryptoPP::Integer get_n(){
        return n;
      }

			CryptoPP::ECPPoint get_Qs(){
				return Qs;
			}

			CryptoPP::ECPPoint get_Qb(){
				return Qb;
			}

			CryptoPP::ECPPoint get_Q(){
				return Q;
			}

			CryptoPP::Integer get_db(){
				return db;
			} 

			CryptoPP::Integer get_r(){
				return r;
			} 

			CryptoPP::Integer get_cb(){
				return cb;
			}

			byte *get_data(){
				return data.data();
			}

			unsigned get_data_length(){
				return data.size();
			}	

			void set_data(byte *data, unsigned length){
				this->data.resize(length);
				copy(data, data + length, this->data.begin());
			}

			void setOpenVerified(bool val){
				open_verified = val;
			}

			bool getOpenVerified(){
				return open_verified;
			}

			void start_init();

			void finish_init(CryptoPP::ECPPoint Qs);

			void cont_sig(CryptoPP::ECPPoint Ks, CryptoPP::Integer cs, byte *m, unsigned m_len);
	};

	class SharedSignature : public Protocol<S, B> {
		public:
			void init(S *, B *);
			void exec(S *, B *);
			void open(S *, B *);
	};
}

#endif
