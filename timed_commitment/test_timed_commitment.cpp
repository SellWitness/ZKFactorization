
#include "timed_commitment.h"
#include "bit_utils.h"

#include <iostream>
#include <ctime>

using namespace CryptoPP;
using namespace std;
using namespace timed_commitment;

void test(int K, string m, bool t = false){

  time_t start, end; 

  if (t){
    cerr << "--- " << "K : " << K << " ------------" << endl;
  }

  start = time(NULL);
  Commiter c;
  end = time(NULL);

  if (t){
    cerr << "init   " << difftime(end, start) << endl;
  }

  // cerr << "commit" << endl;
  start = time(NULL);
  Commitment com = c.commit(K, BitUtils::string_to_bits(m));

  // cerr << "receiver" << endl;
  Receiver r;
	r.accept_commitment(com);
  
  // cerr << "zk" << endl;
  for(int i = 0; i < 10; ++i){
    vector<RegularCommitment> commits = r.zk_1();
    vIvI zw = c.zk_2(commits);
    vI commit_values = r.zk_3(zw);
    vI y = c.zk_4(commit_values);
    r.zk_5(y);
  }
  end = time(NULL);

  if (t){
    cerr << "commit " << difftime(end, start) << endl;
  }

  // cerr << "open" << endl;
  Integer vp = c.open();

  // cerr << "open" << endl;
  string m2 = BitUtils::bits_to_string(r.open(vp));

  if (m != m2){
    cerr << "FAIL open" << endl;
  } else {
    // cerr << "OK" << endl;
  }
  
  start = time(NULL);
  // cerr << "force open" << endl;
  string m3 = BitUtils::bits_to_string(r.force_open());
  if (m != m3){
    cerr << "FAIL force open" << endl;
  } else {
    // cerr << "OK" << endl;
  }
  end = time(NULL);

  if (t){
    cerr << "force open " << difftime(end, start) << endl;
  }

  start = time(NULL);
  // cerr << "force open smart" << endl;
  string m4 = BitUtils::bits_to_string(r.force_open_smart());
  if (m != m4){
    cerr << "FAIL force open smart" << endl;
  } else {
    // cerr << "OK" << endl;
  }

  end = time(NULL);

  if (t){
    cerr << "force open smart" << difftime(end, start) << endl;
  }

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

	string m3 = BitUtils::bits_to_string(r.force_open_smart());
	if (m3 != m){
		cerr << "ERR force open smart failed" << endl;
		return 1;
	}

	return 0;
}

int main(){
  string m = "hello world";

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

  cerr << "time test" << endl;
  for(int K = 18; K < 21; ++K){
    string m = "hello world";
    test(K, m, true);
  }
}

