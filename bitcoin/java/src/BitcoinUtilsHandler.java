import org.apache.thrift.TException;

// Generated code
import bitcoin_utils.*;

import org.bitcoinj.core.*;
import org.bitcoinj.params.TestNet3Params;
import org.bitcoinj.kits.WalletAppKit;
import org.bitcoinj.utils.BriefLogFormatter;
import org.bitcoinj.wallet.WalletTransaction ;
import org.bitcoinj.crypto.TransactionSignature;
import org.bitcoinj.script.Script;
import org.bitcoinj.core.Transaction.SigHash;
import org.bitcoinj.script.ScriptChunk;
import org.bitcoinj.params.RegTestParams;
import org.bitcoinj.script.ScriptBuilder;

import com.google.common.util.concurrent.ListenableFuture;

import org.spongycastle.crypto.params.ECDomainParameters;
import org.spongycastle.math.ec.ECPoint;
import org.spongycastle.math.ec.ECCurve;

import java.io.File;
import java.nio.ByteBuffer;

import org.apache.log4j.*;

import java.util.List;
import java.util.ArrayList;
import java.math.BigInteger;
import java.util.concurrent.TimeUnit;
import java.net.InetAddress;


public class BitcoinUtilsHandler implements BitcoinUtils.Iface {

  private NetworkParameters params;
  public WalletAppKit kit;

  private void printl(){
    System.out.println(
      "----------------------------------------"
    );
  }

  public BitcoinUtilsHandler(String prefix) {
    System.out.println("BitcoinUtilsHandler start init");
    Logger logger = LogManager.getRootLogger();
    logger.setLevel(Level.OFF);

    params = RegTestParams.get();
    kit = new WalletAppKit(params, new File("."), prefix);

    Wallet.SendRequest.DEFAULT_FEE_PER_KB = Coin.valueOf(6000);
    kit.setPeerNodes(new PeerAddress(InetAddress.getLoopbackAddress(), 19000));

    kit.startAsync();
    kit.awaitRunning();
    System.out.println("BitcoinUtilsHandler finish init");
    
    /*
    System.out.println("Balance: " + kit.wallet().getBalance());
    Iterable<WalletTransaction> transactions = kit.wallet().getWalletTransactions();
    for(WalletTransaction t: transactions) {
      System.out.println("-------------------------------------");
      System.out.println("  " + t);
      System.out.println("  " + t.getTransaction());
    }
    */
  }

  final protected static char[] hexArray = "0123456789ABCDEF".toCharArray();
  public static String bytesToHex(byte[] bytes) {
    char[] hexChars = new char[bytes.length * 2];
    for ( int j = 0; j < bytes.length; j++ ) {
      int v = bytes[j] & 0xFF;
      hexChars[j * 2] = hexArray[v >>> 4];
      hexChars[j * 2 + 1] = hexArray[v & 0x0F];
    }
    return new String(hexChars);
  }

  public void ping() {
    System.out.println("ping()");
  }

  public void sleep() {
    try {
      System.out.println("sleep start");
      Thread.sleep(10000);
      System.out.println("sleep end");
    } catch (Exception e) {
      System.out.println(e);
    }
  }

  public void printBalance(){
    System.out.println("Balance: " + kit.wallet().getBalance());
  }

  public void printTransaction(ByteBuffer btx){
    Transaction tx = new Transaction(kit.params(), btx.array());
    System.out.println(tx);
  }

  public void printAddress(ByteBuffer baddr){
    Address addr = new Address(kit.params(), baddr.array());
    System.out.println(addr);
  }

  public ByteBuffer freshAddress() {
    Address freshAddress = kit.wallet().freshReceiveKey().toAddress(params);

    System.out.println("Fresh address:");
    System.out.println(freshAddress);
    System.out.println("Hex bytes:");
    System.out.println(bytesToHex(freshAddress.getHash160()));
    return ByteBuffer.wrap(freshAddress.getHash160());
  }

