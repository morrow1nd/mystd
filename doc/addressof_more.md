# more info about addressof of mystd's achievement.

test code.
```cpp
#include <iostream>

namespace mytest {
typedef decltype(nullptr) nullptr_t;

template<typename T>
struct addressof_impl
{
    static inline T* f(T& v, long)
    {
        return reinterpret_cast<T*>(
            &const_cast<char&>(reinterpret_cast<const volatile char &>(v)));
    }
    static inline T* f(T* v, int) /* for function name */
    {
        std::cout << "a function name" << std::endl;
        return v;
    }
};
template<> struct addressof_impl<nullptr_t>
{
    static inline nullptr_t* f(nullptr_t& v, int) { return &v; }
};
template<> struct addressof_impl<nullptr_t const>
{
    static inline nullptr_t const* f(nullptr_t const& v, int) { return &v; }
};
template<> struct addressof_impl<nullptr_t volatile>
{
    static inline nullptr_t volatile* f(nullptr_t volatile& v, int) { return &v; }
};
template<> struct addressof_impl<nullptr_t const volatile>
{
    static inline nullptr_t const volatile* f(nullptr_t const volatile& v, int) { return &v; }
};

template<typename T>
inline T* addressof(T& v) noexcept
{
    return addressof_impl<T>::f(v, 0);
}
template<typename T>
T* addressof(const T&&) = delete;
}


void func()
{
}

int globala;

struct foo
{
    int a;
    foo() : a(0) {}
};

int main()
{
    auto ret = mytest::addressof(func);
    std::cout << *ret << std::endl;
    auto funcp = &func; // funcp ->  void(*)()
    auto ret2 = mytest::addressof(funcp);
    std::cout << *ret2 << std::endl;
    // auto ret4 = mytest::addressof(&func); // error
    // auto ret3 = mytest:;addressof(foo().a); // error
    foo f;
    auto ret5 = mytest::addressof(f.a);
    std::cout << *ret5 << std::endl;
    return 0;
}

```