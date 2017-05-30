#include "test.h"

#include <inner/type_traits.h>

int main()
{
    assert(is_null_pointer<nullptr_t>::value == true);
    assert(is_null_pointer<int*>::value == false);
    return 0;
}
