import java.util.ArrayList;

 /*If "attack" command is issued by user with creature's vulnerability, action 
  * elements in 'attack' are to be executed if any given conditions are met.
  */
public class Creature extends GameElement{
	private ArrayList<String> vulnerability = new ArrayList<String>();
	private ArrayList<String> attack_action = new ArrayList<String>();
	private ArrayList<Condition> attack_condition = new ArrayList<Condition>();
	private ArrayList<String> attack_print = new ArrayList<String>();
	
	
	
	Creature(){
		super();
	}
	
	
	public void print(){

		System.out.println("+ Creature");
		System.out.println("\tStandard fields:");
		System.out.println("\t- name \t\t\t= "+ name);
		System.out.println("\t- status \t\t= "+ status);
		System.out.println("\t- description \t\t= "+ description);
		System.out.println("\t+ triggers not handled\n");
		
		System.out.println("\tCreature specific fields:");
		System.out.println("\tAttack");
		System.out.println("\tPrints: " + attack_print);
		System.out.println("\tVulnerabilities: " + vulnerability);
		System.out.println("\tActions: " + attack_action);
		System.out.println("\tCondtions: " + attack_condition);
	}

	public void setAttack() {
		
	}
	
	public void addVulnerability(String v){
		this.vulnerability.add(v);
	}

	public void addAction(String a){
		this.attack_action.add(a);
	}
	
	public void addPrint(String p){
		this.attack_print.add(p);
	}
	
	public void addCondition(Condition c){
		this.attack_condition.add(c);
	}

	//returns true if attack is successful
	//returns false if it fails
	public boolean attack(Item item){
		if (vulnerability.contains(item.getName())){
			if (attack_condition.size() == 0){
				return true;
			}else{
				//check conditions
				for (int i = 0; i < attack_condition.size(); i++){
					Condition c = attack_condition.get(i);
					if (c.type.equals("own")){
						System.out.println("ownership not tested");
						if (c.has.equals("yes")){
							//should have
						}else{
							//not supposed to have
						}
					} else if (c.type.equals("status")){
						if (c.object.equals(item.getName())){
							if (c.status.equals(item.getStatus())){
								
								return true;
							} else {
								
								return false;
							}
						}else{
							System.out.println("A condition on an item that's not a vulnerability is not allowed");
						}
					}
				}
			}
		}
		return false;
	}


	public ArrayList<String> getPrints()
	{
		// TODO Auto-generated method stub
		return this.attack_print;
	}
	
	public ArrayList<String> getActions()
	{
		// TODO Auto-generated method stub
		return this.attack_action;
	}
}
