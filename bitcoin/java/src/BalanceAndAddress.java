import bitcoin_utils.*;

import org.bitcoinj.core.*;
import org.bitcoinj.params.TestNet3Params;
import org.bitcoinj.kits.WalletAppKit;
import org.bitcoinj.utils.BriefLogFormatter;
import org.bitcoinj.wallet.WalletTransaction ;
import org.bitcoinj.crypto.TransactionSignature;
import org.bitcoinj.script.Script;
import org.bitcoinj.core.Transaction.SigHash;

import org.spongycastle.crypto.params.ECDomainParameters;
import org.spongycastle.math.ec.ECPoint;
import org.spongycastle.math.ec.ECCurve;

import java.io.File;
import java.nio.ByteBuffer;

import org.apache.log4j.*;

import java.util.List;
import java.math.BigInteger;

import org.bitcoinj.core.ECKey.ECDSASignature;

class BalanceAndAddress{
  public static void main(String argv[]) throws Exception {
    printBalanceAndAddress("seller");
    printBalanceAndAddress("buyer");
    printBalanceAndAddress("test");
  }

  private static void printBalanceAndAddress(String name){
    System.out.println("--------------------------------------------");
    System.out.println(name);
    BitcoinUtilsHandler handler = new BitcoinUtilsHandler(name);
    printBalance(handler);
    printAddress(handler);
    System.out.println("--------------------------------------------");
  }

  private static void printBalance(BitcoinUtilsHandler handler) {
    System.out.println("Balance:");
    System.out.println(handler.kit.wallet().getBalance());
    System.out.println("Spendable balance:");
    System.out.println(handler.kit.wallet().getBalance(Wallet.BalanceType.AVAILABLE_SPENDABLE));
  }

  private static void printAddress(BitcoinUtilsHandler handler) {
    Address addr = handler.kit.wallet().freshReceiveAddress();
    System.out.println("Fresh receive address");
    System.out.println(addr);
  }
}
