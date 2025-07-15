import java.net.*;
import java.io.IOException;

public class EventStreamReader extends Thread {

	public interface EventStreamReaderI {
		public void handleData(String s);
	}
	
	static final String CRLF = "\015\012";
	Socket socket_;
	EventStreamReaderI handler;
	String host;
	String url;
	int port;
	
	public EventStreamReader(EventStreamReaderI handler, String host, int port, String url) {
		this.handler = handler;
		this.host = host;
		this.port = port;
		this.url = url;
	}
	
	public void run() {

		InetSocketAddress address = new InetSocketAddress(host, port);
		try {
			socket_ = new Socket();
			socket_.connect(address, 10000); // 10 second timeout
		} catch (IOException ex) {
			if (handler != null) handler.handleData(ex.getMessage());
		}
		
		if (socket_.isConnected()) {
		
			try {
				String connectionString = 
					"GET " + url + " HTTP/1.0" + CRLF +
					"Connection: Close" + CRLF +
					CRLF +
					CRLF;
				socket_.getOutputStream().write(
					connectionString.getBytes());
			} catch (IOException ex) {
				if (handler != null) handler.handleData(ex.getMessage());
			}
		
			byte buffer[] = new byte[256];
			while (socket_.isConnected()) {
		
				try {
					Thread.sleep(10);
				} catch (Exception ex) {
				}
			
				try {
					int amount = socket_.getInputStream().read(buffer);
					if (amount > 0) {
						String s = new String(buffer, 0, amount);
						if (handler != null) handler.handleData(s);
					}
				} catch (IOException ex) {
					if (handler != null) handler.handleData(ex.getMessage());
					break;
				}
			}
		}
		
		close();
	}
	
	public void close() {
		try {
			socket_.close();
		} catch (IOException ioEx) {
		}
	}
}
