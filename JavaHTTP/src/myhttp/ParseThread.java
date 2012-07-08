package myhttp;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

public class ParseThread extends Thread{


	ParseThread(){

	}

	public void run(){
		while (true){
			try {
				this.wait();
			}
			catch (InterruptedException fuckup1) {
				fuckup1.printStackTrace();
			}
			if (!ClientSocket.in_file.exists()) {
				System.err.println("The stats text file hasn't been created");
				System.exit(-1);
			}

			try {
				FileReader inreader = new FileReader(ClientSocket.in_file);
				BufferedReader b_inreader = new BufferedReader(inreader);
				String line = b_inreader.readLine();
				System.out.println(line);
				while (!line.contains("Allen_Humphreys")){
					line = b_inreader.readLine();
				}
				System.out.println(line);
				b_inreader.close();
			}
			catch (FileNotFoundException fuckup) {
				System.out.println("HERE");
				fuckup.printStackTrace();
			}
			catch(IOException e){
				System.out.println("HERE2");
			}
			
		}
	}
}

