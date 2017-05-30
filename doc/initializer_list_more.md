# Why can't achieve our own initializer_list in another namespace?

Try to compile this code in VS2015:
```cpp
namespace std {
template<class _Elem>
class initializer_list
{   // list of pointers to elements
    typedef unsigned int size_t;
public:
    typedef _Elem value_type;
    typedef const _Elem& reference;
    typedef const _Elem& const_reference;
    typedef size_t size_type;

    typedef const _Elem* iterator;
    typedef const _Elem* const_iterator;

    constexpr initializer_list() 
        : _First(0), _Last(0)
    {   // empty list
    }

    constexpr initializer_list(const _Elem *_First_arg,
        const _Elem *_Last_arg) noexcept
        : _First(_First_arg), _Last(_Last_arg)
    {   // construct with pointers
    }

    constexpr const _Elem *begin() const noexcept
    {   // get beginning of list
        return (_First);
    }

    constexpr const _Elem *end() const noexcept
    {   // get end of list
        return (_Last);
    }

    constexpr size_t size() const noexcept
    {   // get length of list
        return ((size_t)(_Last - _First));
    }

private:
    const _Elem *_First;
    const _Elem *_Last;
};
}

int main()
{
// this test code is copied from http://en.cppreference.com/w/cpp/utility/initializer_list/initializer_list
    std::initializer_list<int> empty_list;
    int a = empty_list.size();

    // create initializer lists using list-initialization
    std::initializer_list<int> digits = { 1, 2, 3, 4, 5 };
    int b = digits.size();

    // special rule for auto means 'fractions' has the
    // type std::initializer_list<double>
    auto fractions = { 3.14159, 2.71828 };
    int c = fractions.size();
    return 0;
}
```

It works.

Try in GCC. In my gcc 5.3.0, it can't go through the compiltation. Error info:
```
$ g++ Source.cpp  -std=c++14
Source.cpp:54:7: 致命错误：definition of std::initializer_list does not match #include <initializer_list>
 class initializer_list
       ^
编译中断。
```


Now, change the namespace `std` to other namespace, for example `mystd` in the above code. And test again, VS and gcc both show error info:
```
Source.cpp: 在函数‘int main()’中:
Source.cpp:119:58: 错误：could not convert ‘{1, 2, 3, 4, 5}’ from ‘<花括号内的初始值列表>’ to ‘stda::initializer_list<int>’
     stda::initializer_list<int> digits = { 1, 2, 3, 4, 5 };
                                                          ^
Source.cpp:124:41: 错误：从花括号中的初始化列表演绎需要 #include <initializer_list>
     auto fractions = { 3.14159, 2.71828 };
         
```