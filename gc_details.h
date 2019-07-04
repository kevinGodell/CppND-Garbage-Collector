// This class defines an element that is stored
// in the garbage collection information list.
//
template<class T>
class PtrDetails {
public:
    unsigned refcount;  // current reference count
    T *memPtr;          // pointer to allocated memory
    bool isArray;       // isArray is true if memPtr points to an allocated array. It is false otherwise.
    unsigned arraySize; // If memPtr is pointing to an allocated array, then arraySize contains its size

    PtrDetails(T *ptr, unsigned size = 0) {
        // TODO: Implement PtrDetails
        refcount = 1;
        memPtr = ptr;
        isArray = size > 0;
        arraySize = size;
    }
};

// Overloading operator== allows two class objects to be compared.
// This is needed by the STL list class.
template<class T>
bool operator==(const PtrDetails<T> &ob1, const PtrDetails<T> &ob2) {
    // TODO-ne: Implement operator==
    return ob1.memPtr == ob2.memPtr;// we only care that it is same memory location
}