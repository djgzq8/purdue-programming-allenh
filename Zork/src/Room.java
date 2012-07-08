import java.util.ArrayList;


public class Room extends GameElement{
	private String type = "regular"; //Type is assumed 'regular unless specified'
	private String north = null, south = null, east = null, west = null;
	private ArrayList<String> containers = new ArrayList<String>();
	private ArrayList<String> items = new ArrayList<String>();
	private ArrayList<String> creatures = new ArrayList<String>();

	Room(){
		super();
		
	}

	Room(String name, String status, String description){
		super(name, status, description);
	}

	Room(String name){
		this.name = name;
	}

	//Setters
	public void addContainer(String c){
		containers.add(c);
	}

	//	public Container getContainer(String cname){
	//		return this.containers.indexOf(cname)
	//	}

	public void addItem(String i){
		items.add(i);
	}

	public boolean hasItem(String i){
		return this.items.contains(i);
	}

	public boolean hasContainer(String c) {
		return this.containers.contains(c);
	}

	public boolean hasCreature(String c) {
		return this.creatures.contains(c);
	}
	
	public void addCreature(String c){
		creatures.add(c);
	}

	public void print(){

		System.out.println("+ Room");
		System.out.println("\tStandard fields:");
		System.out.println("\t- name \t\t\t= "+ name);
		System.out.println("\t- status \t\t= "+ status);
		System.out.println("\t- description \t\t= "+ description);
		System.out.println("\t+ There are " + this.triggers.size() + " triggers in this room\n");

		System.out.println("\tRoom specific fields:");
		System.out.println("\t- type (regular) \t= "+ type);
		System.out.println("\t+ borders");
		if (north != null) {
			System.out.println("\t\tNorth border is room: " + north);
		} 
		if (south != null) {
			System.out.println("\t\tSouth border is room: " + south);
		} 
		if (east != null) {
			System.out.println("\t\tEast border is room: " + east);
		}
		if (west != null) {
			System.out.println("\t\tWest border is room: " + west);
		}
		System.out.println("\t+ containers:\n\t\t" + this.containers);
		System.out.println("\t+ items: \n \t\t" + this.items);
		System.out.println("\t+ creatures: \n \t\t" + this.creatures);
	}

	public void setType(String s) {
		this.type = s;
	}

	public void addBorder(String direction, String name){
		if (direction.equalsIgnoreCase("north")) {
			this.north = new String();
			this.north = name;
		} else if (direction.equalsIgnoreCase("south")) {
			this.south = new String();
			this.south = name;
		} else if (direction.equalsIgnoreCase("east")) {
			this.east = new String();
			this.east = name;
		} else if (direction.equalsIgnoreCase("west")) {
			this.west = new String();
			this.west = name;
		} 
	}

	public String getDescription()
	{
		return this.description;
	}

	public boolean hasBorder(String string)
	{
		if (string.equalsIgnoreCase("north") || string.equalsIgnoreCase("n")){
			return (north != null);
		} else if (string.equalsIgnoreCase("south") || string.equalsIgnoreCase("s")) {
			return (south != null);
		} else if (string.equalsIgnoreCase("east") || string.equalsIgnoreCase("e")) {
			return (east != null);
		} else if (string.equalsIgnoreCase("west") || string.equalsIgnoreCase("w")) {
			return (west != null);
		} else {
			return false;
		}
	}

	public String getBorder(String string)
	{
		if (string.equalsIgnoreCase("north") || string.equalsIgnoreCase("n")){
			return new String(this.north);
		} else if (string.equalsIgnoreCase("south") || string.equalsIgnoreCase("s")) {
			return new String(this.south);
		} else if (string.equalsIgnoreCase("east") || string.equalsIgnoreCase("e")) {
			return new String(this.east);
		} else if (string.equalsIgnoreCase("west") || string.equalsIgnoreCase("w")) {
			return new String(this.west);
		} else {
			return null;
		}
	}


	public String getType() {

		return new String(this.type);
	}


	public void removeItem(String i)
	{
		this.items.remove(i);
	}

	public boolean verify(MyGameMap map){

		for (int i = 0; i < this.items.size(); i++) {
			if (!map.hasItem(this.items.get(i))) {
				return false;
			}
		}

		for (int i = 0; i < this.containers.size(); i++) {
			if (!map.hasContainer(this.containers.get(i))) {
				return false;
			}
		}

		for (int i = 0; i < this.creatures.size(); i++) {
			if (!map.hasCreature(this.creatures.get(i))) {
				return false;
			}
		}

		return true;
	}

	public ArrayList<String> getAllItems() {
		return (ArrayList<String>) items.clone();
	}

	public ArrayList<String> getAllCreatures() {
		return (ArrayList<String>) this.creatures.clone();
	}
	
	public ArrayList<String> getAllContainers() {
		return (ArrayList<String>) this.containers.clone();
	}
	
	

}
