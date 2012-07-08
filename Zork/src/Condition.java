class Condition {
	public String has = "";
	public String object = "";
	public String owner = "";
	public String type = ""; //will be own or status depending on the
	public String status = "";
	
	Condition(String owner, String obj, String has){
		this.has = has;
		this.owner = owner;
		this.object = obj;
		this.type = "own";
	};

	Condition(String obj, String status){ //obj must have the same status
		this.object = obj;
		this.status = status;
		this.type = "status";
	};

	public String getHas(){
		return this.has;
	}

	public String getObject(){
		return this.has;
	}
	
	public String getOwner(){
		return this.has;
	}
	
	public String getType(){
		return this.has;
	}
	
	
	
}