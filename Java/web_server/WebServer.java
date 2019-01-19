import java.net.*;
import java.io.*;
import java.util.Arrays;
import java.io.Reader;
import java.lang.StringBuilder;

/* Simple web server. Serves only one client at a time */

public class WebServer {

	public static void main(String[] args) throws IOException {
		int port = 33333;
		ServerSocket listen = null;
		Socket server = null;
		while (true) {
			try {
				System.out.println("\n-------------------------------\n");
				/* ******************************
				 * Set up TCP socket connection 
				 * ******************************/
				listen = new ServerSocket(port);
				listen.setSoTimeout(0); // Infinite timeout
				listen.setReuseAddress(true);
				System.out.println("Waiting for client on port " + 
	               listen.getLocalPort() + "...");

				server = listen.accept();
				System.out.println("Accepted connection from " + server.getRemoteSocketAddress());

				/***************
				 * Read in data 
				 ***************/
				BufferedReader bufReader = 
					new BufferedReader(
						new InputStreamReader(
							server.getInputStream()));

				String line = bufReader.readLine();
				String filename = null;
				if (line != null && line.startsWith("GET")) {
					// First line of http request is "GET <filename> HTTP/1.1\r\n"
					filename = "." + (line.split(" "))[1];
				}
				
				// Print the rest of the request data
				while (line != null && !line.isEmpty()) {
					System.out.println(line);
					line = bufReader.readLine();
				}
				System.out.println("File requested: " + filename);

				/***********************************************
				 * Get contents of file and send http response 
				 ***********************************************/
				DataOutputStream outToClient = new DataOutputStream(server.getOutputStream());
				outToClient.writeUTF("HTTP/1.1 200 OK\r\n\r\n");
				
				if (filename == null) {
					continue;
				}

				FileInputStream fileStream = null;
				try {
					System.out.println("File not found.");
					fileStream = new FileInputStream(filename);
				}
				catch (IOException e) {
					fileStream = new FileInputStream("notfound.html");
				}

				try (BufferedInputStream bufStream = new BufferedInputStream(fileStream)) {	
					byte[] bytes = new byte[500];
					int nread = bufStream.read(bytes, 0, bytes.length);
					
					while (nread != -1) {
						outToClient.write(bytes, 0, nread);
						nread = bufStream.read(bytes, 0, bytes.length);
					}				
					System.out.println("File sent.");
				} 
				catch (IOException e) {
					e.printStackTrace();
				}
			}
			catch (SocketTimeoutException st) {
					System.out.println("Socket timed out!");
			}
			catch (IOException e) {
				e.printStackTrace();
			} 
			finally {
				if (listen != null) listen.close();
				if (server != null) server.close();
			}
		}
	}
}
