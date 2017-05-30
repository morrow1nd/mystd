#pragma once

#include "../mystd.h"
#include "../type_traits.h"
#include <cstddef> // size_t, ptrdiff_t
#include <utility> // forward, declval
#include <limits> // numeric_limits


MYSTD_NS_BEGIN

using std::size_t;
using std::ptrdiff_t;
using std::forward;
using std::numeric_limits;
using std::declval;


template<typename T>
class allocator
{
public:
    static_assert(!is_const_v<T>, "The C++ Standard forbids containers of const elements"
        "because allocator<const T> is ill-formed.");

    typedef void _Not_user_specialized; // needed by _Is_default_allocator


    typedef T               value_type;
    typedef value_type*     pointer;
    typedef const T*        const_pointer;
    typedef value_type&     reference;
    typedef const T&        const_reference;
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef true_type       propagate_on_container_move_assignment;
    typedef true_type       is_always_equal;

    template<typename Other>
    struct rebind { typedef allocator<Other> other; };

    allocator() noexcept {}
    allocator(const allocator<value_type>&) noexcept {}
    template<typename Other>
    allocator(const allocator<Other>&) noexcept {}
    template<typename Other>
    allocator<value_type>& operator=(const allocator<Other>&) { return *this; }

    void deallocate(pointer p, size_type count)
    {
        ::operator delete(p);
    }
    pointer allocate(size_type count)
    {
        if(count == 0) return 0;
        void* ptr = ::operator new(count * sizeof(T));
        return static_cast<pointer>(ptr);
    }
    pointer allocate(size_type count, const void *) { return allocate(count); }

    template<typename Obj, typename... Args>
    void construct(Obj* ptr, Args&&... args)
    {
        ::new ((void *)ptr) Obj(forward<Args>(args)...);
    }
    template<typename U>
    void destroy(U* p)
    {
        p->~U();
    }

    size_type max_size() const noexcept
    {
        return (size_type)(-1) / sizeof(T);
    }
};

template<>
class allocator<void>
{
public:
    typedef void value_type;
    typedef void* pointer;
    typedef const void* const_pointer;

    template<typename Other>
    struct rebind { typedef allocator<Other> other; };

    // Specialization for void lacks the member typedefs reference, const_reference, 
    // size_type and difference_type. This specialization declares no member functions.
};


template<typename T1, typename T2>
inline bool operator==(const allocator<T1>&, const allocator<T2>&) noexcept
{
    return true; // Since default allocators are stateless, two default allocators are always equal.
}
template<typename T1, typename T2>
inline bool operator!=(const allocator<T1>&, const allocator<T2>&) noexcept { return false; }


namespace detail {
// addressof_impl is copied and simplified from boost 1.62.0 core/addressof.hpp
// http://stackoverflow.com/a/6495205/273767
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
        return v;
    }
};

template<> struct addressof_impl<nullptr_t>
{ static inline nullptr_t*                  f(nullptr_t& v,                 int) { return &v; } };

template<> struct addressof_impl<nullptr_t const>
{ static inline nullptr_t const*            f(nullptr_t const& v,           int) { return &v; } };

template<> struct addressof_impl<nullptr_t volatile>
{ static inline nullptr_t volatile*         f(nullptr_t volatile& v,        int) { return &v; } };

template<> struct addressof_impl<nullptr_t const volatile>
{ static inline nullptr_t const volatile*   f(nullptr_t const volatile& v,  int) { return &v; } };

} // namespace detail


template<typename T>
inline T* addressof(T& v) noexcept
{
    return detail::addressof_impl<T>::f(v, 0);
}
template<typename T>
T* addressof(const T&&) = delete;


template<typename Ptr>
struct pointer_traits
{
    // Ptr: Pointer-like type, smart_ptr...
private:
    template<typename T>
    struct get_first_parameter;
    template<template<typename, typename...> typename T,
        typename First,
        typename... Rest>
    struct get_first_parameter<T<First, Rest...>>
    { typedef First type; };

