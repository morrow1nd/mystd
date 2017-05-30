#pragma once

#include "mystd.h"
#include <cstddef> // need nullptr_t, size_t
#include <type_traits>


MYSTD_NS_BEGIN

using std::nullptr_t;
using std::size_t;

//
// Forward declaration
//

template<typename T, typename U> struct is_same;
template<typename T, typename U> constexpr bool is_same_v = is_same<T, U>::value;
template<typename T> struct remove_const;
template<typename T> struct remove_volatile;
template<typename T> struct remove_cv;
template<typename T> using remove_const_t = typename remove_const<T>::type;
template<typename T> using remove_volatile_t = typename remove_volatile<T>::type;
template<typename T> using remove_cv_t = typename remove_cv<T>::type;
template<typename T> struct add_cv;
template<typename T> struct add_const;
template<typename T> struct add_volatile;
template<typename T> using add_cv_t = typename add_cv<T>::type;
template<typename T> using add_const_t = typename add_const<T>::type;
template<typename T> using add_volatile_t = typename add_volatile<T>::type;
template<typename T> struct is_member_pointer;
template<typename T> constexpr bool is_member_pointer_v = is_member_pointer<T>::value;
template<typename T> struct is_member_function_pointer;
template<typename T> constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;
template<typename T> struct is_arithmetic;
template<typename T> constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
template<typename T> struct is_trivially_copyable;
template<typename T> constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;
template<typename T> struct is_default_constructible;
template<typename T> struct is_trivially_default_constructible;
template<typename T> struct is_nothrow_default_constructible;
template<typename T> constexpr bool is_default_constructible_v = is_default_constructible<T>::value;
template<typename T> constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<T>::value;
template<typename T> constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<T>::value;
template<bool B, typename T, typename F> struct conditional;
template<bool B, typename T, typename F> using conditional_t = typename conditional<B, T, F>::type;
template<typename T> struct remove_extent;
template<typename T> struct remove_all_extents;
template<typename T> using remove_extent_t = typename remove_extent<T>::type;
template<typename T> using remove_all_extents_t = typename remove_all_extents<T>::type;
template<typename T> struct remove_reference;
template<typename T> struct add_lvalue_reference;
template<typename T> struct add_rvalue_reference;
template<typename T> using remove_reference_t = typename remove_reference<T>::type;
template<typename T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
template<typename T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;


//
// helper class:
//

template <typename T, T v>
struct integral_constant
{
    static constexpr T value = v;
    typedef T value_type;
    typedef integral_constant<T, v> type;

    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};

template <typename T, T v>
constexpr T integral_constant<T, v>::value;

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

//
// primary type categories:
//

template<typename T> struct is_void : false_type {};
template<> struct is_void<void> : true_type {};
template<> struct is_void<const void> : true_type {};
template<> struct is_void<volatile void> : true_type {};
template<> struct is_void<const volatile void> : true_type {};
/*
** Or:
** template< class T >
** struct is_void : std::is_same<void, typename std::remove_cv<T>::type> {};
*/

template<typename T> constexpr bool is_void_v = is_void<T>::value;


template<typename T> struct is_null_pointer : is_same<nullptr_t, remove_cv_t<T>> {};
template<typename T> constexpr bool is_null_pointer_v = is_null_pointer<T>::value;


template<typename T> struct is_integral_helper : false_type {};
template<> struct is_integral_helper<bool>                  : true_type {};
template<> struct is_integral_helper<char>                  : true_type {};
template<> struct is_integral_helper<unsigned char>         : true_type {};
template<> struct is_integral_helper<signed char>           : true_type {};
template<> struct is_integral_helper<char16_t>              : true_type {};
template<> struct is_integral_helper<char32_t>              : true_type {};
template<> struct is_integral_helper<wchar_t>               : true_type {};
template<> struct is_integral_helper<short>                 : true_type {};
template<> struct is_integral_helper<unsigned short>        : true_type {};
template<> struct is_integral_helper<int>                   : true_type {};
template<> struct is_integral_helper<unsigned int>          : true_type {};
template<> struct is_integral_helper<long>                  : true_type {};
template<> struct is_integral_helper<unsigned long>         : true_type {};
template<> struct is_integral_helper<long long>             : true_type {};
template<> struct is_integral_helper<unsigned long long>    : true_type {};
template<typename T> struct is_integral : is_integral_helper<remove_cv_t<T>> {};
template<typename T> constexpr bool is_integral_v = is_integral<T>::value;