  private ECKey getECKeyFromXY(ByteBuffer bx, ByteBuffer by) {
    BigInteger ix = new BigInteger(1, bx.array());
    BigInteger iy = new BigInteger(1, by.array());

    // System.out.println(ix);
    // System.out.println(iy);

    ECDomainParameters ec_params = ECKey.CURVE; 
    ECCurve ec_curve = ec_params.getCurve(); 

    ECPoint ec_point = ec_curve.createPoint(ix, iy, true);

    ECKey pk = ECKey.fromPublicOnly(ec_point);

    System.out.println("getECKeyFromXY compressed: " + pk.isCompressed());
    return pk;
  }

  private Address getAddressFromXY(ByteBuffer bx, ByteBuffer by) {
    ECKey pk = getECKeyFromXY(bx, by);
    Address address = pk.toAddress(kit.params());

    return address;
  }

  public TransactionAndScripts createSend(List<ByteBuffer> bxs, List<ByteBuffer> bys, int value) {
    System.out.println("createSend");
		TransactionAndScripts out = new TransactionAndScripts();
    Transaction transaction = new Transaction(kit.params());
		Script original_script;
		Script script;
		int R = bys.size();
    Address address;
    List<ECKey> keys;
		
    Coin coin = Coin.valueOf(value);
    try {
			if (R == 1) {
	 			address = getAddressFromXY(bxs.get(0), bys.get(0));
		   	System.out.println("Address:");
    		System.out.println(address);
 			 	System.out.println("Address hex bytes: " + bytesToHex(address.getHash160()));

      	transaction = kit.wallet().createSend(address, coin);
				script = getOutput(transaction, address).getScriptPubKey();
				original_script = script;
			} else {
				ECKey buyer_key = new ECKey();
				kit.wallet().importKey(buyer_key);
    	  keys = new ArrayList<ECKey>(R);
    		System.out.println("ECKeys used in the multisig:");
				for (int i = 0; i < 2*R-1; ++i) {
					if (i < R) {
	 					keys.add(getECKeyFromXY(bxs.get(i), bys.get(i)));
					} else {
						keys.add(buyer_key);
					}

					if (i > 0) {
  	  			System.out.println();
					}
	    		System.out.println(keys.get(i));
  	  		System.out.println("Key hex bytes: " + bytesToHex(keys.get(i).getPubKey()));
				}

				original_script = ScriptBuilder.createMultiSigOutputScript(R, keys);
				script = ScriptBuilder.createP2SHOutputScript(original_script);
				transaction.addOutput(coin, script);
				Wallet.SendRequest req = Wallet.SendRequest.forTx(transaction);
				kit.wallet().completeTx(req);
      	transaction = req.tx;
			}
      if (transaction == null) {
        System.out.println("Transaction is null");
      }
    } catch (Exception e) {
      System.out.println("Error! Error! " + e.getMessage());
      System.out.println(e.toString());
      return out;
    }
    System.out.println(transaction);
		out.tx = ByteBuffer.wrap(transaction.bitcoinSerialize());
		out.original_script = ByteBuffer.wrap(original_script.getProgram());
		out.script = ByteBuffer.wrap(script.getProgram());
    return out;
  }

  public ByteBuffer createSendTransactionToAddress(ByteBuffer tx, List<ByteBuffer> xs, List<ByteBuffer> ys, ByteBuffer dst) {
    System.out.println("createSendTransactionToAddress");
    Transaction src_tx = new Transaction(kit.params(), tx.array());
    Address dst_addr = new Address(kit.params(), dst.array());

    TransactionOutput output = getTransactionOutputScript(tx, xs, ys);
    if (output == null) {
      return ByteBuffer.wrap(new byte[0]);
    }

    Transaction result = new Transaction(kit.params());
    result.addInput(output);
    result.addOutput(
      output.getValue().subtract(Wallet.SendRequest.DEFAULT_FEE_PER_KB),
      dst_addr
    );

    System.out.println(result);
    return ByteBuffer.wrap(result.bitcoinSerialize());
  }

  private TransactionOutput getOutput(
    Transaction tx,
    Address dst
  ) {

    List<TransactionOutput> outputs = tx.getOutputs();
    TransactionOutput output = null;
    for(TransactionOutput o: outputs) {
			Address receiver = o.getAddressFromP2PKHScript(kit.params());
      if ((receiver == null && dst == null) || (receiver != null && receiver.equals(dst))) {
        output = o;
				break;
      }
    }

    return output;
  }

