package myhttp;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.Socket;
import java.util.Calendar;

public class StatsThread extends Thread {

	StatsThread(){
	}
	public void run(){
		String webAddress = "fah-web.stanford.edu";	
		String hostHeader = "Host: " + webAddress;
		OutputStream os;
		PrintStream ps;
		InputStream in;
		InputStreamReader in_reader;
		BufferedReader b_reader;
		String str;
		Calendar rightNow;
		try {
			while(true){
				rightNow = Calendar.getInstance();
				 File out_file = new File("stats-" 
						 + rightNow.get(Calendar.YEAR) +"-" + (rightNow.get(Calendar.MONTH)+1) +"-"
						 + rightNow.get(Calendar.DAY_OF_MONTH) +"_"+ rightNow.get(Calendar.HOUR) +"H"
						 + rightNow.get(Calendar.MINUTE) +"M" +rightNow.get(Calendar.MINUTE)+"S"
						 + ".html");
				 System.out.println("Time: " + rightNow.getTime().toString());
				System.out.println("Output file: " + out_file.getName());
				System.out.println("Starting to gather stats");
				Socket socket = new Socket( webAddress, 80 );
				os = socket.getOutputStream();      
				ps = new PrintStream( os, true );
				in = socket.getInputStream();
				in_reader = new InputStreamReader( in );
				b_reader = new BufferedReader( in_reader );
				//			http://fah-web.stanford.edu/
				//			ps.print( "GET /daily_user_summary.txt.bz2 HTTP/1.0\r\n" + hostHeader +"\r\n" + "\r\n" );
				ps.print( "GET /cgi-bin/main.py?qtype=userpage&username=Allen_Humphreys HTTP/1.0\r\n" + hostHeader +"\r\n" + "\r\n" );
				str = b_reader.readLine();
				while (str.length() != 0) {
//					System.out.print(String.valueOf(str.length()));
//					System.out.println(str);
					str = b_reader.readLine();
				}
				while (str.length() == 0) {
//					System.out.print(String.valueOf(str.length()));
//					System.out.println(str);
					str = b_reader.readLine();
				}

				if (out_file.exists()){
					System.out.println("The specified file already exists!");
					System.out.println(out_file);
					System.exit(-1);
				}
				
				try{
					// Create file 
					FileWriter fstream = new FileWriter(out_file);
					BufferedWriter b_writer = new BufferedWriter(fstream);

					int c = b_reader.read();

					while (c != -1) {
						b_writer.write(c);
						c = b_reader.read();
					}

					//Close the output stream
					b_writer.close();
					fstream.close();
					
				}catch (Exception e){//Catch exception if any
					System.err.println("Error: " + e.getMessage());
				}
//				try {
//					Process p=Runtime.getRuntime().exec("bzip2 -fd "+ ClientSocket.out_file);
//					p.waitFor();
//				}
//				catch (InterruptedException fuckup) {
//					// TODO Auto-generated catch block
//					fuckup.printStackTrace();
//				}
				socket.close();
				try {
					System.out.println("Going to sleep for 30 minutes");
					sleep(30*60*1000);
					System.out.println("Waking up");
				}
				catch (InterruptedException fuckup) {
					// TODO Auto-generated catch block
					fuckup.printStackTrace();
				}
			}
		} 
		catch( IOException e ) {
			System.out.println( "Error:   " + e ); 
		}
//60*60*1000
	}
}