    template<typename NewFirst,
        typename T>
    struct replace_first_parameter;
    template<typename NewFirst,
        template<typename, typename...> typename T,
        typename First,
        typename... Rest>
    struct replace_first_parameter<NewFirst, T<First, Rest...>>
    { typedef T<NewFirst, Rest...> type; };


    template<typename P>
    static typename P::element_type element_type_helper(P*);
    static typename get_first_parameter<Ptr>::type element_type_helper(...);

    template<typename P>
    static typename P::difference_type difference_type_helper(P*);
    static ptrdiff_t difference_type_helper(...);

    // __ptrtr_rebind_helper is copied from gcc 5.3.0 ptr_traits.h
    template<typename _Ptr, typename U>
    class __ptrtr_rebind_helper
    {
        template<typename __Ptr, typename U2>
        static constexpr true_type
        _S_check(typename __Ptr::template rebind<U2>*);

        template<typename, typename>
        static constexpr false_type
        _S_check(...);
    public:
        typedef decltype(_S_check<_Ptr, U>(nullptr)) result;
    };

    template<typename T, typename U,
        bool = __ptrtr_rebind_helper<T, U>::result::value>
    struct __ptrtr_rebind;
    template<typename T, typename U>
    struct __ptrtr_rebind<T, U, true>
    {
        typedef typename T::template rebind<U> __type;
    };
    template<template<typename, typename...> typename _SomePtr, typename _Tp,
            typename _Up, typename... _Args>
    struct __ptrtr_rebind<_SomePtr<_Tp, _Args...>, _Up, false>
    {
        typedef _SomePtr<_Up, _Args...> __type;
    };

public:
    typedef Ptr pointer;
    typedef decltype(element_type_helper((Ptr*)0)) element_type;
    typedef decltype(difference_type_helper((Ptr*)0)) difference_type;

    template<typename OtherPtr>
    using rebind = typename __ptrtr_rebind<Ptr, OtherPtr>::__type;

    typedef conditional_t<is_void_v<element_type>,
        char&,
        add_lvalue_reference_t<element_type>> _ref_type;

    static pointer pointer_to(_ref_type r)
    {
        return Ptr::pointer_to(r);
    }
};

template<typename T>
struct pointer_traits<T*>
{   
    // T: pointed type
    typedef T* pointer;
    typedef T element_type;
    typedef ptrdiff_t difference_type;
    
    template<typename U>
    using rebind = U*;

    typedef conditional_t<is_void_v<T>,
        char&,
        add_lvalue_reference_t<T>> _ref_type;

    static pointer pointer_to(_ref_type r) noexcept
    {
        return addressof(r);
    }
};


template<typename Allocator>
struct allocator_traits
{
    // MSVC(14.0) also defines a specialization for allocator_tratis<allocator<T>>.
    // It can increases compiler speed. See VC/include/xmemory0

