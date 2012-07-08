package myhttp;

import java.io.*;
import java.util.*;


public class ClientSocket {
	static Calendar rightNow = Calendar.getInstance();
//	static File out_file = new File("stats-" + rightNow.get(Calendar.YEAR) + (rightNow.get(Calendar.MONTH)+1) + rightNow.get(Calendar.DAY_OF_MONTH) + rightNow.get(Calendar.HOUR) + rightNow.get(Calendar.MINUTE) +"Second:" +rightNow.get(Calendar.MINUTE)+ ".html");
	static File in_file = new File("stats-" + rightNow.get(Calendar.YEAR) + (rightNow.get(Calendar.MONTH)+1) + rightNow.get(Calendar.DAY_OF_MONTH) + ".txt");
	static StatsThread T1= new StatsThread();
	public static void main( String[] args ) throws Exception {
		  
	//	ParseThread T2 = new ParseThread();
		T1.start();
		
//		prompt();
	}
	public static void prompt(){
		ArrayList<String> command = new ArrayList<String>();
		String word = "";
		boolean newline = false;

		//map setup

		while (true){
			System.out.print( "> ");
			word = ""; 
			command.clear();

			newline = false;
			while (newline == false ) {
				word = ""; //clear the temporary string so as not to accumulate words
				//get the command and split into words
				try {
					int ch;
					while ( ( ch = System.in.read() ) == ' ' );            //ignore leading spaces

					if ( ch == '\n' || ch == '\r') {                                   //(F)
						newline = true;                                   //no words
						break;//break out of the try/catch
					}
					word += (char) ch;                                    //add first letter
					while ( ( ch = System.in.read() ) != ' ' && ch != '\n' && ch != '\r' ){               //(J)
						word += (char) ch;                                //get the rest of the letters
					}
					command.add(word);
					if ( ch == '\r' || ch == '\n' ){
						newline = true;
					}
				} catch( IOException e ) {}
			}
			doCommand(command);
		
			
		}//end shell
	} //end main
	
	private static void doCommand(ArrayList<String> command){

		//******* ONE WORD COMMANDS ****//
		if (command.size() == 1) {
			if (command.get(0).equalsIgnoreCase("wakeup") ) {
//				printHelp();
				
				
			} else if (command.get(0).equals("n") || 
					command.get(0).equals("s") || 
					command.get(0).equals("e") || 
					command.get(0).equals("w")) 
			{
//				move(command.get(0));
			} else if (command.get(0).equals("i") || command.get(0).equals("inventory")) {
//				printInventory();
			} else {
				System.out.println("Command with 1 word:\n\t\"" + command.get(0) + "\" is an invalid command!");
			}
		} 
		//******* TWO WORD COMMANDS ****//
		else if (command.size() == 2){
			if (command.get(0).equals("open")){
				//open exit
//				openObject(command.get(1));
			} else if (command.get(0).equals("take")){
//				takeItem(command.get(1));
			} else if (command.get(0).equals("read")){
//				readItem(command.get(1));
			} else if (command.get(0).equals("drop")){
//				dropItem(command.get(1));
			} else if (command.get(0).equals("Delete")){  /*{2}*/
//				Delete (command.get(1));

			} else if (command.get(0).equals("Game") &&  /*{4}*/
					command.get(1).equals("Over")) {
//				endGame();
			} 
			else {
				System.out.println("Command with 2 words:\n\t\"" + command.get(0) + " " 
						+ command.get(1) + "\" is not a valid command!");
			}
		} 
		//******* THREE WORD COMMANDS ****//
		else if (command.size() == 3) {
			if (command.get(0).equals("turn") && command.get(1).equals("on")) {
//				turnOn(command.get(2));
			} else {
				System.out.println("Command with 3 words:\n\t" + command.get(0) + " "
						+ command.get(1) + " " + command.get(2) + "\" is not a valid command!");
			}
		} 
		//******* FOUR WORD COMMANDS ****//
		else if (command.size() == 4) {
			if (command.get(0).equals("attack") && command.get(2).equals("with")) {
//				attack(command.get(1), command.get(3));
			} else if (command.get(0).equals("put") && command.get(2).equals("in")) {
//				putItem(command.get(1), command.get(3));//put (item) in (container)
			} else if (command.get(0).equals("Add") && command.get(2).equals("to")){ /*{1}*/
				//Add (object) to (room/container)  (non user command)
//				Add(command.get(1), command.get(3));
			} else if (command.get(0).equals("Update") && command.get(2).equals("to")){ /*{3}*/
//				Update(command.get(1), command.get(3));
			}
			else {

				System.out.println("Command with 4 words:\n\t\"" + command.get(0) + " "
						+ command.get(1) + " " + command.get(2) + " " + command.get(3) + "\" is not a valid command!");
			}
		} else {
			if (command.size() == 0) {
				System.out.println("Please enter between 1 and 4 words. Type \"help\" for a list of commands.");
			} else if (command.size() > 4) {
				System.out.println("This command is invalid.\nToo many words!");
			} else {
				System.out.println("WHAT?! I'm not equipped for this.");
			}
		}//end parse by command size
		
	}
}

