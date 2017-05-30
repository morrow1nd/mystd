#include "test.h"

#include <inner/type_traits.h>
 
class Foo {
    int v1;
    double v2;
 public:
    Foo(int n) : v1(n), v2() {}
    Foo(int n, double f) noexcept : v1(n), v2(f) {}
};
 
int main() {
    // std::cout << "Foo is ...\n" << std::boolalpha
    //           << "\tTrivially-constructible from const Foo&? "
    //           << std::is_trivially_constructible<Foo, const Foo&>::value << '\n'
    //           << "\tTrivially-constructible from int? "
    //           << std::is_trivially_constructible<Foo, int>::value << '\n'
    //           << "\tConstructible from int? "
    //           << std::is_constructible<Foo, int>::value << '\n'
    //           << "\tNothrow-constructible from int? "
    //           << std::is_nothrow_constructible<Foo, int>::value << '\n'
    //           << "\tNothrow-constructible from int and double? "
    //           << std::is_nothrow_constructible<Foo, int, double>::value << '\n';

    test(is_trivially_constructible<Foo, const Foo&>::value == true);
    test(is_trivially_constructible_v<Foo, int> == false);
    test(is_constructible_v<Foo, int> == true);
    test(is_nothrow_constructible_v<Foo, int> == false);
    test(is_nothrow_constructible_v<Foo, int, double> == true);
    return 0;
}