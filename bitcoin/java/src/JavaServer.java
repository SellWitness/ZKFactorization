import org.apache.thrift.server.TServer;
import org.apache.thrift.server.TServer.Args;
import org.apache.thrift.server.TSimpleServer;
import org.apache.thrift.server.TThreadPoolServer;
import org.apache.thrift.transport.TSSLTransportFactory;
import org.apache.thrift.transport.TServerSocket;
import org.apache.thrift.transport.TServerTransport;
import org.apache.thrift.transport.TSSLTransportFactory.TSSLTransportParameters;

import java.util.concurrent.Executors;

import bitcoin_utils.*;

public class JavaServer {

  public static BitcoinUtilsHandler handler;

  public static BitcoinUtils.Processor processor;

  public static void main(String [] args) {
    if (args.length != 2) {
      System.out.println("Expected  arguments app name and port");
      return;
    }
    
    String app_name = args[0];
    final Integer port = new Integer(args[1]);

    try {
      handler = new BitcoinUtilsHandler(app_name);
      processor = new BitcoinUtils.Processor(handler);

      Runnable simple = new Runnable() {
        public void run() {
          simple(processor, port);
        }
      };      

      new Thread(simple).start();
    } catch (Exception x) {
      x.printStackTrace();
    }
  }

  public static void simple(BitcoinUtils.Processor processor, Integer port) {
    try {
      TServerTransport serverTransport = new TServerSocket(port);
      TServer server = new TThreadPoolServer(new TThreadPoolServer.Args(serverTransport).processor(processor).executorService(Executors.newFixedThreadPool(16)));

      System.out.println(server);

      // TServer server = new TSimpleServer(new Args(serverTransport).processor(processor));

      System.out.println("Starting the theadpool server...");
      server.serve();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}
