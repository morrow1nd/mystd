#include "test.h"

#include <inner/type_traits.h>


int main()
{
    assert(is_integral<float>::value == false);
    assert(is_integral<int>::value == true);
    assert(is_integral<signed long>::value == true);
    assert(is_integral<signed long long>::value == true);
    assert(is_integral<double>::value == false);
    assert(is_integral<unsigned char>::value == true);

    assert(is_integral<const char>::value == true);
    assert(is_integral<volatile unsigned char>::value == true);
    assert(is_integral<void>::value == false);
    return 0;
}
