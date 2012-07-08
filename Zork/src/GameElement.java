import java.util.ArrayList;


public class GameElement {
	protected String name = null;
	protected String status = null;
	protected String description = null;
	protected ArrayList<Trigger> triggers = new ArrayList<Trigger>();
	
	GameElement(){
		this.name = new String();
		this.status = new String();
		this.description = new String();
	};
	
	GameElement(String n, String s, String d){
		this.name = n;
		this.status = s;
		this.description = d;
	}
	
	public void addTrigger(Trigger t) {
		this.triggers.add(t);
	}
	
	public Trigger checkTrigger(String command, MyGameMap gamemap, ArrayList<String> inventory){
//		System.out.println("Checking triggers on: " + this.name);
		if (triggers.size() == 0){
//		System.out.println(this.name + " doesn't have any triggers");
			return null;
		}
		for (int i = 0; i < triggers.size(); i++){
			
			if (triggers.get(i).checkTrigger(command, gamemap, inventory)){
//				System.out.println(this.name + " triggered a trigger");
				return triggers.get(i);
			}
		}
		
		return null;
	}
	
	public void setName(String s) {
		this.name = s;	
	}

	public String getName(){
		return new String(this.name);
	}
	
	public void setStatus(String s) {
		this.status = s;	
	}
	
	public void setDescription(String s) {
		this.description = s;
	}	

	public String getStatus(){
		return new String(this.status);
	}
}