  private TransactionOutput getTransactionOutputScript(
    ByteBuffer btx, 
    List<ByteBuffer> xs, 
    List<ByteBuffer> ys
  ) {
    Transaction tx = new Transaction(kit.params(), btx.array());
	  Address dst_addr;
		if (xs.size() == 1) {
	    dst_addr = getAddressFromXY(xs.get(0), ys.get(0));
		} else {
	    dst_addr = null;
		}
    return getOutput(tx, dst_addr);
  }

  /* Get the output of T1 */
  public ByteBuffer getOutputScript(
    ByteBuffer btx, 
    List<ByteBuffer> xs, 
    List<ByteBuffer> ys
  ) {
    return ByteBuffer.wrap(getTransactionOutputScript(btx, xs, ys).getScriptBytes());
  }

  public ByteBuffer hashForSignature(
    ByteBuffer btx, 
    List<ByteBuffer> xs, 
    List<ByteBuffer> ys,
    ByteBuffer bredeem_script
  ) {
    printl();
    System.out.println("Hash for signature");
    printl();

    Transaction tx = new Transaction(kit.params(), btx.array());
    System.out.println("Transaction");
    System.out.println(tx);
    printl();

    Script redeem_script = new Script(bredeem_script.array());
    System.out.println("Redeem script");
    System.out.println(redeem_script);
    printl();

    // Address addr = getAddressFromXY(x, y);
    // TransactionOutput output = getOutput(tx, addr);

    Sha256Hash hash = tx.hashForSignature(
      // output.getIndex(),
      0,
      redeem_script,
      SigHash.ALL,
      false
    );
    return ByteBuffer.wrap(hash.getBytes());
  }

  public ByteBuffer getSignedTransaction(
    ByteBuffer btx, 
    List<ByteBuffer> bxs, 
    List<ByteBuffer> bys,
    List<ByteBuffer> brs, 
    List<ByteBuffer> bss,
    ByteBuffer bredeem_script,
    ByteBuffer boriginal_redeem_script
  ) {
    printl();
    System.out.println("getSignedTransaction");
    printl();

    Transaction tx = new Transaction(kit.params(), btx.array());
    System.out.println("Transaction");
    System.out.println(tx);
    printl();

    Script redeem_script = new Script(bredeem_script.array());
    System.out.println("Redeem script");
    System.out.println(redeem_script);
    printl();

    Script input_script;
		int R = brs.size();
		List<TransactionSignature> signatures = new ArrayList<TransactionSignature>();
		for (int i = 0; i < R; ++i) {
	   	BigInteger r = new BigInteger(1, brs.get(i).array());
  	 	BigInteger s = new BigInteger(1, bss.get(i).array());
		  signatures.add(new TransactionSignature(r, s));

  	 	System.out.println("r: " + r);
	    System.out.println("s: " + s);
    	printl();
		}
		if (R == 1) {
  	  ECKey pk = getECKeyFromXY(bxs.get(0), bys.get(0));
    	input_script = new Script(
  	    Script.createInputScript(
	        signatures.get(0).encodeToBitcoin(),
        	pk.getPubKey()
      	)
    	);
		} else {
    	Script original_redeem_script = new Script(boriginal_redeem_script.array());
			input_script = ScriptBuilder.createP2SHMultiSigInputScript(signatures, original_redeem_script);
		}

    System.out.println("input_script");
    System.out.println(input_script);
    System.out.println("/input_script");
    printl();

    tx.getInput(0).setScriptSig(input_script);
    try {
      tx.getInput(0).getScriptSig().correctlySpends(
          tx,
          0,
          redeem_script
          );
    } catch (Exception e) {
      
      System.out.println(e.toString());
      return ByteBuffer.wrap(new byte[0]);
    }
    
    System.out.println("Hip hip! Hurray! (verified signaure)");
    printl();
    return ByteBuffer.wrap(tx.bitcoinSerialize());
  }

  public boolean cppVerifySignature(ByteBuffer data, List<ByteBuffer> xs, List<ByteBuffer> ys, List<ByteBuffer> rs, List<ByteBuffer> ss) {
    System.out.println("Error Error cpp called in java server");
    return false;
  }

