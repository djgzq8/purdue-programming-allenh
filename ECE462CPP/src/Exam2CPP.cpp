//============================================================================
// Name        : Exam2CPP.cpp
// Author      : Allen Humphreys
// Version     :
// Copyright   : Allen's Exam Cheat Sheet
// Description : Hello World in C++, Ansi-style
// 		something I made up to help demonstrate a lot about CPP and OO in
//		a shorter program. I think I wasn't successful. But I'll keep it around
//============================================================================

#include <iostream>
#include <string>
using namespace std;

int main() {
	string cargoNames[5] = {"None", "Human", "Animal", "freight", "Natural Resources"};

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}

class Vehicle {

public:
	Vehicle (){//or initialize members like this
		mode = "";
		cargo = 0;
	}
	Vehicle(string mode, int c) : mode(mode), cargo(c) { //can initialize members like this

	}
private:
	string mode;
	int cargo;
};

class Automobile : Vehicle {
public:
	Automobile();
	Automobile(int year, string make, string model);//only a declaration
private:
	int year;
	string make;
	string model;
};

//definition outside of class definition
Automobile::Automobile(int year, string make, string model){
	this->year = year;
	this->make = make;
	this->model = model;
}

class SUV : Automobile {
public:
	SUV(){
		Automobile::Automobile();
		this->offroadoptions = new string[4]();
		this->offroadoptions[0] = new string("4x4");
	}

	SUV(int year, string make, string model, string** offroadoptions) : offroadoptions(offroadoptions) {
		Automobile::Automobile(year, make, model);
	}
	~SUV(){
		delete offroadoptions; //only need to delete something if it's been created with new
	}
private:
	string** offroadoptions;
};

class Sedan : Automobile{
public:
	Sedan(){
		Automobile::Automobile();
		this->luxuryoptions = new string[4]();
		luxuryoptions[0] = new string("heated seats");
	}

	Sedan(int year, string make, string model, string** luxuryoptions) : luxuryoptions(luxuryoptions) {
		Automobile::Automobile(year, make, model);
	}
	~Sedan(){
		delete luxuryoptions;
	}
private:
	string** luxuryoptions;
};

class Crossover : SUV, Sedan {//two base classes
private:
	Crossover(){

	}

};
