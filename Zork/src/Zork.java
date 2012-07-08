import java.io.IOException;
import java.util.ArrayList;

public class Zork {
	private static MyGameMap gamemap = new MyGameMap();
	private static boolean newline = false;
	private static Room curr_room;
	private static ArrayList<String> inventory = new ArrayList<String>();

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			if (args.length > 0){
				new XMLParser(args[0], gamemap);
			}else{
				System.err.println("Please specify an XML game file!");
				System.exit(-1);
			}
		}
		catch (Exception e) { }


		//verify all containers, items, creatures that are in rooms, containers have a valid definition in the gamemap
		// TODO verify the verifier
		//gamemap.print();


		Shell();
	}

	static boolean shellt = true;

	private static void Shell(){
		ArrayList<String> command = new ArrayList<String>();
		String word = "";

		//map setup
		curr_room = gamemap.getRoom("Entrance");
		pp(curr_room.getDescription());

		while (shellt){
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

			if (checkAllTriggers(command)){ //true if something triggered, false otherwise

			} else {
				doCommand(command);
			}

		}//end shell
	} //end main



	private static boolean checkAllTriggers(ArrayList<String> command)
	{//present room, objects in present room, objects in containers in present room, inventory
		//true if anything triggers, false otherwise
		boolean rval = false;
		ArrayList<String> chklist = new ArrayList<String>();

		Trigger t = new Trigger();
		//check all triggers associated with the current room
		if (  (t = curr_room.checkTrigger(unchop(command), gamemap, inventory))   != null){
			t.use();
			System.out.println(t.prints);
			rval = true;
		}

		//items
		chklist = curr_room.getAllItems();
		for (int i = 0; i < chklist.size(); i++){
			//			System.out.println("Checking item: " + chklist.get(i));
			if ( (t = gamemap.items.get(chklist.get(i)).checkTrigger(null, gamemap, inventory)) != null){
				t.use();
				System.out.println(t.prints);
				rval = true;
			}
		}
		//creatures
		chklist.clear();
		chklist = curr_room.getAllCreatures();
		for (int i = 0; i < chklist.size(); i++){
			//			System.out.println("Checking creature: " + chklist.get(i));
			if ( (t = gamemap.creatures.get(chklist.get(i)).checkTrigger(null, gamemap, inventory)) != null){
				t.use();
				System.out.println(t.prints);
				rval = true;
			}
		}
		//containers
		chklist.clear();
		chklist = curr_room.getAllContainers();

		for (int i = 0; i < chklist.size(); i++){

			if ( (t = gamemap.containers.get(chklist.get(i)).checkTrigger(null, gamemap, inventory)) != null){
				t.use();
				System.out.println(t.prints);
				for (int j = 0; j < t.actions.size(); j++){
					doCommand(chopchop(t.actions.get(j)));
				}
				rval = true;
			}
		}

		//items,containers,creatures in containers
		//inventory
		//		if (temp == null){
		//			return true;
		//		}else {
		//			//doCommand(chopchop(temp.actions));
		//			System.out.println(temp.prints);
		//			return false;
		//		}


		return rval;
	}


	private static void doCommand(ArrayList<String> command){

		//******* ONE WORD COMMANDS ****//
		if (command.size() == 1) {
			if (command.get(0).equalsIgnoreCase("help") ) {
				printHelp();
			} else if (command.get(0).equals("n") || 
					command.get(0).equals("s") || 
					command.get(0).equals("e") || 
					command.get(0).equals("w")) 
			{
				move(command.get(0));
			} else if (command.get(0).equals("i") || command.get(0).equals("inventory")) {
				printInventory();
			} else {
				System.out.println("Command with 1 word:\n\t\"" + command.get(0) + "\" is an invalid command!");
			}
		} 
		//******* TWO WORD COMMANDS ****//
		else if (command.size() == 2){
			if (command.get(0).equals("open")){
				//open exit
				openObject(command.get(1));
			} else if (command.get(0).equals("take")){
				takeItem(command.get(1));
			} else if (command.get(0).equals("read")){
				readItem(command.get(1));
			} else if (command.get(0).equals("drop")){
				dropItem(command.get(1));
			} else if (command.get(0).equals("Delete")){  /*{2}*/
				Delete (command.get(1));

			} else if (command.get(0).equals("Game") &&  /*{4}*/
					command.get(1).equals("Over")) {
				endGame();
			} 
			else {
				System.out.println("Command with 2 words:\n\t\"" + command.get(0) + " " 
						+ command.get(1) + "\" is not a valid command!");
			}
		} 
		//******* THREE WORD COMMANDS ****//
		else if (command.size() == 3) {
			if (command.get(0).equals("turn") && command.get(1).equals("on")) {
				turnOn(command.get(2));
			} else {
				System.out.println("Command with 3 words:\n\t" + command.get(0) + " "
						+ command.get(1) + " " + command.get(2) + "\" is not a valid command!");
			}
		} 
		//******* FOUR WORD COMMANDS ****//
		else if (command.size() == 4) {
			if (command.get(0).equals("attack") && command.get(2).equals("with")) {
				attack(command.get(1), command.get(3));
			} else if (command.get(0).equals("put") && command.get(2).equals("in")) {
				putItem(command.get(1), command.get(3));//put (item) in (container)
			} else if (command.get(0).equals("Add") && command.get(2).equals("to")){ /*{1}*/
				//Add (object) to (room/container)  (non user command)
				Add(command.get(1), command.get(3));
			} else if (command.get(0).equals("Update") && command.get(2).equals("to")){ /*{3}*/
				Update(command.get(1), command.get(3));
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
		checkAllTriggers(null);
	}


	private static void attack(String creature, String item)
	{
		if (!playerHas(item)){
			System.out.println("You cannot attack a " + creature + " with a " + item + " if you don't have an " + item);
		} else if (!curr_room.hasCreature(creature)){
			System.out.println("There is no " +creature + " to attack.");
		} else {
			//do the attack
			if (gamemap.getCreature(creature).attack(gamemap.getItem(item))){
				for (int i = 0; i < gamemap.getCreature(creature).getPrints().size(); i++){
					pp(gamemap.getCreature(creature).getPrints().get(i));
				}
				for (int i = 0; i < gamemap.getCreature(creature).getActions().size(); i++){
					doCommand(chopchop(gamemap.getCreature(creature).getActions().get(i)));
				}
			}else {
				System.out.println("Attack fails!");
			}
		}

	}



	private static void openObject(String container) {
		if (container.equalsIgnoreCase("exit")){
			if (curr_room.getType().equalsIgnoreCase("exit")){
				endGame();
			} else {
				System.out.println("This room is not an exit!");
			}
		}

		else {
			//if the player has the container or the container is in the room we can open it
			if (playerHas(container) || curr_room.hasContainer(container)){
				gamemap.getContainer(container).open();

				for (int i = 0; i < gamemap.getContainer(container).getItems().size();i++){
					curr_room.addItem(gamemap.getContainer(container).getItems().get(i));
				}
				gamemap.getContainer(container).clearItems();
			}
		}
	}



	private static void Update(String item, String status){
		if (gamemap.getItem(item) != null){
			gamemap.getItem(item).setStatus(status);
		} else if(gamemap.getContainer(item) != null){
			gamemap.getContainer(item).setStatus(status);
		} else if(gamemap.getCreature(item) != null){
			gamemap.getCreature(item).setStatus(status);
		} else{
			System.out.println("There is no " + item + " to update");
		}
	}



	private static void Add(String string, String string2)
	{
		
		if (gamemap.hasRoom(string2)){
			
			gamemap.getRoom(string2).addItem(string);
			
			if (!gamemap.hasItem(string2)){
				gamemap.addItem(new Item(string));
			}
		} else if(gamemap.hasContainer(string2)){
			gamemap.getContainer(string2).addItem(string);
			if (!gamemap.hasItem(string2)){
				gamemap.addItem(new Item(string));
			}
		}
	}



	//	removes object references from game, but item can still be brought back into 
	//	the game through the 'Add' command. Rooms will have references to them as 'borders' 
	//	removed, but there is no means at present for adding a room back in.
	private static void Delete(String string)
	{
		if (gamemap.hasItem(string)){
			gamemap.removeItem(string);
		} else if(gamemap.hasContainer(string)){
			gamemap.removeContainer(string);
		} else if(gamemap.hasCreature(string)){
			gamemap.removeCreature(string);
		} else{
			System.out.println("There is no " + string + " to delete");
		}
		System.out.println("Delete is not yet handled!");
	}

	private static void turnOn(String item) {

		if (playerHas(item)){
			System.out.println("You activate the " + item +"!!");
			doCommand(chopchop(gamemap.getItem(item).turnon()));
		} else {
			System.out.println("You can't turn on a(n) " + item + " because it's not in your inventory");
		}

	}

	private static void takeItem(String item){
		//how does it look for the item? -> depends how item ownership is implemented
		//if item/container/creature definitions are all moved into the gamemap, the for a room, player, container to 
		//have ownership, all we'd need is the name... what if multiple items with same name?

		if (curr_room.hasItem(item)){//items just laying around
			curr_room.removeItem(item);
			inventory.add(item);
		} else {
			System.out.println("There is no " + item + " to take");
		}
	}

	private static void dropItem(String item){

		if (playerHas(item)){
			curr_room.addItem(item);
			inventory.remove(item);
		} else {
			System.out.println("You can't drop a(n) " + item + " because it's not in your satchel");
		}
	}

	private static void readItem(String item){

		if (playerHas(item)) {
			pp(gamemap.getItem(item).getWriting());
		} else if (curr_room.hasItem(item)) {
			System.out.println("You must pick up the " + item + " before you can read it.");
		} else {
			System.out.println("What " + item + "?");
		}
	}

	private static void putItem(String item, String container){
		if (curr_room.hasContainer(container)){
			gamemap.getContainer(container).addItem(item);
		}else{
			System.out.println("This room doesn't have a " + container);
		}
	}

	private static void printInventory(){
		System.out.print("Inventory: ");
		if (inventory.size() == 0){
			System.out.println("empty");
		}else{
			for (int i = 0; i < inventory.size(); i++){
				System.out.print(inventory.get(i));
				if (i < inventory.size()-1){
					System.out.print(", ");
				}
			}
			System.out.println("");
		}
	}

	private static boolean playerHas(String obj){
		return inventory.contains(obj);
	}

	private static void endGame() {
		shellt = false;
		System.out.println("Game Over");
	}

	private static void printHelp()
	{
		System.out.println("Help Menu:");
		System.out.println("\nValid Commands:");
		System.out.println("n, s, e, w: move in direction indicated if able");
		System.out.println("i or inventory prints what items are in players satchel");
		System.out.println("take (item)");
		System.out.println("open (container)");
		System.out.println("open exit");
		System.out.println("read (item)");
		System.out.println("drop (item)");
		System.out.println("put (item) in (container)");
		System.out.println("turn on (item)");
		System.out.println("attack (creature) with (item)");
	}

	private static void move(String d){


		//check to see if border is there
		if (curr_room.hasBorder(d)){
			curr_room = gamemap.getRoom(curr_room.getBorder(d));
			System.out.println(curr_room.getDescription());
		} else {
			System.out.println("Can't go that way");
		}
	}//end move

	//chops String of words delimited by spaces into and ArrayList<String>
	//with each entry corresponding to one word
	public static ArrayList<String> chopchop(String in){
		ArrayList<String> r = new ArrayList<String>();
		String temp = "";

		for (int i = 0; i < in.length(); i++){
			if (in.charAt(i) != ' '){
				temp += in.charAt(i);
			} else {
				r.add(temp);
				temp = "";
			}
		}		
		r.add(temp);

		return r;
	}

	private static String unchop(ArrayList<String> in){
		String temp = "";


		if (in == null){
			return null;
		}
		for (int i = 0; i < in.size(); i++){
			temp += in.get(i) + " ";
		}

		return temp.trim();
	}

	public static void pp(String s){
		if (s == null){
			return;
		}
		char [] ch = s.toCharArray();


		String word = ""; 
		String out = "";

		char c = 0;
		boolean whitespace = false;
		int i = 0;
		int wordcount = 0;
		while (whitespace == false && i < ch.length) {
			word = ""; //clear the temporary string so as not to accumulate words
			while ( ( c = ch[i] ) == ' ' ){

				if ((i) >= ch.length){
					whitespace = true;
					break;
				}
				i++;
			}

			//word += ch[i];                                    //add first letter
			while ( i < ch.length && ( c = ch[i++] ) != ' ' && c != '\n' && c != '\r' ){               //(J)
				word += c;                                //get the rest of the letters
			}
			System.out.print(word + " ");
			wordcount++;
			if (wordcount % 5 == 0){
				System.out.println("");
			}
			out.concat(word);
			//				if ( ch[i] == '\r' || ch[i] == '\n' ){
			//					whitespace = true;
			//				}
			if (i >= ch.length){
				whitespace = true;
			}
		}
		System.out.println("");
	}

}//end Zork class
