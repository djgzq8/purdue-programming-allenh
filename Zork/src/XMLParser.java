import java.io.File;
import org.w3c.dom.*;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

public class XMLParser {


	private static int indent = 0;
	private static boolean flag = false;

	public String currentRoom = null;
	private MyGameMap	gamemap;


	public XMLParser(String fname, MyGameMap gamemap) throws Exception {
		this.gamemap = gamemap;


		DocumentBuilderFactory docBuilderFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder docBuilder = null;
		try {
			docBuilder = docBuilderFactory.newDocumentBuilder();
		}
		catch (ParserConfigurationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Document doc = null;
		try {
			doc = docBuilder.parse(new File(fname));
			if (doc == null){
				System.err.println("Failed to open and parse XML file: \n" + fname);
				System.exit(-1);
			}
		}
		catch (Exception e) {
			// TODO: handle exception
		}

		// normalize text representation
		doc.getDocumentElement().normalize();


		//check to see if the XML document has an acceptable format
		if (!doc.getDocumentElement().getNodeName().equalsIgnoreCase("map")){
			System.err.println ("Root element in XML file must be map!\n");
			System.err.println("It is " + doc.getDocumentElement().getNodeName());
			System.exit(-1);
		}

		Node rootnode = doc.getFirstChild();
		Node walker_node = rootnode.getFirstChild();
		while (walker_node != null){
			String nodename = walker_node.getNodeName();


			if (nodename.equalsIgnoreCase("room")){
				gamemap.addRoom(parseRoom(walker_node));

			} else if(nodename.equalsIgnoreCase("item")){
				gamemap.addItem(parseItem(walker_node));

			} else if(nodename.equalsIgnoreCase("container")){
				gamemap.addContainer(parseContainer(walker_node));

			} else if(nodename.equalsIgnoreCase("creature")){
				gamemap.addCreature(parseCreature(walker_node));
			}

			walker_node = walker_node.getNextSibling();
		}
	}

	private Room parseRoom(Node rootnode) throws Exception {
		Room room = new Room();

		Node walker_node = rootnode.getFirstChild();
		while (walker_node != null){
			String nodename = walker_node.getNodeName();

			if (nodename.equalsIgnoreCase("name")){
				room.setName(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("status")){
				room.setStatus(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("description")){
				room.setDescription(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("type")){
				room.setType(walker_node.getFirstChild().getNodeValue().trim());

			} else if (nodename.equalsIgnoreCase("border")){
				NodeList borders = walker_node.getChildNodes();
				String d = "";
				String n = "";

				for (int i = 0; i < borders.getLength(); i++){
					if (borders.item(i).getNodeName() == "direction"){
						d = borders.item(i).getFirstChild().getNodeValue();
					} else if (borders.item(i).getNodeName() == "name") {
						n = borders.item(i).getFirstChild().getNodeValue();
					} else {
						//ignore "#text"
					}
				}
				room.addBorder(d, n);
			} 

			//this bunch uses parsers
			else if(nodename.equalsIgnoreCase("trigger")){
				//parseTrigger returns trigger
				Trigger t = parseTrigger(walker_node);
				room.addTrigger(t);

			} else if (nodename.equalsIgnoreCase("container")){
				if (walker_node.getChildNodes().getLength() == 1){
					if (walker_node.getChildNodes().item(0).getNodeName().equalsIgnoreCase("#text")){
						room.addContainer(walker_node.getChildNodes().item(0).getNodeValue());
					}
				}else {
					Container c = parseContainer(walker_node);
					System.out.println(c.getName());
					gamemap.addContainer(c);
					room.addContainer(c.getName());
				}
			} else if (nodename.equalsIgnoreCase("item")){

				if (walker_node.getChildNodes().getLength() == 1){
					if (walker_node.getChildNodes().item(0).getNodeName().equalsIgnoreCase("#text")){
						room.addItem(walker_node.getChildNodes().item(0).getNodeValue());
					}
				}else {
					Item i = parseItem(walker_node);
					System.out.println(i.getName());
					gamemap.addItem(i);
					room.addItem(i.getName());
				}

			} else if (nodename.equalsIgnoreCase("creature")){
				if (walker_node.getChildNodes().getLength() == 1){
					if (walker_node.getChildNodes().item(0).getNodeName().equalsIgnoreCase("#text")){
						room.addCreature(walker_node.getChildNodes().item(0).getNodeValue());
					}
				}else {
					Creature c = parseCreature(walker_node);
					System.out.println(c.getName());
					gamemap.addCreature(c);
					room.addItem(c.getName());
				}
			} else if (nodename.equalsIgnoreCase("#text")){

			} else {
				throw new Exception(nodename + " isn't an element that should be defined\n" +
						"in a " + rootnode.getNodeName());
			}
			walker_node = walker_node.getNextSibling();
		}

		return room;
	}

	//when parseItem is called an Item object is returned, all Item definitions should remain in the gamemap
	private Item parseItem(Node rootnode) throws Exception{
		Item item = new Item();

		if (rootnode.getChildNodes().getLength() == 1){

			return null;
		}
		Node walker_node = rootnode.getFirstChild();

		while (walker_node != null){
			String nodename = walker_node.getNodeName();

			if (nodename.equalsIgnoreCase("name")){
				item.setName(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("status")){
				item.setStatus(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("description")){
				item.setDescription(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("writing")){
				item.setWriting(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("turnon")){

				item.setTurnOn(walker_node.getFirstChild().getNextSibling().getFirstChild().getNodeValue(),
						walker_node.getFirstChild().getNextSibling().getNextSibling().getNextSibling().getFirstChild().getNodeValue());
			}

			//this bunch uses parsers
			else if(nodename.equalsIgnoreCase("trigger")){
				//parseTrigger returns trigger
				Trigger t = parseTrigger(walker_node);
				item.addTrigger(t);

			} else if (nodename.equalsIgnoreCase("#text")){

			} else {
				throw new Exception(nodename + " isn't an element that should be defined\n" +
						"in a " + rootnode.getNodeName());
			}
			walker_node = walker_node.getNextSibling();
		}

		return item;
	}


	private Container parseContainer(Node rootnode) throws Exception{
		Container container = new Container();
	

	Node walker_node = rootnode.getFirstChild();
		
		while (walker_node != null){
			String nodename = walker_node.getNodeName();

			if (nodename.equalsIgnoreCase("name")){
				container.setName(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("status")){
				container.setStatus(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("description")){
				container.setDescription(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("item")){
				if (walker_node.getChildNodes().getLength() == 1){
					if (walker_node.getChildNodes().item(0).getNodeName().equalsIgnoreCase("#text")){
						container.addItem(walker_node.getChildNodes().item(0).getNodeValue());
					}
				}else {
					Item i = parseItem(walker_node);
					System.out.println(i.getName());
					gamemap.addItem(i);
					container.addItem(i.getName());
				}

			} else if(nodename.equalsIgnoreCase("accept")){
				//container.addAccept(walker_node.getFirstChild().getNodeValue());
			}

			//this bunch uses parsers
			else if(nodename.equalsIgnoreCase("trigger")){
				//parseTrigger returns trigger
				Trigger t = parseTrigger(walker_node);
				container.addTrigger(t);

			} else if (nodename.equalsIgnoreCase("#text")){

			} else {
				throw new Exception(nodename + " isn't an element that should be defined\n" +
						"in a " + rootnode.getNodeName());
			}
			walker_node = walker_node.getNextSibling();
		}

		
		return container;
	}

	//when parseItem is called an Item object is returned, all Item definitions should remain in the gamemap
	private Creature parseCreature(Node rootnode) throws Exception{
		Creature creature = new Creature();

		Node walker_node = rootnode.getFirstChild();
		while (walker_node != null){
			String nodename = walker_node.getNodeName();

			if (nodename.equalsIgnoreCase("name")){
				creature.setName(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("status")){
				creature.setStatus(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("description")){
				creature.setDescription(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("vulnerability")){
				creature.addVulnerability(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("attack")){
				//creature.setDescription(walker_node.getFirstChild().getNodeValue().trim());
				parseAttack(walker_node, creature);
			}

			//this bunch uses parsers
			else if(nodename.equalsIgnoreCase("trigger")){
				//parseTrigger returns trigger
				Trigger t = parseTrigger(walker_node);
				creature.addTrigger(t);

			} else if (nodename.equalsIgnoreCase("#text")){

			} else {
				throw new Exception(nodename + " isn't an element that should be defined\n" +
						"in a " + rootnode.getNodeName());
			}
			walker_node = walker_node.getNextSibling();
		}

		
		return creature;
	}

	private void parseAttack(Node rootnode, Creature creature) throws Exception
	{
		Node walker_node = rootnode.getFirstChild();
		
		while (walker_node != null){
			String nodename = walker_node.getNodeName();
			
			
			if (nodename.equalsIgnoreCase("condition")){
				Node t = walker_node.getFirstChild();
				String has = "";
				String owner = "";
				String object = "";
				String status = "";

				while (t != null) {

					if (t.getNodeName().equalsIgnoreCase("has")){
						has = t.getFirstChild().getNodeValue();
					}else if ( (t.getNodeName().equalsIgnoreCase("owner"))) {
						owner = t.getFirstChild().getNodeValue();
					}else if ( (t.getNodeName().equalsIgnoreCase("object"))) {
						object = t.getFirstChild().getNodeValue();
					}else if ( (t.getNodeName().equalsIgnoreCase("status"))) {
						status = t.getFirstChild().getNodeValue();
					}else if ( (t.getNodeName().equalsIgnoreCase("#text"))) {

					}


					t = t.getNextSibling();
				}
				if (has == ""){//means it wasn't touched
					creature.addCondition(new Condition(object, status));
				} else {
					creature.addCondition(new Condition(owner, object, has));
				}
			} else if (nodename.equalsIgnoreCase("print")){
				creature.addPrint(walker_node.getFirstChild().getNodeValue().trim());

			} else if (nodename.equalsIgnoreCase("action")){
				creature.addAction(walker_node.getFirstChild().getNodeValue().trim());

			} else if (nodename.equalsIgnoreCase("#text")){
				
				
			} else {
				throw new Exception(nodename + " isn't an element that should be defined\n" +
						"in a " + rootnode.getNodeName());
			}
			
			walker_node = walker_node.getNextSibling();
		}
		
		
	}

	private Trigger parseTrigger(Node rootnode) throws Exception{
		Trigger trigger = new Trigger();

		Node walker_node = rootnode.getFirstChild();

		while (walker_node != null){
			String nodename = walker_node.getNodeName();

			if (nodename.equalsIgnoreCase("type")){
				trigger.setType(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("command")){
				trigger.setCommand(walker_node.getFirstChild().getNodeValue().trim());

			} else if(nodename.equalsIgnoreCase("print")){
				trigger.setPrint(walker_node.getFirstChild().getNodeValue().trim());
		

			} else if(nodename.equalsIgnoreCase("action")){
				trigger.addAction(walker_node.getFirstChild().getNodeValue().trim());
				

			} else if(nodename.equalsIgnoreCase("turnon")){
				//item.setDescription(walker_node.getFirstChild().getNodeValue().trim());

			}

			//this bunch uses parsers
			else if(nodename.equalsIgnoreCase("condition")){
				Node t = walker_node.getFirstChild();
				String has = "";
				String owner = "";
				String object = "";
				String status = "";

				while (t != null) {

					if (t.getNodeName().equalsIgnoreCase("has")){
						has = t.getFirstChild().getNodeValue();
					}else if ( (t.getNodeName().equalsIgnoreCase("owner"))) {
						owner = t.getFirstChild().getNodeValue();
					}else if ( (t.getNodeName().equalsIgnoreCase("object"))) {
						object = t.getFirstChild().getNodeValue();
					}else if ( (t.getNodeName().equalsIgnoreCase("status"))) {
						status = t.getFirstChild().getNodeValue();
					}else if ( (t.getNodeName().equalsIgnoreCase("#text"))) {

					}


					t = t.getNextSibling();
				}
				if (has == ""){//means it wasn't touched
					trigger.addCondition(object, status);
				} else {
					trigger.addCondition(owner, object, has);
				}

			} else if (nodename.equalsIgnoreCase("#text")){

			} else {
				throw new Exception(nodename + " isn't an element that should be defined\n" +
						"in a " + rootnode.getNodeName());
			}
			walker_node = walker_node.getNextSibling();
		}

		return trigger;
	}

	public void printMap(){
		//		Set<String> keySet = gamemap.keySet();
		//		Iterator<String> i = keySet.iterator();
		//
		//		while(i.hasNext()) { 
		//			//System.out.println(gamemap.get(i.next()).name); 
		//		} 

	}

}