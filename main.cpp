#include "gc_pointer.h"
#include "LeakTester.h"

int main() {

    int *ptr = new int(123);

    int *ptr2 = new int(999);

    // calls constructor
    Pointer<int> a(ptr);// 1 ref to ptr

    // calls constructor
    Pointer<int> b(ptr);// 2 ref to ptr

    // calls copy constructor
    Pointer<int> c(b);// 3 ref to ptr

    // calls copy constructor
    Pointer<int> d(c);// 4 ref to ptr

    std::cout << std::endl;

    // calls constructor
    Pointer<int> e = new int(468);

    // calls assignment overload Pointer = Pointer
    e = d;// 5 ref to ptr

    // calls assignment overload Pointer = Pointer
    c = b;// 5 ref to ptr

    // calls assignment overload Pointer = *ptr
    b = ptr;// 5 ref to ptr

    // calls assignment overload Pointer = *ptr
    b = ptr2;// 4 ref to ptr, 1 ref to ptr2

    return 0;
}