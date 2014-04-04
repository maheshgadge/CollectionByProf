/*
 * Main.cpp
 *
 *  Created on: Dec 11, 2012
 *      Author: Mahesh
 */




//
//  main.cpp
//  Collections-C++
//
//  Created by Ugo  Buy on 12/5/2012.
//
//  Modification history:
//  1.  12/7/2012 --    Fixed bug in OrderedCollection::add()
//                      Added tests for iterate() and operator[]
//  2.  12/9/2012 --    Added OrderedCollection::remove() with
//                      test cases

#include <iostream>


// An abstract superclass defining a common protocol for concrete
// subclasses that are collections of elements

class Collection {

public:

    virtual Collection* copy()=0;       // The virtual copy scheme

    virtual ~Collection() {} ;          // Virtual destructor

    virtual Collection& operator=(Collection& rhs)=0;   // Assignment operator

    virtual Collection& add(int x, int pos)=0;  // Add one more element

    virtual bool remove(int x)=0 ;       // Remove an element

    virtual int& operator[](int i)=0;   // Indexing operator

    // This is the iterator, non deferred because it uses only concrete
    // methods defined in subclasses of Collection (when this code is executed,
    // the receiver will be either an Array or OrderedCollection instance and
    // their operator[] will be used, not the deferred one defined in
    // Collection).
    virtual void iterate(void (*fn) (int)) {

        for (int i=0; i < size_; i++) {

            int elem = (*this)[i] ;
            (*fn)(elem) ;
        }
    }

    // This is the membership test, whether argument x is contained in
    // receiver
    virtual bool contains(int x) {

        for (int i = 0; i <= size_ - 1; i++) {

            if ((*this)[i] == x) {
                return true ;
            }
        }

        return false ;
    }

    int size() {return size_ ; }        // accessor for size_ member

protected:

    int size_;                          // number of elements stored in receiver

    // the copy constructor
    Collection(const Collection& c) { size_ = c.size_ ; }

    // the default constructor
    Collection () {size_ = 0 ; }

} ;


// A concrete Collection subclass that simulates the same class in Smalltalk

class OrderedCollection: public Collection {

public:

    OrderedCollection() : Collection() {

        alloc_size = 4 ;                // The initial allocation size
        array_ = new int[4] ;           // the initial allocation
        first = 1 ;                     // initial pointer to the first element
        last = 0 ;                      // initial pointer to the last element
    }

    // the copy constructor will allocate a C-style array for the elements
    // of the receiver and copy of the elements of the existing instance into
    // the new array.  Data member alloc_size, first and last are similarly
    // copied from the argument OC.
    OrderedCollection(const OrderedCollection& oc) : Collection (oc) {

        alloc_size = oc.alloc_size ;    // the size
        first = oc.first ;
        last = oc.last ;
        array_ = new int[alloc_size] ;

        for (int i = first; i <= last; i++) {

            array_[i] = oc.array_[i] ;
        }
    }

    // the destructor just deallocates the array storing the receiver's
    // elements
    virtual ~OrderedCollection () {
        delete[] array_ ;
    }

    // this method creates a new OC instance and initializes it from
    // the receiver by implicitly invoking the copy constructor of OC.
    // The new OC instance is returned.
    virtual OrderedCollection* copy() {

        OrderedCollection* result = new OrderedCollection(*this) ;
        return result ;
    }

    // this operator is similar to the copy constructor, but it also
    // needs to check whether rhs == receiver and to deallocate the
    // existing array of the receiver
    virtual OrderedCollection& operator=( Collection& rhs) {

        if (this == &rhs) return *this ;   // rhs = receiver? If yes, do nothing

        OrderedCollection* temp;           // arg not an OC? If yes, do nothing
        temp = dynamic_cast<OrderedCollection*>(&rhs) ;
        if (! temp) return *this ;

        delete array_ ;                     // delete existing array
        alloc_size = temp->alloc_size ;     // copy data members from rhs
        first = temp->first ;
        last = temp->last ;
        array_ = new int[alloc_size] ;      // allocate new array

        for (int i = first; i <= last; i++) // and copy all rhs elements to receiver
            array_[i] = temp->array_[i] ;


        return *this ;
    }

