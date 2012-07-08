/*If item has turn on element and "turn on" command is issued 
 * by user, action elements in 'turn on' are to be executed if any 
 * given conditions are met
 */
public class Item extends GameElement{
	private String writing = new String();
	private String turnon_print = new String();
	private String turnon_command = new String();
	//private (print element) (action element) turnon = false;
	
	Item(){
		super();
	}

	public Item(String name) {
		super();
		this.setName(name);
		// TODO Auto-generated constructor stub
	}

	public void print(){

		System.out.println("+ Item");
		System.out.println("\tStandard fields:");
		System.out.println("\t- name \t\t\t= "+ name);
		System.out.println("\t- status \t\t= "+ status);
		System.out.println("\t- description \t\t= "+ description);
		System.out.println("\t+ triggers not handled\n");
		
		System.out.println("\tItem specific fields:");
		System.out.println("\t- writing \t\t\t= "+ writing);
	}

	public String getWriting() {
		return new String(writing);
	}

	public void setWriting(String w)
	{
		this.writing = w;
	}

	public void setTurnOn(String print, String action){
		this.turnon_print = print;
		this.turnon_command = action;
	}

	public String turnon(){
		System.out.println(turnon_print);
		return this.turnon_command;
	}



}
