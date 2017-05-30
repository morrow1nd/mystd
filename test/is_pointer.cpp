#include "test.h"

#include <inner/type_traits.h>


int main()
{
    assert(is_pointer<nullptr_t>::value == false);
    assert(is_pointer<int*>::value == true);
    assert(is_pointer<void*>::value == true);
    assert(is_pointer<void>::value == false);
    return 0;
}
