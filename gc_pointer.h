#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include <assert.h>
#include <algorithm>
#include "gc_details.h"
#include "gc_iterator.h"

/*
    Pointer implements a pointer type that uses
    garbage collection to release unused memory.
    A Pointer must only be used to point to memory
    that was dynamically allocated using new.
    When used to refer to an allocated array,
    specify the array size.
*/
template<class T, unsigned size = 0>
class Pointer {
private:
    // true when first Pointer is created
    static bool first;
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T> > refContainer;

    // Return an iterator to pointer details in refContainer.
    typename std::list<PtrDetails<T> >::iterator findPtrInfo(T *ptr);

    // addr points to the allocated memory to which this Pointer pointer currently points.
    T *addr;
    // isArray is true if this Pointer points to an allocated array. It is false otherwise.
    bool isArray;
    // size of the array
    unsigned arraySize;
public:
    // Define an iterator type for Pointer<T>.
    typedef Iter<T> GCiterator;

    // Empty constructor
    // NOTE: templates aren't able to have prototypes with default arguments
    // this is why constructor is designed like this:
    Pointer() {
        Pointer(NULL);
    }

    // Constructor with pointer parameter
    Pointer(T *);

    // Copy constructor.
    Pointer(const Pointer &);

    // Destructor for Pointer.
    ~Pointer();

    // Collect garbage. Returns true if at least one object was freed.
    static bool collect();

    // Overload assignment of pointer to Pointer.
    T *operator=(T *);

    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(const Pointer &);

    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*() {
        return *addr;
    }

    // Return the address being pointed to.
    T *operator->() { return addr; }

    // Return a reference to the object at the
    // index specified by i.
    T &operator[](int i) { return addr[i]; }

    // Conversion function to T *.
    operator T *() { return addr; }

    // Return an Iter to the start of the allocated memory.
    Iter<T> begin() {
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }

    // Return an Iter to one past the end of an allocated array.
    Iter<T> end() {
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }

    // Return the size of refContainer for this type of Pointer.
    static unsigned long refContainerSize() {
        return refContainer.size();
    }

    // A utility function that displays refContainer.
    static void showlist();

    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template<class T, unsigned size>
std::list<PtrDetails<T> > Pointer<T, size>::refContainer;
template<class T, unsigned size>
bool Pointer<T, size>::first = true;

// Constructor for both initialized and uninitialized objects. -> see class interface
template<class T, unsigned size>
Pointer<T, size>::Pointer(T *ptr) : addr(ptr), isArray(size > 0), arraySize(size) {
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;

    // TODO: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab

    auto p = findPtrInfo(addr);

    if (p == refContainer.end()) {
        refContainer.emplace_front(PtrDetails<T>(addr, size));
    } else {
        // must have found one here, so we must increment its refcount
        p->refcount++;
    }

}

// Copy constructor.
template<class T, unsigned size>
Pointer<T, size>::Pointer(const Pointer &pointer) {
    // TODO: Implement Pointer constructor
    // Lab: Smart Pointer Project Lab

    addr = pointer.addr;
    isArray = pointer.isArray;
    arraySize = pointer.arraySize;

    auto p = findPtrInfo(addr);

    assert(p != refContainer.end());

    p->refcount++;
}

// Destructor for Pointer.
template<class T, unsigned size>
Pointer<T, size>::~Pointer() {

    // TODO: Implement Pointer destructor
    // Lab: New and Delete Project Lab
    auto p = findPtrInfo(addr);

    // must exist
    assert(p != refContainer.end());

    p->refcount--;

    /*if (p->refcount == 0) {
        p->isArray ? delete[] p->memPtr : delete p->memPtr;
        refContainer.remove(*p);
    }*/

    collect();
}

// Collect garbage. Returns true if at least
// one object was freed.
template<class T, unsigned size>
bool Pointer<T, size>::collect() {

    // based on answer @https://stackoverflow.com/a/4478653/8965434

    // TODO: Implement collect function
    // LAB: New and Delete Project Lab
    // Note: collect() will be called in the destructor
    bool deleted = false;

    auto removeIf = [&deleted](const PtrDetails<T> &ptrDetails) {
        if (ptrDetails.refcount == 0) {
            ptrDetails.isArray ? delete[] ptrDetails.memPtr : delete ptrDetails.memPtr;
            deleted = true;
            return true;
        }
        return false;
    };

    // move unwanted items to end
    auto new_end = std::remove_if(refContainer.begin(), refContainer.end(), removeIf);

    // erase items from end of list
    refContainer.erase(new_end, refContainer.end());

    return deleted;
}

// Overload assignment of pointer to Pointer.
template<class T, unsigned size>
T *Pointer<T, size>::operator=(T *ptr) {

    // TODO: Implement operator==
    // LAB: Smart Pointer Project Lab

    // other Pointer instance is using same ptr address, nothing to do
    if (addr == ptr) {
        return *this;
    }

    auto p = findPtrInfo(addr);

    // must exist
    assert(p != refContainer.end());

    p->refcount--;

    collect();

    addr = ptr;

    p = findPtrInfo(addr);

    if (p == refContainer.end()) {
        refContainer.emplace_front(PtrDetails<T>(addr, size));
    } else {
        // must have found one here, so we must increment its refcount
        p->refcount++;
    }

    return *this;
}

// Overload assignment of Pointer to Pointer.
template<class T, unsigned size>
Pointer<T, size> &Pointer<T, size>::operator=(const Pointer &pointer) {

    // TODO: Implement operator==
    // LAB: Smart Pointer Project Lab

    // other Pointer instance is using same ptr address, nothing to do
    if (addr == pointer.addr) {
        return *this;
    }

    auto p = findPtrInfo(addr);

    // must exist
    assert(p != refContainer.end());

    p->refcount--;

    collect();

    addr = pointer.addr;

    p = findPtrInfo(addr);

    // must exist
    assert(p != refContainer.end());

    p->refcount++;

    return *this;
}

// A utility function that displays refContainer.
template<class T, unsigned size>
void Pointer<T, size>::showlist() {
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end()) {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++) {
        std::cout << "[" << (void *) p->memPtr << "]"
                  << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Find a pointer in refContainer.
template<class T, unsigned size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr) {
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++) {
        if (p->memPtr == ptr) {
            return p;
        }
    }

    return p;
}

// Clear refContainer when program exits.
template<class T, unsigned size>
void Pointer<T, size>::shutdown() {
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++) {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}