template<typename T>
struct is_floating_point : integral_constant<bool,
        is_same_v<float, remove_cv_t<T>> ||
        is_same_v<double, remove_cv_t<T>> ||
        is_same_v<long double, remove_cv_t<T>>> {};
template<typename T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;

template<typename T> struct is_array : false_type {};
template<typename T> struct is_array<T[]> : true_type {};
template<typename T, size_t N> struct is_array<T[N]> : true_type {};
template<typename T> constexpr bool is_array_v = is_array<T>::value;

template<typename T> struct is_enum : integral_constant<bool, __is_enum(T)> {}; // gcc and VC both uses __is_enum
template<typename T> constexpr bool is_enum_v = is_enum<T>::value;

template<typename T> struct is_union : integral_constant<bool, __is_union(T)> {};
template<typename T> constexpr bool is_union_v = is_union<T>::value;

template<typename T> struct is_class : integral_constant<bool, __is_class(T)> {};
template<typename T> constexpr bool is_class_v = is_class<T>::value;


#ifdef _MSC_VER
# define _MYSTD__TEMPLATE_VARS ,  /* VS uses Args..., ... */
#else
# define _MYSTD__TEMPLATE_VARS
#endif
// copied from cppreference:http://en.cppreference.com/w/cpp/types/is_function
template<typename>
struct is_function : false_type { };
// specialization for regular functions
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)> : true_type {};
// specialization for variadic functions such as std::printf
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...)> : true_type {};
// specialization for function types that have cv-qualifiers
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)volatile> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const volatile> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...)const> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...)volatile> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...)const volatile> : true_type {};
// specialization for function types that have ref-qualifiers
template<typename Ret, typename... Args>
struct is_function<Ret(Args...) &> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const &> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)volatile &> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const volatile &> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...) &> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...)const &> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...)volatile &> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...)const volatile &> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...) &&> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const &&> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)volatile &&> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args...)const volatile &&> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...) &&> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...)const &&> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...)volatile &&> : true_type {};
template<typename Ret, typename... Args>
struct is_function<Ret(Args... _MYSTD__TEMPLATE_VARS ...)const volatile &&> : true_type {};

template<typename T> constexpr bool is_function_v = is_function<T>::value;


template<typename T> struct is_pointer_helper     : false_type {};
template<typename T> struct is_pointer_helper<T*> : true_type {};
template<typename T> struct is_pointer : is_pointer_helper<remove_cv_t<T>> {};
template<typename T> constexpr bool is_pointer_v = is_pointer<T>::value;

template<typename T> struct is_lvalue_reference : false_type {};
template<typename T> struct is_lvalue_reference<T&> : true_type {};
template<typename T> constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;


template<typename T> struct is_rvalue_reference : false_type {};
template<typename T> struct is_rvalue_reference<T&&> : true_type {};
template<typename T> constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template<typename T> struct is_member_object_pointer : integral_constant<bool,
        is_member_pointer_v<T> &&
        !is_member_function_pointer_v<T>> {};
template<typename T> constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;

template<typename T> struct is_member_function_pointer_helper : false_type {};
template<typename T, typename U> struct is_member_function_pointer_helper<T U::*> : is_function<T> {};
template<typename T> struct is_member_function_pointer : is_member_function_pointer_helper<remove_cv_t<T>> {};


//
// composite type categories:
//

template<typename T> struct is_fundamental : integral_constant<bool,
        is_arithmetic_v<T> ||
        is_void_v<T> ||
        is_same_v<nullptr_t, remove_cv_t<T>>> {};
template<typename T> constexpr bool is_fundamental_v = is_fundamental<T>::value;

template<typename T> struct is_arithmetic : integral_constant<bool,
        is_integral_v<T> ||
        is_floating_point_v<T>> {};

template<typename T> struct is_scalar : integral_constant<bool,
        is_arithmetic_v<T> ||
        is_enum_v<T> ||
        is_pointer_v<T> ||
        is_member_pointer_v<T> ||
        is_null_pointer_v<T>> {};
template<typename T> constexpr bool is_scalar_v = is_scalar<T>::value;

template<typename T> struct is_object : integral_constant<bool,
        is_scalar_v<T> ||
        is_array_v<T> ||
        is_union_v<T> ||
        is_class_v<T>> {};
template<typename T> constexpr bool is_object_v = is_object<T>::value;

template<typename T> struct is_compound : integral_constant<bool, !is_fundamental_v<T>> {};
template<typename T> constexpr bool is_compound_v = is_compound<T>::value;

