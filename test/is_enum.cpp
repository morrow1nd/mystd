#include "test.h"

#include <inner/type_traits.h>

class A {};
enum E {};
enum class Ec : int {};


int main()
{
    assert(is_enum<A>::value == false);
    assert(is_enum<E>::value == true);
    assert(is_enum<Ec>::value == true);
    assert(is_enum<int>::value == false);
    return 0;
}
