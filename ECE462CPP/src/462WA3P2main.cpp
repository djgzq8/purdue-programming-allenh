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
class User
{
public:
  User (string n, int a):
    name(n),
    age(a)
  {
  }
  virtual ~User()
  {
  }
  virtual void print()
  {
    cout << "name: " << name << endl;
    cout << "age: " << age << endl;
  }
  bool operator == (const User & u2) const
  {
    if ((name != u2.name) || (age != u2.age))
      {
	return false;
      }
    return true;
  }
private:
  string name;
  int age;
};

class UserFunction
{
public:
  static void uf1(User u1, User u2)
  {
    u1 = u2;
  }
  static void uf2(User & u1, User & u2)
  {
    u1 = u2;
  }
  static void uf3(User * u1, User * u2)
  {
    u1 = u2;
  }
  static void uf4(User * u1, User * u2)
  {
    * u1 = * u2;
  }
};

bool testUserFunction(int f,
		      string n1, int a1,
		      string n2, int a2)
{
  User u1(n1, a1);
  User u2(n2, a2);
  switch (f)
    {
    case 1:
      UserFunction::uf1(u1, u2);
      break;
    case 2:
      UserFunction::uf2(u1, u2);
      break;
    case 3:
      UserFunction::uf3(& u1, & u2);
      break;
    case 4:
      UserFunction::uf4(& u1, & u2);
      break;
    default:
      cout << "unknown option" << endl;
      break;
    }
  return (u1 == u2);
}

class Vector
{
public:
  Vector(int s, int * d)
  {
    size = s;
    data = new int[s];
    for (int i = 0; i < s; i ++)
      {
	data[i] = d[i];
      }
  }
  virtual ~Vector()
  {
    // *************
    // ATTENTION
    // delete [] data;
    // *************
  }
  virtual void print()
  {
    cout << "There are " << size << " elements:" << endl;
    for (int i = 0; i < size; i ++)
      {
	cout << data[i] << " ";
      }
    cout << endl;
  }
  bool operator == (const Vector & u2) const
  {
    if (size != u2.size)
      {
	return false;
      }
    for (int i = 0; i < size; i ++)
      {
	if (data[i] != u2.data[i])
	  {
	    return false;
	  }
      }
    return true;
  }
private:
  int size;
  int * data;
};

class VectorFunction
{
public:
  static void vf1(Vector v1, Vector v2)
  {
    v1 = v2;
  }
  static void vf2(Vector & v1, Vector & v2)
  {
    v1 = v2;
  }
  static void vf3(Vector * v1, Vector * v2)
  {
    v1 = v2;
  }
  static void vf4(Vector * v1, Vector * v2)
  {
    * v1 = * v2;
  }
};

bool testVectorFunction(int f,
			int s1, int * d1,
			int s2, int * d2)
{
  Vector v1(s1, d1);
  Vector v2(s2, d2);
  switch (f)
    {
    case 1:
      VectorFunction::vf1(v1, v2);
      break;
    case 2:
      VectorFunction::vf2(v1, v2);
      break;
    case 3:
      VectorFunction::vf3(& v1, & v2);
      break;
    case 4:
      VectorFunction::vf4(& v1, & v2);
      break;
    default:
      cout << "unknown option" << endl;
      break;
    }
  return (v1 == v2);
}

int main(int argc, char * argv[])
{
  /* Assume
     name11 != name12, age11 != age12
     name21 != name22, age21 != age22
     name31 != name32, age31 != age32
     name41 != name42, age41 != age42

     size11 != size12, data11 != data12
     size21 != size22, data21 != data22
     size31 != size32, data31 != data32
     size41 != size42, data41 != data42
  */

  /* Which prints 1? */
  cout << testUserFunction(1, "Allen", 1, "Katie", 2) << endl;
  cout << testUserFunction(2, "John", 3, "Lisa", 4) << endl;
  cout << testUserFunction(3, "Jim", 5, "Sarah", 6) << endl;
  cout << testUserFunction(4, "Rick", 7, "Destiny", 8) << endl;

  /* Which prints 1? */

  /* Which would cause the program to crash if Vector's
     destructor has

     delete [] data;

  */
	int data1[3] = {1,2,3};
	int data2[3] = {4,5,6};
	int data3[3] = {7,8,9};
	int data4[3] = {10,11,12};
	int data5[3] = {13,14,15};
	int data6[3] = {16,17,18};
	int data7[3] = {19,20,21};
	int data8[3] = {22,23,24};
  cout << testVectorFunction(1, 1, data1, 2, data2) << endl;
  cout << testVectorFunction(2, 3, data3, 4, data4) << endl;
  cout << testVectorFunction(3, 5, data5, 6, data6) << endl;
  cout << testVectorFunction(4, 7, data7, 8, data8) << endl;

  return 0;
}