  public boolean verifyTransaction(ByteBuffer btx, ByteBuffer bredeem_script, List<ByteBuffer> xs, List<ByteBuffer> ys, int value) {
    System.out.println("verifyTransaction");

    Transaction tx = new Transaction(kit.params(), btx.array());
		TransactionOutput output = null;
		Script redeem_script = new Script(bredeem_script.array());
		for (TransactionOutput o : tx.getOutputs()) {
			if (o.getScriptPubKey().equals(redeem_script)) {
				output = o;
				break;
			}
		}

	  if (output == null) {
    	System.out.println("verifyTransaction no matching output");
      return false;
	  }
    if (output.getValue().getValue() != value) {
      System.out.println("verifyTransaction incorrect value");
      System.out.println(output.getValue().getValue());
      System.out.println(value);
      return false;
    }
 
    System.out.println("verifyTransaction finished");
    return true;
  }

  public boolean broadcastTransaction(ByteBuffer btx) {
    System.out.println("broadcastTransaction");
    Transaction tx = new Transaction(kit.params(), btx.array());
    System.out.println(tx);
    try {
      kit.peerGroup().broadcastTransaction(tx).future().get();
    } catch (Exception e) {
      System.out.println("broadcastTransaction failed");
      System.out.println(e);
      return false;
    }
    System.out.println("broadcastTransaction finished");
    return true;
  }

  private void waitSecond(TransactionConfidence confidence, int depth) {
    try {
      System.out.println(confidence);
      confidence.getDepthFuture(depth).get(1, TimeUnit.SECONDS);
    } catch (Exception e) {
    }
  }

  public boolean waitForTransactionHelper(Transaction tx, int depth) {
    System.out.println("waitForTransactionHelper");
    System.out.println(tx);

    try {
      TransactionConfidence confidence = tx.getConfidence();
      for(int i = 0; i < 3600 && confidence.getDepthInBlocks() < depth; ++i) {
        System.out.println(i);
        confidence = tx.getConfidence();
        waitSecond(confidence, depth);
      }
      System.out.println(confidence);
      confidence.getDepthFuture(depth).get(1, TimeUnit.SECONDS);
    } catch (Exception e) {
      System.out.println("waitForTransactionHelper failed");
      System.out.println(e);
      return false;
    }
    System.out.println("waitForTransactionHelper finished");
    return true;
  }
 
  public boolean waitForTransaction(ByteBuffer btx, int depth) {
    System.out.println("waitForTransaction");
    Transaction tx = new Transaction(kit.params(), btx.array());

    for(TransactionOutput o: tx.getOutputs()) {
      Address address = o.getAddressFromP2PKHScript(kit.params());
			if (address != null) {
    	  kit.wallet().addWatchedAddress(address);
  	    System.out.println("Adding watched address");
	      System.out.println(address);
			}
    }

    return waitForTransactionHelper(tx, depth);
  }

  private Transaction findTransactionByOutput(Address addr) {

    Transaction tx = null;
    Iterable<WalletTransaction> txs = kit.wallet().getWalletTransactions();
    for(WalletTransaction wtx: txs) {
      TransactionOutput o = getOutput(wtx.getTransaction(), addr);
      if (o != null) {
        tx = wtx.getTransaction();
      }
    }
    return tx;
  }

  public boolean waitForTransactionByOutput(ByteBuffer baddr, int depth) {
    System.out.println("waitForTransactionByOutput");
    Address addr = new Address(kit.params(), baddr.array());

    System.out.println("Adding watched address");
    System.out.println(addr);
    kit.wallet().addWatchedAddress(addr);

    Transaction tx = null;
    
    try{
      while(tx == null){
        Thread.sleep(1000);
        System.out.println("Searching for transaction");
        tx = findTransactionByOutput(addr); 
     }
      
      System.out.println("Found transaction");
      System.out.println(tx);

      return waitForTransactionHelper(tx, depth);
    }
    catch (Exception e) {
      System.out.println(e);
      return false;
    }
  }

