// CPP program to illustrate the concept of Virtual Functions (Abstract Functions)
/// Comments with three signs are mine
/// Useful example http://www.learncpp.com/cpp-tutorial/126-pure-virtual-functions-abstract-base-classes-and-interface-classes/
#include <iostream>
using namespace std;

class base
{
public:
    virtual void print ()
    { cout<< "print base class" <<endl; } /// endl does the same as cout<< "print\n"
                                          ///                       cout.flush(); It flushes the stream

    void show () 
    { cout<< "show base class" <<endl; }

};

class derived : public base ///derived inherits :  from base class
{
public: 
    void print () override ///only the virtual can be overriden! or is it?
    { cout<< "print derived class" <<endl; }

    void show () 
    { cout<< "show derived class" <<endl; }
};

int main () 
{
    base *bptr;
    derived d;
    bptr = &d;
    base b;

    //virtual function, binded at runtime
    bptr->print (); 
    
    // Non-virtual function, binded at compile time
    bptr->show (); 

    d.print (); ///prints derived class - only father classes that are pointers to derived classes run their
                ///base virtual methods -> 

    b.print (); ///prints base class
    
    b.show (); 
}
