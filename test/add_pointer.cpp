#include "test.h"

#include <inner/type_traits.h>

#include <iostream>


int main()
{
    int i = 123;
    int& ri = i;
    typedef add_pointer<decltype(i)>::type IntPtr;
    typedef add_pointer<decltype(ri)>::type IntPtr2;
    IntPtr pi = &i;
    std::cout << "i = " << i << "\n";
    std::cout << "*pi = " << *pi << "\n";
 
    static_assert(is_pointer<IntPtr>::value, "IntPtr should be a pointer");
    static_assert(is_same<IntPtr, int*>::value, "IntPtr should be a pointer to int");
    static_assert(is_same<IntPtr2, IntPtr>::value, "IntPtr2 should be equal to IntPtr");
 
    typedef remove_pointer<IntPtr>::type IntAgain;
    IntAgain j = i;
    std::cout << "j = " << j << "\n";
 
    static_assert(!is_pointer<IntAgain>::value, "IntAgain should not be a pointer");
    static_assert(is_same<IntAgain, int>::value, "IntAgain should be equal to int");
    return 0;
}
