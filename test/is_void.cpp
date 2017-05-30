#include "test.h"

#include <inner/type_traits.h>


int main()
{
    assert(is_void<void>::value == true);
    assert(is_void<const void>::value == true);
    assert(is_void<volatile void>::value == true);
    assert(is_void<const volatile void>::value == true);
    assert(is_void<volatile const void>::value == true);
    assert(is_void<int>::value == false);
    assert(is_void<void*>::value == false);
    assert(is_void<bool>::value == false);
    return 0;
}