    // indexing operator:  Return by reference so that operator can be used in lhs
    // of assignments.  (No copying of return value here.)
    virtual int& operator[](int i) {

        if (i < 0 || i > size_) {

            std::cerr << "Illegal OC bound: " << i << std::endl ;
            return array_[0] ;
        }
        else
            return array_[i + first] ;

    }

    // remove method: decrease receiver size by removing an integer.  No action
    // taken if argument is not contained in receiver or if receiver is empty.
    virtual bool remove(int x) {

        if (size_ == 0) return false ;                  // receiver empty? Return false

        for (int i = first; i <= last ; i++) {          // Otherwise scan array_ for x

            if (array_[i] == x) {                       // x is found?

                for (int j = i; j > first; j--)         // Close the gap where x was by shifting
                    array_[j] = array_[j-1] ;           // elements to the left x to the right

                first++ ;                               // Update first and size_ and return true
                size_-- ;
                return true ;
            }
        }

        return false;                                   // x not found? return false
    }


    // add method: increase receiver size by adding integer x in position pos
    virtual OrderedCollection& add(int x, int pos) {

        int i;

        if (pos > size_ || pos < 0) {           // check whether pos is out of bounds
            std::cerr << "Illegal index position for addition: " << pos << ".\n" ;
            return *this ;
        }

        if (first > last) {                     // is receiver empty?
            first = last = alloc_size / 2 - 1;  // If yes, just store element in middle of
            array_[first] = x ;                 // array and return
            size_++;
            return *this ;
        }

        if (size_ == alloc_size) {              // Is receiver full? If yes, grow()
            std::cerr << "Exceeding limit! Must grow capacity.\n";
            grow() ;
        }

        i = first ;                             // use i to point to logical position
                                                // where x inserted

        if (first != 0) {                       // space available at front of array? If yes, insert there

            // Use j to count elements to be shifted left to create gap for x
            for (int j = 0; j < pos && i <= last ; i++, j++) {
                array_[i-1] = array_[i] ;       // shift element left
            }

            array_[i-1] = x ;                   // insert x in gap
            first-- ;                           // update first and size_
            size_++;
        }
        else if (last != alloc_size - 1)  {     // space available at back of array? If yes,
                                                // put at array_ back.
            for (i = last; i >= pos ; i-- ) {
                array_[i+1] = array_[i] ;
            }

            array_[pos] = x ;                   // insert at pos after creating gap
            last++ ;                            // updae last and size_
            size_++ ;
        }
        else {                                  // control flow should never get here
            std::cerr << "Checks failed" ;
            return *this ;
        }
        return *this ;
    }

protected:

    int* array_;                                // the array holding the values stored in receiver
    int first, last ;                           // the first and last being used in array_
    int alloc_size ;                            // the current size of array_

    // This function doubles the size of array_ when current array_ gets full
    void grow() {

        int* new_array = new int[alloc_size * 2] ;  // allocate new array_ twich the current size
        int new_first = alloc_size / 2 ;        // start using new array 1/4 of the way in
        alloc_size *= 2;                        // update size of array_
                                                // diagnostic output
        std::cout << "new_first is " << new_first << ".\n" ;

        // copy elements from old array to new one
        for(int i = first, j = new_first; i <= last; i++, j++) {
            new_array[j] = array_[i] ;
        }

        delete[] array_ ;                       // deallocate old array
        array_ = new_array ;                    // make new array the current one
        first = new_first ;                     // update first and last
        last = new_first + size_ - 1;
                                                // diagnostic output
        std::cout << "First and last are " << first << " and " << last << ".\n" ;

    }
};


