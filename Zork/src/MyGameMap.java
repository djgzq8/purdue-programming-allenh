import java.util.Enumeration;
import java.util.Hashtable;


public class MyGameMap {
	public Hashtable<String, Room> rooms;
	public Hashtable<String, Item> items;
	public Hashtable<String, Container> containers;
	public Hashtable<String, Creature> creatures;
	
	
	public MyGameMap() {
		rooms = new Hashtable<String, Room>();
		items = new Hashtable<String, Item>();
		containers = new Hashtable<String, Container>();
		creatures = new Hashtable<String, Creature>();
	}
	
	public void addRoom(Room r){
		rooms.put(r.name.toLowerCase(), r);
	}
	
	public void addItem(Item i){

		this.items.put(i.name.toLowerCase(), i);
	}
	
	public void addContainer(Container c){
		containers.put(c.name.toLowerCase(), c);
	}
	
	public void addCreature(Creature c){
		creatures.put(c.name.toLowerCase(), c);
	}

	public Room getRoom(String name){
		return this.rooms.get(name.toLowerCase());
	}
	

	public Item getItem(String name) {
		return this.items.get(name.toLowerCase());
	}

	public void verify() 
	{
		
		
		for (Enumeration<Room> iter = this.rooms.elements(); iter.hasMoreElements();){
			Room r = iter.nextElement();
			if (!r.verify(this)) {
				System.err.println("Inconsistent Room");
				System.exit(-1);
			}
			
		}
		
		for (Enumeration<Container> iter = this.containers.elements(); iter.hasMoreElements();){
			Container c = iter.nextElement();
			if (!c.verify(this)) {
				System.err.println("Inconsistent Container");
				System.exit(-1);
			}
		}
		
		
	}

	public boolean hasItem(String i) {
		return this.items.containsKey(i.toLowerCase());
	}

	public boolean hasContainer(String i) {
		return this.containers.containsKey(i.toLowerCase());
	}
	
	public boolean hasCreature(String i) {
		return this.creatures.containsKey(i.toLowerCase());
	}
	
	public boolean hasRoom(String i) {
		return this.rooms.containsKey(i.toLowerCase());
	}

	public void print() {
		for (Enumeration<Room> e = this.rooms.elements(); e.hasMoreElements();){
			e.nextElement().print();
		}
		for (Enumeration<Container> e = this.containers.elements(); e.hasMoreElements();){
			e.nextElement().print();
		}
		for (Enumeration<Item> e = this.items.elements(); e.hasMoreElements();){
			e.nextElement().print();
		}
		for (Enumeration<Creature> e = this.creatures.elements(); e.hasMoreElements();){
			e.nextElement().print();
		}
	}

	public Container getContainer(String cname)
	{
		return this.containers.get(cname.toLowerCase());
	}

	
	public Creature getCreature(String cname)
	{
		// TODO Auto-generated method stub
		return this.creatures.get(cname.toLowerCase());
	}

	public void removeItem(String string)
	{
		this.items.remove(string);
	}
	public void removeContainer(String string)
	{
		this.containers.remove(string);
	}	
	
	
	public void removeCreature(String string)
	{
		this.creatures.remove(string);
	}

}
