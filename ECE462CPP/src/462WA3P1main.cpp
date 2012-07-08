/*
 * Was for Writing Assignment 3 Problem 1 for ECE462 fall 2011
 *
 * This is an example C++ program. This is the only file necessary.
 *
 *
 */

#include <iostream>
#include <string>
using namespace std;

class Person
{
public:
	Person(string n, int a):
		name(n),
		age(a)
	{
	}
	//void print() -- Teacher class couldn't override it since it's not virtual
	virtual void print()
	{
		cout << "Person::print" << endl;
		cout << "    name: " << name << endl;
		cout << "    age: " << age << endl;
	}
	virtual ~Person()
	{
	}
private:
	const string name;
	int age;
};

//class Teacher: public Person
class Teacher : virtual public Person
{
public:
	Teacher(string n, int a, string s):
		Person(n, a),
		subject(s)
	{
	}
	//private: -- TeachingAssistant print method could not call this private method
	void print()
	{
		cout << "Teacher::print" << endl;
		Person::print();
		cout << "    subject: " << subject << endl;
	}
private:
	string subject;
};

//class Student: public Person
class Student : virtual public Person
{
public:
	Student(string n, int a, string d):
		Person(n, a), department(d)
	{
	}
	void print()
	{
		cout << "Student::print" << endl;
		Person::print();
		cout << "    department: " << department << endl;
	}
private:
	string department;
};

class GradStudent: public Student
{
public:
	GradStudent(string n, int a, string d, string v):
		Person(n, a), //this is fixed by virtual inheritance  in Student class
		Student(n, a, d),
		adviser(v)
	{
	}
	void print()
	{
		cout << "GradStudent::print" << endl;
		Student::print();
		cout << "    adviser: " << adviser << endl;
	}
private:
	string adviser;
};

class TeachingAssistant: public GradStudent, public Teacher
{
public:
	TeachingAssistant(string n, int a, string s,
			string d, string v, int y):
				Person(n, a),
				GradStudent(n, a, d, v),
				Teacher(n, a, s),
				salary(y)
	{
	}
	void print()
	{
		cout << "TeachingAssistant::print" << endl;
		GradStudent::print();
		Teacher::print();
		cout << "    salary: " << salary << endl;
	}
private:
	int salary;
};

int main(int argc, char * argv[])
{
	Person * ta =
			new TeachingAssistant("Smith", 25, "Java", "ECE", "Johnson", 2000);
	ta -> print();
	/* ATTENTION: should print ALL attributes */
	delete ta;
	return 0;
}