// global var to test iterate()
int x = 0;

// file scope function to test function pointer (passed as an argument) in iterate()
void addAll(int i) {
    x += i ;
}

// the test program
int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << "Testing started!\n";

    Collection* temp = new OrderedCollection();

    temp->add(10, 0) ;          // insert in first position
    temp->add(200, 1) ;         // insert in second position
    temp->add(250, 1) ;         // insert in second position (move 2nd elem to 3rd place)

                                // print temp
    for (int i = 0; i <= temp->size() - 1; i++) {
        std::cout << "(*temp)[" << i << "] = " << (*temp)[i] << "\n" ;
    }

    Collection* temp2;
    temp2 = temp->copy() ;      // test copy method
    temp2->add(300,2) ;         // make sure deep copy was made

                                // print both OC instances
    for (int i = 0; i <= temp->size() - 1; i++) {
        std::cout << "(*temp)[" << i << "] = " << (*temp)[i] << "\n" ;
    }

    for (int i = 0; i <= temp2->size()- 1; i++) {
        std::cout << "(*temp2)[" << i << "] = " << (*temp2)[i] << "\n" ;
    }

    temp2->add(275,2) ;         // test grow() method

    for (int i = 0; i <= temp2->size()- 1; i++) {
        std::cout << "(*temp2)[" << i << "] = " << (*temp2)[i] << "\n" ;
    }

    // Repeat above test cases with Array instances
    // instead of OrderedCollection

    Collection** a ;            // try a polymorphic list;

    a = new Collection*[5] ;    // a elements can be Arrays or OCs

    a[0] = new OrderedCollection ;
    a[0]->add(100,0) ;
    a[0]->add(50,0) ;
    a[0]->add(75,1) ;
    a[1] = a[0]->copy() ;

    for (int i = 0; i <= a[1]->size()- 1; i++) {

        std::cout << "(*a[1])[" << i << "] = " << (*a[1])[i] << "\n" ;

    }

    x = 0;

    temp2->iterate(&addAll) ;   // try iterate() method with addAll() as argument

    std::cout << "x = " << x << ".\n" ;

    std::cout << "Does *temp contain 300? " << (temp->contains(300) ? "True" : "False") << "! \n" ;
    std::cout << "Does *temp2 contain 300? " << (temp2->contains(300) ? "True" : "False") << "! \n" ;

    delete temp ;               // cleanup dynamically allocated structures and test virtual destructors
    delete temp2 ;
    delete a[0]  ;
    delete a[1] ;
    delete[] a ;

    temp = new OrderedCollection;   // test use of operator[]() on lhs of assignment
    temp->add(9, 0);
    temp->add(10,0);
    temp->add(11,2);
    (*temp)[1] = 100;

    // 2nd element of temp should be 100 now.
    for (int i = 0; i <= temp->size() - 1; i++) {
        std::cout << "(*temp)[" << i << "] = " << (*temp)[i] << "\n" ;
    }

    delete temp;                // clean up temp

    // test remove functionality now
    temp = new OrderedCollection ;
    temp->add(10, 0) ;          // insert in first position
    temp->add(200, 1) ;         // insert in second position
    temp->add(250, 2) ;         // insert in third position
    temp->add(300, 3) ;         // insert in fourth position
    temp->add(5, 0) ;           // insert in first position, needs to grow
    temp->add(275, 4) ;         // insert in fifth position


    // print temp
    for (int i = 0; i <= temp->size() - 1; i++) {
        std::cout << "(*temp)[" << i << "] = " << (*temp)[i] << "\n" ;
    }

    temp->remove(5) ;          // remove test
    temp->remove(12) ;
    temp->remove(300) ;

    // print temp
    for (int i = 0; i <= temp->size() - 1; i++) {
        std::cout << "(*temp)[" << i << "] = " << (*temp)[i] << "\n" ;
    }


    return 0;                   // That's all folks!
}