    typedef Allocator allocator_type;
    typedef typename Allocator::value_type value_type;

#define _GET_TYPE_OR_DEFAULT(TYPE, DEFAULT)                             \
private:                                                                \
    static typename allocator_type::TYPE     TYPE##_helper(allocator_type*);      \
    static DEFAULT                      TYPE##_helper(...);             \
    typedef decltype(TYPE##_helper((allocator_type*)0)) __##TYPE;            \
public:


    // template<typename Alloc>
    // static typename Alloc::pointer pointer_helper(Alloc*);
    // static value_type* pointer_helper(...);

    // template<typename Alloc>
    // static typename Alloc::const_pointer const_pointer_helper(Alloc*);
    // static typename pointer_traits<pointer>::template rebind<const value_type> const_pointer_helper(...);

    // template<typename Alloc>
    // static typename Alloc::void_pointer void_pointer_helper(Alloc*);
    // static typename pointer_traits<pointer>::template rebind<void> void_pointer_helper(...);

    // template<typename Alloc>
    // static typename Alloc::const_void_pointer const_void_pointer_helper(Alloc*);
    // static typename pointer_traits<pointer>::template rebind<const void> void_pointer_helper(...);

    // template<typename Alloc>
    // static typename Alloc::difference_type difference_type_helper(Alloc*);
    // static typename pointer_traits<pointer>::difference_type difference_type_helper(...);

    // template<typename Alloc>
    // static typename Alloc::size_type size_type_helper(Alloc*);
    // static make_unsigned_t<difference_type> size_type_helper(...);

public:

    // typedef decltype(pointer_helper((Allocator*)0)) pointer;
    // typedef decltype(const_pointer_helper((Allocator*)0)) const_pointer;
    // typedef decltype(void_pointer_helper((Allocator*)0)) void_pointer;
    // typedef decltype(const_void_pointer_helper((Allocator*)0)) const_void_pointer;
    // typedef decltype(difference_type_helper((Allocator*)0)) difference_type;
    // typedef decltype(size_type_helper((Allocator*)0)) size_type;

    _GET_TYPE_OR_DEFAULT(pointer, value_type*)
    typedef __pointer pointer;

    _GET_TYPE_OR_DEFAULT(const_pointer,
        typename pointer_traits<pointer>::template rebind<const value_type>)
    typedef __const_pointer const_pointer;

    _GET_TYPE_OR_DEFAULT(void_pointer,
        typename pointer_traits<pointer>::template rebind<void>)
    typedef __void_pointer  void_pointer;

    _GET_TYPE_OR_DEFAULT(const_void_pointer,
        typename pointer_traits<pointer>::template rebind<const void>)
    typedef __const_void_pointer const_void_pointer;

    _GET_TYPE_OR_DEFAULT(difference_type,
        typename pointer_traits<pointer>::difference_type)
    typedef __difference_type difference_type;

    _GET_TYPE_OR_DEFAULT(size_type,
        make_unsigned_t<difference_type>)
    typedef __size_type size_type;

    _GET_TYPE_OR_DEFAULT(propagate_on_container_move_assignment,
        false_type)
    typedef __propagate_on_container_move_assignment propagate_on_container_move_assignment;

    _GET_TYPE_OR_DEFAULT(propagate_on_container_swap,
        false_type)
    typedef __propagate_on_container_swap propagate_on_container_swap;

    _GET_TYPE_OR_DEFAULT(is_always_equal,
        typename is_empty<allocator_type>::type)
    typedef __is_always_equal is_always_equal;

private:
    template<typename Alloc, typename T>
    class alloctr_rebind_helper
    {
        template<typename Alloc2, typename T2>
        static constexpr true_type
        _S_check(typename Alloc2::template rebind<T2>::other*);

        template<typename, typename>
        static constexpr false_type
        _S_check(...);

    public:
        typedef decltype(_S_check<Alloc, T>(nullptr)) __type;
    };
    template<typename Alloc, typename T,
        bool = alloctr_rebind_helper<Alloc, T>::__type::value>
    struct alloctr_rebind;
    template<typename Alloc, typename T>
    struct alloctr_rebind<Alloc, T, true>
    {
        typedef typename Alloc::template rebind<T>::other __type;
    };
    template<template<typename, typename...> typename Alloc,
        typename T,
        typename U,
        typename... Args>
    struct alloctr_rebind<Alloc<U, Args...>, T, false>
    {
        typedef Alloc<T, Args...> __type;
    };

public:
    template<typename T>
    using rebind_alloc = typename alloctr_rebind<allocator_type, T>::__type;
    template<typename T>
    using rebind_tratis = allocator_traits<rebind_alloc<T>>;

private:
    struct allocate_helper
    {
        template<typename Alloc,
            typename Size_type,
            typename Const_void_pointer>
        static auto Fn(int, Alloc& a, Size_type count, Const_void_pointer hint)
                -> decltype(a.allocate(count, hint))
        {
            return a.allocate(count, hint);
        }

        template<typename Alloc,
            typename Size_type,
            typename Const_void_pointer>
        static auto Fn(detail::_Wrap_int, Alloc& a, Size_type count, Const_void_pointer)
                -> decltype(a.allocate(count))
        {
            return a.allocate(count);
        } 
    };

    static pointer allocate(allocator_type& a, size_type count)
    {
        return a.allocate(count);
    }
    static pointer allocate(allocator_type& a, size_type count, const_void_pointer hint)
    {
        return allocate_helper::Fn(0, a, count, hint);
    }

    static void deallocate(allocator_type& a, pointer ptr, size_type count)
    {
        a.deallocate(ptr, count);
    }

public:
    template<typename T, typename... Args>
    static auto construct(allocator_type& a, T* ptr, Args&&... args)
            -> decltype(construct_helper(a, ptr, forward<Args>(args)...))
    {
        construct_helper(a, ptr, forward<Args>(args)...);
    }


private:
            // STRUCT TEMPLATE _Unwrap_alloc
    template<class _Ty>
        class allocator;
    template<class _Alloc>
        struct _Wrap_alloc;

    template<class _Alloc>
        struct _Unwrap_alloc
        {   // extracts a wrapped allocator type if it is wrapped; otherwise, passes it through unchanged
        typedef _Alloc type;
        };

    template<class _Alloc>
        struct _Unwrap_alloc<_Wrap_alloc<_Alloc>>
        {   // extracts a wrapped allocator type if it is wrapped; otherwise, passes it through unchanged
        typedef _Alloc type;
        };

    // TEMPLATE STRUCT _Param_tester
template<class...>
    struct _Param_tester
    {   // test if parameters are valid
    typedef void type;
    };

    // ALIAS TEMPLATE void_t
template<class... _Types>   // TRANSITION, VSO#138046
    using void_t = typename _Param_tester<_Types...>::type;


            // ALIAS TEMPLATE _Unwrap_alloc_t
    template<class _Alloc>
        using _Unwrap_alloc_t = typename _Unwrap_alloc<_Alloc>::type;

        // STRUCT TEMPLATE _Is_default_allocator
template<class _Alloc,
    class = void>
    struct _Is_default_allocator
        : false_type
    {   // tests whether _Alloc is non-specialized default allocator (N4582 20.9.9 [default.allocator])
    };

template<class _Ty>
    struct _Is_default_allocator<allocator<_Ty>, typename allocator<_Ty>::_Not_user_specialized>
        : true_type
    {   // tests whether _Alloc is non-specialized default allocator (N4582 20.9.9 [default.allocator])
    };

            // ALIAS TEMPLATES _Uses_default_construct AND _Uses_default_construct_t
    struct _Has_no_alloc_construct_tag
        {   // TRANSITION, C1XX
        };

    template<class _Void,
        class... _Types>
        struct _Has_no_alloc_construct
            : true_type
        {   // determines whether _Alloc has no construct
        };

    template<class _Alloc,
        class _Ptr,
        class... _Args>
        struct _Has_no_alloc_construct<
            void_t<
                _Has_no_alloc_construct_tag,    // TRANSITION, C1XX
                decltype( declval<_Alloc&>().construct( declval<_Ptr>(),  declval<_Args>()...))>,
            _Alloc, _Ptr, _Args...>
            : false_type
        {   // determines whether _Alloc has no construct
        };

    template<class _Alloc,
        class _Ptr,
        class... _Args>
        using _Uses_default_construct = detail::disjunction<
            _Is_default_allocator<_Alloc>,
            _Has_no_alloc_construct<void, _Alloc, _Ptr, _Args...>>;

    template<class _Alloc,
        class _Ptr,
        class... _Args>
        using _Uses_default_construct_t = typename _Uses_default_construct<_Alloc, _Ptr, _Args...>::type;


            // ALIAS TEMPLATE _Uses_default_destroy AND _Uses_default_destroy_t
    struct _Has_no_alloc_destroy_tag
        {   // TRANSITION, C1XX
        };

    template<class _Alloc,
        class _Ptr,
        class = void>
        struct _Has_no_alloc_destroy
            : true_type
        {   // determines whether _Alloc has no destroy
        };

    template<class _Alloc,
        class _Ptr>
        struct _Has_no_alloc_destroy<_Alloc, _Ptr, void_t<
                _Has_no_alloc_destroy_tag,  // TRANSITION, C1XX
                decltype(declval<_Alloc&>().destroy(declval<_Ptr>()))>>
            : false_type
        {   // determines whether _Alloc has no destroy
        };

    template<class _Alloc,
        class _Ptr>
        using _Uses_default_destroy = detail::disjunction<
            _Is_default_allocator<_Alloc>,
            _Has_no_alloc_destroy<_Alloc, _Ptr>>;

    template<class _Alloc,
        class _Ptr>
        using _Uses_default_destroy_t = typename _Uses_default_destroy<_Alloc, _Ptr>::type;

    template<class _Ty,
        class... _Types>
        static void _Construct1(true_type, allocator_type&, _Ty *_Ptr,
            _Types&&... _Args)
        {   // construct _Ty(_Types...) at _Ptr, default version
        ::new (static_cast<void *>(_Ptr))
            _Ty(forward<_Types>(_Args)...);
        }

    template<class _Ty,
        class... _Types>
        static void _Construct1(false_type, allocator_type& _Al, _Ty *_Ptr,
            _Types&&... _Args)
        {   // construct _Ty(_Types...) at _Ptr, allocator-supplied version
        _Al.construct(_Ptr, forward<_Types>(_Args)...);
        }
    template<class _Ty>
        static void _Destroy1(allocator_type&, _Ty *_Ptr, true_type)
        {   // destroy object at _Ptr, default version
        _Ptr->~_Ty();
        }

    template<class _Ty>
        static void _Destroy1(allocator_type& _Al, _Ty *_Ptr, false_type)
        {   // destroy object at _Ptr, allocator-supplied version
        _Al.destroy(_Ptr);
        }

public:
    template<typename T, typename... Args>
    static void construct(allocator_type& a, T* ptr, Args&&... args)
    {
        _Construct1(_Uses_default_construct_t<_Unwrap_alloc_t<allocator_type>, T*, Args...>(),
            a, ptr, forward<Args>(args)...);
    }

    template<typename T>
    static void destroy(allocator_type& a, T* ptr)
    {
        _Destroy1(a, ptr, _Uses_default_destroy_t<_Unwrap_alloc_t<allocator_type>, T*>());
    }

private:
    struct max_size_helper
    {
        template<typename T>
        static auto Fn(int, const T& a) noexcept
                -> decltype(a.max_size())
        {
            return a.max_size();
        }

        template<typename T>
        static auto Fn(detail::_Wrap_int, const T&) noexcept
                -> size_type
        {
            return numeric_limits<size_type>::max();
        }
    };
    struct alloc_select
    {
        template<typename T>
        static auto Fn(int, const T& a)
                -> decltype((T)a.select_on_container_copy_construction())
        {
            return a.select_on_container_copy_construction();
        }

        template<typename T>
        static auto Fn(detail::_Wrap_int, const T& a)
                -> T
        {
            return a;
        }
    };
public:
    /**
     *  @brief The maximum supported allocation size
     *  @return a.max_size() or @c numeric_limits<size_type>::max()
     *
     *  Returns a.max_size() if such an expression is well formed,
     *  otherwise, it returns @c numeric_limits<size_type>::max()
     */
    static size_type max_size(const allocator_type& a) noexcept
    {
        return max_size_helper::Fn(0, a);
    }

    static allocator_type select_on_container_copy_construction(const allocator_type& a)
    {
        return alloc_select::Fn(0, a);
    }
};

// todo: allocator_arg_t, allocator_arg, uses_allocator

MYSTD_NS_END
