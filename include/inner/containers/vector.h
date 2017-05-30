#pragma once

#include "../mystd.h"
#include "../memory/allocators.h"

#include <initializer_list> // std::initializer_list is special to the compiler, we can't achieve it in namespace mystd. see doc/initializer_list_more.md
#include <cstddef> // size_t, ptrdiff_t


MYSTD_NS_BEGIN

using std::initializer_list;
using std::size_t;
using std::ptrdiff_t;


template<typename T, typename Allocator = allocator<T>>
class vector
{
public:
    typedef T           value_type;
    typedef Allocator   allocator_type;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef allocator_traits<Allocator>::pointer        pointer;
    typedef allocator_traits<Allocator>::const_pointer  const_pointer;
    typedef             iterator;
    typedef             const_iterator;
    typedef reverse_iterator<iterator>          reverse_iterator;
    typedef reverse_iterator<const_iterator>    const_reverse_iterator;

    vector() noexcept
    
};

MYSTD_NS_END
