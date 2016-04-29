#include "timed_commitment.h"
#include "bit_utils.h"

#include <iostream>
#include <chrono>

using namespace CryptoPP;
using namespace std;
using namespace std::chrono;
using namespace timed_commitment;


duration<double> time_span(high_resolution_clock::time_point &end, high_resolution_clock::time_point &start) {
	return duration_cast<duration<double>>(end - start);
}

void test(int K, string m, bool t = false){

	high_resolution_clock::time_point start, end;

  if (t){
    cerr << "--- " << "K : " << K << " ------------" << endl;
  }

 	start = high_resolution_clock::now();
  Commiter c;
  end = high_resolution_clock::now();

  if (t){
    cerr << "init   " << time_span(end, start).count() << endl;
  }

  // cerr << "commit" << endl;
  start = high_resolution_clock::now();
  Commitment com = c.commit(K, BitUtils::string_to_bits(m));

  // cerr << "receiver" << endl;
  Receiver r;
	r.accept_commitment(com);
  
  end = high_resolution_clock::now();

  if (t){
    cerr << "commit " << time_span(end, start).count() << endl;
  }

  // cerr << "open" << endl;
  start = high_resolution_clock::now();
  Integer vp = c.open();

  // cerr << "open" << endl;
  string m2 = BitUtils::bits_to_string(r.open(vp));
  end = high_resolution_clock::now();

  if (m != m2){
    cerr << "FAIL open" << endl;
  } else {
    // cerr << "OK" << endl;
  }

  if (t){
    cerr << "open " << time_span(end, start).count() << endl;
  }
  
  start = high_resolution_clock::now();
  // cerr << "force open" << endl;
  string m3 = BitUtils::bits_to_string(r.force_open());
  if (m != m3){
    cerr << "FAIL force open" << endl;
  } else {
    // cerr << "OK" << endl;
  }
  end = high_resolution_clock::now();

  cerr << "force open " << K << ": " << time_span(end, start).count() << endl;

  if (t){
    cerr << "--------------------" << endl;
  }
}

int test2(int K, string m){

  Commiter c;
  Receiver r;
	commit(&c, &r, 16, BitUtils::string_to_bits(m));

	string m1 = BitUtils::bits_to_string(open_commitment(&c, &r));
	if (m1 != m){
		cerr << "ERR open failed" << endl;
		return 1;
	}

	string m2 = BitUtils::bits_to_string(r.force_open());
	if (m2 != m){
		cerr << "ERR force open failed" << endl;
		return 1;
	}

	return 0;
}

int main(){
  string m = "hello world";
/*
  cerr << "small test" << endl;

  for(int i = 0; i < 10; ++i){
    cerr << i + 1 << "/" << 10 << endl;
    byte m[i+5+1];
    m[i+5] = '\0';
    common::rng().GenerateBlock(m, i+5);
    test(18, string((char *)m));
  }

	cerr << "small test 2" << endl;
	for(int i = 0; i < 10; ++i){
	  cerr << i + 1 << "/" << 10 << endl;
    byte m[i+5+1];
    m[i+5] = '\0';
    common::rng().GenerateBlock(m, i+5);
    test2(18, string((char *)m));
	}
*/
  cerr << "time test" << endl;
  for(int K = 26; K < 31; ++K){
		m += " a little longer";
		for (int i = 0; i < 10; ++i) {
    	test(K, m, false);
		}
  }
}

