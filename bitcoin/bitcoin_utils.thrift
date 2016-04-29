namespace cpp bitcoin_utils
namespace java bitcoin_utils

typedef binary Address
typedef binary Transaction
typedef binary IntegerBytes
typedef binary Signature
typedef binary Script
typedef binary Hash

struct IntegerPair {
  1: required IntegerBytes r;
  2: required IntegerBytes s;
}

struct TransactionAndScripts {
  1: required Transaction tx;
  2: required Script original_script;
  3: required Script script;
}

service BitcoinUtils { 
   void ping(),
   void sleep(),
   
   /**
    * Returns a fresh address for the wallet.
    */
   Address freshAddress(),

   /**
    * Creates transaction that sends value satoshis to 
    * address x, y.
    */
   TransactionAndScripts createSend(1:list<IntegerBytes> xs, 2:list<IntegerBytes> ys, 3:i32 value),
   
   /**
    * Creates transaction that spends P2PKH output of transaction tx.
    * The output is for public key x, y. Output of this transaction is
    * to address dst.
    */
   Transaction createSendTransactionToAddress(
     1:Transaction tx, 
     2:list<IntegerBytes> xs, 
     3:list<IntegerBytes> ys, 
     4:Address dst
   ),

   Script getOutputScript(
     1:Transaction tx,
     2:list<IntegerBytes> xs, 
     3:list<IntegerBytes> ys,
   ),

   Hash hashForSignature(
     1:Transaction tx
     2:list<IntegerBytes> xs, 
     3:list<IntegerBytes> ys,
     4:Script redeem_script
   ),

   Transaction getSignedTransaction(
     1:Transaction tx, 
     2:list<IntegerBytes> xs, 
     3:list<IntegerBytes> ys,
     4:list<IntegerBytes> rs,
     5:list<IntegerBytes> ss,
     6:Script redeem_script
     7:Script original_redeem_script
   ),
/*
   bool verifySignature(
     1:Transaction tx, 
     2:list<IntegerBytes> xs, 
     3:list<IntegerBytes> ys,
     4:list<IntegerBytes> rs,
     5:list<IntegerBytes> ss,
     6:Script redeem_script
   ),
 */

   bool cppVerifySignature(
     1:binary data,
     2:list<IntegerBytes> xs, 
     3:list<IntegerBytes> ys,
     4:list<IntegerBytes> rs,
     5:list<IntegerBytes> ss
   ),

   bool broadcastTransaction(
     1:Transaction tx
   ),

   bool verifyTransaction(
     1:Transaction tx,
     2:Script redeem_script
     3:list<IntegerBytes> xs,
     4:list<IntegerBytes> ys,
     5:i32 value
   ),

   bool waitForTransaction(
     1:Transaction tx,
     2:i32 depth,
   ),

   bool waitForTransactionByOutput(
     1:Address addr,
     2:i32 depth,
   ),

   list<IntegerPair> getSignatures(
     1:Address addr,
		 2:i32 R,
   ),

   bool redeemTransaction(
     1:Transaction tx,
     2:Script original_redeem_script
     3:IntegerBytes sk,
     4:IntegerBytes x, 
     5:IntegerBytes y,
		 6:i32 R,
   ),
}
