#pragma once

#include "mystd.h"
#include <cstddef> // ptrdiff_t
#include <iterator> // istream_iterator, ostream_iterator, istreambuf_iterator, ostreambuf_iterator

MYSTD_NS_BEGIN

using std::ptrdiff_t;


struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

struct output_iterator_tag {};

template<typename Category,
    typename T,
    typename Distance = ptrdiff_t,
    typename Pointer = T*,
    typename Reference = T&>
struct iterator
{
    typedef Category    iterator_category;
    typedef T           value_type;
    typedef Distance    difference_type;
    typedef Pointer     pointer;
    typedef Reference   reference;
};


template<typename Iterator>
struct iterator_traits_base<Iterator, void_t<
        typename Iterator::iterator_category,
        typename Iterator::value_type,
        typename Iterator::difference_type,
        typename Iterator::pointer,
        typename Iterator::reference>>
{
    typedef typename Iterator::value_type           value_type;
    typedef typename Iterator::difference_type      difference_type;
    typedef typename Iterator::pointer              pointer;
    typedef typename Iterator::reference            reference;
    typedef typename Iterator::iterator_category    iterator_category;
};

template<typename Iterator>
struct iterator_traits : iterator_traits_base
{
    // get traits from Iterator, if possible
};

template<typename T>
struct iterator_traits<T*>
{
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef T*                          pointer;
    typedef T&                          reference;
};

template<typename T>
struct iterator_traits<const T*>
{
    typedef random_access_iterator_tag  iterator_category;
    typedef T                           value_type;
    typedef ptrdiff_t                   difference_type;
    typedef const T*                    pointer;
    typedef const T&                    reference;
};

template<typename Iter>
using iterator_value_t = typename iterator_traits<Iter>::value_type;
template<typename Iter>
using iterator_diff_t = typename iterator_traits<Iter>::difference_type;
template<typename Iter>
using iterator_category_t = typename iterator_traits<Iter>::iterator_category;


using std::reverse_iterator; // todo
using std::move_iterator;
using std::back_insert_iterator;
using std::front_insert_iterator;
using std::insert_iterator;

using std::make_reverse_iterator;
using std::make_move_iterator;
using std::font_inserter;
using std::back_inserter;
using std::inserter;


using std::istream_iterator;
using std::ostream_iterator;
using std::istreambuf_iterator;
using std::ostreambuf_iterator;

using std::beign;
using std::cbegin;
using std::end;
using std::cend;
using std::rbegin;
using std::crbegin;
using std::rend;
using std::crend;

// todo: operator == != <= ...


template<typename InputIt, typename Diff>
inline void advance(InputIt& it, Diff offset, input_iterator_tag)
{
    if(offset < 0)
        ; // Error("negative offset in advance");
    for(; 0 < offset; --offset)
        ++it;
}

template<typename BidIt, typename Diff>
inline void advance(BidIt& it, Diff offset, bidirectional_iterator_tag)
{
    for(; 0 < offset; --offset)
        ++it;
    for(; offset < 0; ++offset)
        --it;
}

template<typename RanIt, typename Diff>
inline void advance(Ranit& it, Diff offset, random_access_iterator_tag)
{
    it += offset;
}

template<typename InputIt, typename Diff>
inline void advance(InputIt& it, Diff offset)
{
    advance_helper(it, offset, iterator_category_t<remove_const_t<InputIt>>());
}


template<typename InputIt>
inline iterator_diff_t<InputIt> distance_helper(InputIt first, InputIt last, input_iterator_tag)
{
    iterator_diff_t<InputIt> off = 0;
    for(; first != last; ++first)
        ++off;
    return off;
}

template<typename RanIt>
inline iterator_diff_t<RanIt> distance_helper(RanIt first, RanIt last, random_access_iterator_tag)
{
    return last - first;
}

template<typename InputIt>
inline typename iterator_diff_t<InputIt> distance(InputIt first, InputIt last)
{
    return distance_helper(first, last, iterator_category_t<InputIt>());
}


template<typename InputIt>
inline InputIt next(InputIt first, iterator_diff_t<InputIt> offset = 1)
{
    static_assert(is_base_of<input_iterator_tag,
        iterator_category_t<InputIt>>::value,
        "next requires input iterator");
    advance(first, offset);
    return first;
}

template<typename BidIt>
inline BidIt prev(BidIt first, iterator_diff_t<BidIt> offset = 1)
{
    static_assert(is_base_of<bidirectional_iterator_tag,
        iterator_category_t<BidIt>>::value,
        "prev requires bidirectional iterator");
    advance(first, -offset);
    return first;
}


MYSTD_NS_END