template<typename T> struct is_reference : integral_constant<bool,
        is_lvalue_reference_v<T> ||
        is_rvalue_reference_v<T>> {};
template<typename T> constexpr bool is_reference_v = is_reference<T>::value;

template<typename T> struct is_member_pointer_helper : false_type {};
template<typename T, typename U> struct is_member_pointer_helper<T U::*> : true_type {};
template<typename T> struct is_member_pointer : is_member_pointer_helper<remove_cv_t<T>> {};


//
// type properties:
//

template<typename T> struct is_const : false_type {};
template<typename T> struct is_const<const T> : true_type {};
template<typename T> constexpr bool is_const_v = is_const<T>::value;

template<typename T> struct is_volatile : false_type {};
template<typename T> struct is_volatile<volatile T> : true_type {};
template<typename T> constexpr bool is_volatile_v = is_volatile<T>::value;

template<typename T> struct is_trivial : integral_constant<bool,
        is_trivially_copyable_v<T> &&
        is_trivially_default_constructible_v<T>> {};
template<typename T> constexpr bool is_trivial_v = is_trivial<T>::value;

template<typename T> struct is_trivially_copyable : integral_constant<bool, __is_trivially_copyable(T)> {};

template<typename T> struct is_standard_layout : integral_constant<bool, __is_standard_layout(T)> {};
template<typename T> constexpr bool is_standard_layout_v = is_standard_layout<T>::value;

template<typename T> struct is_pod : integral_constant<bool, __is_pod(T)> {};
template<typename T> constexpr bool is_pod_v = is_pod<T>::value;

template<typename T> struct is_literal_type : integral_constant<bool, __is_literal_type(T)> {};
template<typename T> constexpr bool is_literal_type_v = is_literal_type<T>::value;

template<typename T> struct is_empty : integral_constant<bool, __is_empty(T)> {};
template<typename T> constexpr bool is_empty_v = is_empty<T>::value;

template<typename T> struct is_polymorphic : integral_constant<bool, __is_polymorphic(T)> {};
template<typename T> constexpr bool is_polymorphic_v = is_polymorphic<T>::value;

template<typename T> struct is_abstract : integral_constant<bool, __is_abstract(T)> {};
template<typename T> constexpr bool is_abstract_v = is_abstract<T>::value;

template<typename T> struct is_final : integral_constant<bool, __is_final(T)> {};
template<typename T> constexpr bool is_final_v = is_final<T>::value;

template<typename T, bool = is_arithmetic_v<T>> struct is_signed_helper : false_type {};
template<typename T> struct is_signed_helper<T, true> : integral_constant<bool, T(-1) < T(0)> {};
template<typename T> struct is_signed : is_signed_helper<T>::type {};
template<typename T> constexpr bool is_signed_v = is_signed<T>::value;

template<typename T> struct is_unsigned : integral_constant<bool,
        is_arithmetic_v<T> &&
        ! is_signed_v<T>> {};
template<typename T> constexpr bool is_unsigned_v = is_unsigned<T>::value;



//
// supported operations:
//

using std::is_constructible;
using std::is_trivially_constructible;
using std::is_nothrow_constructible;

template<typename T, typename... Args>
constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
template<typename T, typename... Args>
constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;
template<typename T, typename... Args>
constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;

template<typename T> struct is_default_constructible : is_constructible<T> {};
template<typename T> struct is_trivially_default_constructible : is_trivially_constructible<T> {};
template<typename T> struct is_nothrow_default_constructible : is_nothrow_constructible<T> {};

template<typename T> struct is_copy_constructible : is_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template<typename T> struct is_trivially_copy_constructible : is_trivially_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template<typename T> struct is_nothrow_copy_constructible : is_nothrow_constructible<T, add_lvalue_reference_t<add_const_t<T>>> {};
template<typename T> constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;
template<typename T> constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<T>::value;
template<typename T> constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<T>::value;

template<typename T> struct is_move_constructible : is_constructible<T, add_rvalue_reference_t<T>> {};
template<typename T> struct is_trivially_move_constructible : is_trivially_constructible<T, add_rvalue_reference_t<T>> {};
template<typename T> struct is_nothrow_move_constructible : is_nothrow_constructible<T, add_rvalue_reference_t<T>> {};
template<typename T> constexpr bool is_move_constructible_v = is_move_constructible<T>::value;
template<typename T> constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;
template<typename T> constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

