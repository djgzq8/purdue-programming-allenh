import java.util.ArrayList;

/*If an 'accept' element is added, only specific items may be put into the 
 * container and the container need not be opened to insert them (cannot be 
 * opened until one of those items is inserted, in fact).
 */

public class Container extends GameElement{
	private ArrayList<String> accept = new ArrayList<String>();
	private ArrayList<String> items = new ArrayList<String>();


	Container(){
		super();
		status = "closed";
	}

	public void print(){
		System.out.println("+ Container");
		System.out.println("\tStandard fields:");
		System.out.println("\t- name \t\t\t= "+ name);
		System.out.println("\t- status \t\t= "+ status);
		System.out.println("\t- description \t\t= "+ description);
		System.out.println("\t+ triggers not handled\n");
		
		System.out.println("\tConatiner specific fields:");
		System.out.println("\tItems: ");
		System.out.println("\t\t " + items);
	}

	public void addItem(String i){
		items.add(i);
	}
	
	public ArrayList<String> getItems(){
		return this.items;
	}

	public void clearItems(){
		items.clear();
	}
	
	public boolean hasItem(String item){
		return this.items.contains(item);
	}
	
	public boolean isOpen(){
		return (status == "open"|| status == "unlocked");
	}
	
	public boolean verify(MyGameMap map)
	{
		for (int i = 0; i < this.items.size(); i++) {
			if (!map.hasItem(this.items.get(i))) {
				return false;
			}
		}
		
		return true;
	}

	public void addAccept(String item) {
		this.accept.add(item);
		
	}

	public boolean isClosed(){return status.equalsIgnoreCase("closed");}
	
	public boolean open(){
		if (!status.equalsIgnoreCase("locked")){
			this.status = "open";
			System.out.print("Reveals: ");
			for (int i = 0; i < items.size(); i++){
				System.out.print(items.get(i));
				if (i < items.size()-1){
					System.out.print(", ");
				}
			}
			System.out.println("");
			return true;
		} 
		return false;
	}
}
