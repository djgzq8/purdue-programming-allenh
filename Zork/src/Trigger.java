import java.util.ArrayList;


public class Trigger {
	public String type = "single"; //or permanent for continuous use
	public String command = null;//this a special condition

	public ArrayList<Condition> conditions = new ArrayList<Condition>();
	public ArrayList<String> actions = new ArrayList<String>();
	public String prints = "";

	//or of operation is do prints first, then action commands
	Trigger(){

	}

	public void addCondition(String owner, String obj, String has){
		this.conditions.add(new Condition(owner, obj, has));
	}

	public void addCondition(String obj, String status){
		this.conditions.add(new Condition(obj, status));
	}

	public void setCommand(String str){
		this.command = new String(str);
	}

	public String getCommand(){
		return new String(this.command);
	}

	public void setType(String type) {
		if ( (type.equalsIgnoreCase("single") || type.equalsIgnoreCase("permanent")  ) ){
			this.type = type;
		} else {
			System.out.println("A trigger type can only be \"single\" or \"permanent\"");
		}
	}

	public String getType() {
		return new String(this.type);
	}

	public boolean hasAction(){
		return !(actions.size() == 0 || actions == null);
	}

	public void addAction(String action){
		this.actions.add(action);
	}

	public void use(){
		if (this.type.equalsIgnoreCase("single"))
			this.type = "done";
	}
	
	public void print(){
		System.out.println(this.conditions);
	}

	public boolean checkTrigger(String commandin, MyGameMap gamemap, ArrayList<String> inventory){
		//see if the command portion of the trigger is the same, if it has one
		if (type.equalsIgnoreCase("done"))
			return false;
		if (commandin != null){//there IS a command
			if (!(this.command == null || this.command == "")) {//if there is a command see if they're equal
				if (!commandin.equalsIgnoreCase(this.command)){
					return false;
				}else{
//					System.out.println("The commands are equal");
				}
			}
		} else {
			if (this.command != null){
				return false;
			}
		}
		//if the command passes, and their are no conditions then the trigger passes
		if (conditions.size() == 0){
			return true;
		} else{
//			System.out.println("There are more conditions to check");
		}

		if (this.conditions.size() > 0){
			for (int i = 0; i < this.conditions.size(); i++){//check all conditions
				Condition temp = conditions.get(i);
				if (temp.type.equalsIgnoreCase("status")){
					//check item status

					if (gamemap.getItem(temp.object) != null) {
						if (gamemap.getItem(temp.object).getStatus().equalsIgnoreCase(temp.status)){
							return true;
						} else {
							return false;
						}

					}else if (gamemap.getContainer(temp.object) != null){
						if (gamemap.getContainer(temp.object).getStatus().equalsIgnoreCase(temp.status)){
							return true;
						} else {
							return false;
						}

					}


				}else if (temp.type.equalsIgnoreCase("own")){
					//check ownership
//					System.out.println("Check ownership: " + temp.owner);
					if (temp.has.equalsIgnoreCase("no")){ //trip when they don't have it
						if (!inventory.contains(temp.object)){  //this needs to be broadened
//							System.out.println("player doesn't have " + temp.object);

							return true;
						}else {
							return false;//because the trigger failed
						}
					}else if (temp.has.equalsIgnoreCase("yes")){//trips when they do have it
						if (temp.owner.equalsIgnoreCase("inventory") && inventory.contains(temp.object)){  //this needs to be broadened
//							System.out.println("player has " + temp.object);

							return true;
						}else if (gamemap.getContainer(temp.owner) != null && gamemap.getContainer(temp.owner).getItems().contains(temp.object)){
							return true;
						}else{
							return false;//because the trigger failed
						}
					}

				}
			}

		}

		return false;
	}

	public void setPrint(String p) {
		this.prints = p;	
	}


}
//status condition
//<trigger>
//<type>single</type>
//<command>attack dragon with axe</command>
//<condition>
//  <object>axe</object>
//  <status>sharp</status>
//</condition>
//<action>take axe</action>
//<action>attack dragon with axe</action>
//<print>the axe is quite sharp</print>
//<action>Game Over</action>
//</trigger>

//owner condition
//<trigger>
//<type>permanent</type>
//<command>n</command>
//<condition>
//  <has>no</has>
//  <object>torch</object>
//  <owner>inventory</owner>
//</condition>
//<print>*stumble* need some light...</print>
//</trigger>