using std::is_assignable;
using std::is_trivially_assignable;
using std::is_nothrow_assignable;
template<typename T, typename U> constexpr bool is_assignable_v = is_assignable<T, U>::value;
template<typename T, typename U> constexpr bool is_trivially_assignable_v = is_trivially_assignable<T, U>::value;
template<typename T, typename U> constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;

using std::is_copy_assignable;
using std::is_trivially_copy_assignable;
using std::is_nothrow_copy_assignable;

using std::is_move_assignable;
using std::is_trivially_move_assignable;
using std::is_nothrow_move_assignable;

using std::is_destructible;
using std::is_trivially_destructible;
using std::is_nothrow_destructible;

using std::has_virtual_destructor;


// property queries

template<typename T> struct alignment_of : integral_constant<size_t, alignof(T)> {};
template<typename T> constexpr size_t alignment_of_v = alignment_of<T>::value;

template<typename T> struct rank : integral_constant<size_t, 0> {};
template<typename T> struct rank<T[]> : integral_constant<size_t, rank<T>::value + 1> {};
template<typename T, size_t N> struct rank<T[N]> : integral_constant<size_t, rank<T>::value + 1> {};
template<typename T> constexpr size_t rank_v = rank<T>::value;

template<typename T, unsigned N = 0>
struct extent : integral_constant<size_t, 0> {};
template<typename T>
struct extent<T[], 0> : integral_constant<size_t, 0> {};
template<typename T, unsigned N>
struct extent<T[], N> : extent<T, N - 1> {};
template<typename T, size_t I>
struct extent<T[I], 0> : integral_constant<size_t, I> {};
template<typename T, size_t I, unsigned N>
struct extent<T[I], N> : extent<T, N - 1> {};
template<typename T, unsigned N = 0> constexpr size_t extent_v = extent<T, N>::value;

//
// type relations:
//

template<typename T, typename U> struct is_same : false_type {};
template<typename T> struct is_same<T, T> : true_type {};

using std::is_base_of;
template<typename B, typename D> constexpr bool is_base_of_v = is_base_of<B, D>::value;

using std::is_convertible;
template<typename From, typename To> constexpr bool is_convertible_v = is_convertible<From, To>::value;


// const-volatile modifications:

template<typename T> struct remove_const            { typedef T type; };
template<typename T> struct remove_const<const T>   { typedef T type; };
template<typename T> struct remove_volatile             { typedef T type; };
template<typename T> struct remove_volatile<volatile T> { typedef T type; };
template<typename T> struct remove_cv { typedef typename remove_volatile<typename remove_const<T>::type>::type type; };

template<typename T> struct add_cv { typedef const volatile T type; };
template<typename T> struct add_const { typedef const T type; };
template<typename T> struct add_volatile { typedef volatile T type; };

//
// reference:
//

template<typename T> struct remove_reference      { typedef T type; };
template<typename T> struct remove_reference<T&>  { typedef T type; };
template<typename T> struct remove_reference<T&&> { typedef T type; };

template<typename T> struct add_lvalue_reference      { typedef T& type; };
template<typename T> struct add_lvalue_reference<T&>  { typedef T& type; };
template<typename T> struct add_lvalue_reference<T&&> { typedef T& type; };

template<typename T> struct add_rvalue_reference      { typedef T&& type; };
template<typename T> struct add_rvalue_reference<T&>  { typedef T&& type; };
template<typename T> struct add_rvalue_reference<T&&> { typedef T&& type; };

// pointers

template<typename T> struct remove_pointer                      { typedef T type; };
template<typename T> struct remove_pointer<T*>                  { typedef T type; };
template<typename T> struct remove_pointer<T* const>            { typedef T type; };
template<typename T> struct remove_pointer<T* volatile>         { typedef T type; };
template<typename T> struct remove_pointer<T* const volatile>   { typedef T type; };
template<typename T> using remove_pointer_t = typename remove_pointer<T>::type;

template<typename T, bool = is_function_v<T>>
struct add_pointer_helper               { typedef T type; };
template<typename T>
struct add_pointer_helper<T, false>     { typedef remove_reference_t<T> * type;};
template<typename T> struct add_pointer { typedef typename add_pointer_helper<T>::type type; };
template<typename T> using add_pointer_t = typename add_pointer<T>::type;


// sign modifiers

using std::make_signed;
template<typename T> using make_signed_t = typename make_signed<T>::type;

using std::make_unsigned;
template<typename T> using make_unsigned_t = typename make_unsigned<T>::type;

// arrays

template<typename T> struct remove_extent { typedef T type; };
template<typename T> struct remove_extent<T[]> { typedef T type; };
template<typename T, size_t N> struct remove_extent<T[N]> { typedef T type; };

