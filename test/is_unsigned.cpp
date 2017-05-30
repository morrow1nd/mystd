#include "test.h"

#include <inner/type_traits.h>

class A {};
enum E : unsigned {};
enum class Ec : unsigned {};
enum Ecs : signed int {};

int main()
{
    assert(is_unsigned_v<A> == false);
    assert(is_unsigned_v<float> == false);
    assert(is_unsigned_v<signed int> == false);
    assert(is_unsigned_v<unsigned int> == true);
    assert(is_unsigned_v<E> == false);
    assert(is_unsigned_v<Ec> == false);
    assert(is_unsigned_v<Ecs> == false);
    return 0;
}
