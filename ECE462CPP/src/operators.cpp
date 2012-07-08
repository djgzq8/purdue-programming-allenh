//I'm not entirely sure what this is. All I know is it was created while I was taking
// OO programming in fall 2011. Might demonstrate operator overloading.

#include <iostream>
#include <string>
using namespace std;


class A {
public:
	A() : str("no-arg") {
		cout << "Automatic call to no-arg constructor" << endl;
	}
	A(int j) {
		str.append("1-arg");
		cout << "This 1-arg constructor brought to you by the letter " << j << endl;
	}
	string* getString(){
		string* temp = new string(str);
		return temp;
	}
private:
	string str;
};

class B : public virtual A {
public:
	B(){
		cout << "B no-arg constructor" << endl;
	}
	B(int i) : A(2) {
			cout << "B 1-arg" << endl;
		}
};

class C : public virtual A {
public:
	C() {//This is how you invoke a base class constructor, otherwise it will call the no-arg constructor
		cout << "C no-arg constructor" << endl;
	}
	C(int i) : A(i) {

		cout << "C 1-arg" << endl;
	}
};

class D : public B, public C {
public:
	D() : B(), C() {//This is how you invoke a base class constructor, otherwise it will call the no-arg constructor

	}
	D(int i) : B(i), C(i){

		cout << "D 1-arg" << endl;
	}
};

int main(){
	B* b;
	cout << "******* MAKE B**********" << endl;
	b = new B();
	cout << "this is b " << *(b->getString()) << endl;
	cout << "******* MAKE C**********" << endl;
	C* c = new C(1);
cout << "******* MAKE D**********" << endl;
	D* d = new D(1);
	cout << *(d->getString()) << endl;
	free(c);
	return 0;
}