template<typename T> struct remove_all_extents { typedef T type; };
template<typename T> struct remove_all_extents<T[]> { typedef typename remove_all_extents<T>::type type; };
template<typename T, size_t N> struct remove_all_extents<T[N]> { typedef typename remove_all_extents<T>::type type; };


// miscellaneous transformations

using std::aligned_storage;

using std::aligned_union;

template<typename T>
struct decay
{
private:
    typedef remove_reference_t<T> noref;
public:
    typedef conditional_t<is_array_v<noref>,
        remove_extent_t<noref> *,
        conditional_t<is_function_v<noref>,
            add_pointer_t<noref>,
            remove_cv_t<noref>>>
    type;
};
template<typename T> using decay_t = typename decay<T>::type;

template<bool B, typename T = void> struct enable_if {};
template<typename T> struct enable_if<true, T> { typedef T type; };
template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

template<bool B, typename T, typename F> struct conditional { typedef T type; };
template<typename T, typename F> struct conditional<false, T, F> { typedef F type; };

using std::common_type;
template<typename... T> using common_type_t = typename common_type<T...>::type;

using std::underlying_type;
template<typename T> using underlying_type_t = typename underlying_type<T>::type;

/**
 *  Utility metafunction that maps a sequence of any types to the type void.
 *  This metafunction is used in template metaprogramming to detect ill-formed types in SFINAE context.
 *  It can also be used to detect validity of an expression.
 *  see more http://en.cppreference.com/w/cpp/types/void_t
*/
template<typename...>
using void_t = void;

// operations on traits



MYSTD_DETAIL_NS_BEGIN

// copied from MSVC
    // TEMPLATE CLASS conjunction
template<class...>
    struct _Conjunction;

template<bool,
    class _Lhs,
    class... _Traits>
    struct _Choose_conjunction
    {   // Select _Lhs if it is false
    typedef _Lhs type;
    };

template<class _Lhs,
    class... _Traits>
    struct _Choose_conjunction<true, _Lhs, _Traits...>
    {   // Select the conjunction of _Traits if _Lhs is true
    typedef typename _Conjunction<_Traits...>::type type;
    };

template<>
    struct _Conjunction<>
    {   // Implement conjunction for 0 arguments
    typedef true_type type;
    };

template<class _Trait>
    struct _Conjunction<_Trait>
    {   // Implement conjunction for 1 argument
    typedef _Trait type;
    };

template<class _Lhs,
    class... _Traits>
    struct _Conjunction<_Lhs, _Traits...>
    {   // Implement conjunction for N arguments
    typedef typename _Choose_conjunction<_Lhs::value, _Lhs, _Traits...>::type type;
    };

template<class... _Traits>
    struct conjunction
        : _Conjunction<_Traits...>::type
    {   // If _Traits is empty, true_type
        // Otherwise, if any of _Traits are false, the first false trait
        // Otherwise, the last trait in _Traits
    };

    // TEMPLATE CLASS disjunction
template<class...>
    struct _Disjunction;

template<bool,
    class _Lhs,
    class... _Traits>
    struct _Choose_disjunction
    {   // Select the disjunction of _Traits if _Lhs is true
    typedef _Lhs type;
    };

template<class _Lhs,
    class... _Traits>
    struct _Choose_disjunction<false, _Lhs, _Traits...>
    {   // Select _Lhs if it is false
    typedef typename _Disjunction<_Traits...>::type type;
    };

template<>
    struct _Disjunction<>
    {   // Implement disjunction for 0 arguments
    typedef false_type type;
    };

template<class _Trait>
    struct _Disjunction<_Trait>
    {   // Implement disjunction for 1 argument
    typedef _Trait type;
    };

template<class _Lhs,
    class... _Traits>
    struct _Disjunction<_Lhs, _Traits...>
    {   // Implement disjunction for N arguments
    typedef typename _Choose_disjunction<_Lhs::value, _Lhs, _Traits...>::type type;
    };

template<class... _Traits>
    struct disjunction
        : _Disjunction<_Traits...>::type
    {   // If _Traits is empty, false_type
        // Otherwise, if any of _Traits are true, the first true trait
        // Otherwise, the last trait in _Traits
    };

//     // TEMPLATE CLASS negation
// template<class _Trait>
//     struct negation
//         : bool_constant<!_Trait::value>
//     {   // The negated result of _Trait
//     };

MYSTD_DETAIL_NS_END

MYSTD_NS_END