  public List<IntegerPair> getSignatures(ByteBuffer baddr, int R){
    System.out.println("get signature");
    Address addr = new Address(kit.params(), baddr.array());
    System.out.println(addr);
    Transaction tx = findTransactionByOutput(addr);
    printl();
    System.out.println(tx);
    printl();
    TransactionInput input = tx.getInput(0);
    Script input_script = input.getScriptSig();
    List<ScriptChunk> chunks = input_script.getChunks();
    ArrayList<ECKey.ECDSASignature> signatures = new ArrayList<ECKey.ECDSASignature>();
    ArrayList<IntegerPair> bsignatures = new ArrayList<IntegerPair>();
		if (R == 1) {
	    signatures.add(ECKey.ECDSASignature.decodeFromDER(chunks.get(0).data));
		} else {
			for (int i = 0; i < R; ++i) {
	    	signatures.add(ECKey.ECDSASignature.decodeFromDER(chunks.get(i+1).data));
			}
		}
  	System.out.println("-- r s");
		for (int i = 0; i < R; ++i) {
			if (i > 0) {
	  	  System.out.println();
			}
    	System.out.println(signatures.get(i).r);
  	  System.out.println(signatures.get(i).s);
	    bsignatures.add(new IntegerPair(
    	  ByteBuffer.wrap(signatures.get(i).r.toByteArray()),
  	    ByteBuffer.wrap(signatures.get(i).s.toByteArray())
	    ));
		}
		return bsignatures;
  }

  public boolean redeemTransaction(ByteBuffer btx, ByteBuffer boriginal_redeem_script, ByteBuffer bsk, ByteBuffer bx, ByteBuffer by, int R) {
    try {
      printl();
      printl();
      printl();
      System.out.println("Redeem transaction");
      Transaction tx_src = new Transaction(kit.params(), btx.array());
      printl();
      System.out.println("tx src:");
      System.out.println(tx_src);
      printl();

      BigInteger sk = new BigInteger(1, bsk.array());
      printl();
      System.out.println("sk");
      System.out.println(sk);
      printl();
      ECKey key = ECKey.fromPrivate(sk, true);
      printl();
      System.out.println("key");
      System.out.println(key);
      printl();

      Address addr =  kit.wallet().freshReceiveAddress();
	    Address output_addr;
      Transaction tx = new Transaction(kit.params());

			if (R == 1) {
	      output_addr = getAddressFromXY(bx, by);
      	System.out.println("address");
    	  System.out.println(output_addr);
			} else {
	      output_addr = null;
			}
 	    TransactionOutput output = getOutput(tx_src, output_addr);
		  Coin value = output.getValue();
  	  value = value.subtract(Wallet.SendRequest.DEFAULT_FEE_PER_KB);
  		tx.addOutput(value, addr);

			if (R == 1) {
	      tx.addSignedInput(output, key);
			} else {
    		Script original_redeem_script = new Script(boriginal_redeem_script.array());
				ECKey buyer_key = kit.wallet().findKeyFromPubKey(original_redeem_script.getChunks().get(2*R-1).data);
      	System.out.println("buyer key:");
      	System.out.println(buyer_key);

				tx.addInput(output);
				List<TransactionSignature> signatures = new ArrayList<TransactionSignature>();
    		Sha256Hash hash = tx.hashForSignature(0, original_redeem_script, SigHash.ALL, false);
				signatures.add(new TransactionSignature(key.sign(hash), SigHash.ALL, false));
				for (int i = 0; i < R-1; ++i) {
					signatures.add(new TransactionSignature(buyer_key.sign(hash), SigHash.ALL, false));
				}
				Script input_script = ScriptBuilder.createP2SHMultiSigInputScript(signatures, original_redeem_script);

    		tx.getInput(0).setScriptSig(input_script);
			}

      printl();
      System.out.println("tx");
      System.out.println(tx);
      printl();

      tx.getInput(0).verify(output);
      kit.peerGroup().broadcastTransaction(tx).future().get();

      printl();
      printl();
      printl();
      return waitForTransactionHelper(tx, 6); // TODO depth in param
    } catch (Exception e) {
      System.out.println("redeem transaction: broadcastTransaction failed");
      System.out.println(e);
      return false;
    }
  }
}
