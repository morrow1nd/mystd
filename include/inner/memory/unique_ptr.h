#pragma once

#include "../mystd.h"
#include "../type_traits.h"


MYSTD_NS_BEGIN

template<typename T>
struct default_delete
{
    constexpr default_delete() noexcept = default;
    // construct from another default_delete
    template <typename T2,
        typename = enable_if_t<is_convertible<T2*, T*>::value, void>>
    default_delete(const default_delete<T2>&) noexcept {}

    void operator()(T* p) const noexcept
    {
        static_assert(!is_void<T>::value, "can't delete pointer to incomplete type");
        static_assert(sizeof(T) > 0, "can't delete pointer to incomplete type");
        delete p;
    }
};

template<typename T>
struct default_delete<T[]>
{
    constexpr default_delete() noexcept = default;
    // construct from another default_delete
    template<typename U,
        typename = enable_if_t<is_convertible<U(*)[], T(*)[]>::value, void>>
    default_delete(const default_delete<U[]>&) noexcept {}

    template<typename U,
        typename = enable_if_t<is_convertible<U(*)[], T(*)[]>::value, void>>
    void operator()(U* p) const noexcept
    {
        static_assert(sizeof(T) > 0, "can't delete pointer to incomplete type");
        delete [] p;
    }
};


template<typename T, typename Deleter = default_delete<T>>
class unique_ptr {
    // use SFINAE to determine whether _Up::pointer exists
    // copied from gcc 5.3.0 unique_ptr.h
    class _Pointer
    {
        template<typename _Up>
        static typename _Up::pointer __test(typename _Up::pointer*);    // If _Up::pointer exists, using it, 

        template<typename _Up>
        static T* __test(...);                                          // otherwise, return T*.
    public:
        typedef decltype(__test<remove_reference_t<Deleter>>(0)) type;
    };

    typename _Pointer::type     p_;
    Deleter                     d_;
public:
    typedef typename _Pointer::type     pointer; // std::remove_reference<Deleter>::type::pointer if that type exists, otherwise T*. Must satisfy NullablePointer
    typedef T                           element_type;
    typedef Deleter                     deleter_type;

    // Default constructor, creates a unique_ptr that owns nothing.
    constexpr unique_ptr() noexcept 
        : p_(pointer()), d_(deleter_type()) 
    { static_assert(!is_pointer_v<deleter_type>, "constructed with null function pointer deleter"); }

    explicit unique_ptr(pointer p) noexcept
        : p_(p), d_(deleter_type())
    { static_assert(!is_pointer_v<deleter_type>, "constructed with null function pointer deleter"); }

    unique_ptr(pointer p,
        conditional_t<is_reference_v<deleter_type>,
            deleter_type, const deleter_type&> d) noexcept
        : p_(p), d_(d) {}
    
    unique_ptr(pointer p,
        add_rvalue_reference_t<deleter_type> d) noexcept
        : p_(std::move(p)), d_(std::move(d)) // todo
    { static_assert(!is_reference_v<deleter_type>, "rvalue deleter bound to reference"); }

    constexpr unique_ptr(nullptr_t) noexcept : unique_ptr() {}

    // Move constructors

    unique_ptr(unique_ptr&& u) noexcept
        : p_(u.release()), d_(std::forward<deleter_type>(u.get_deleter())) {}

    // Converting constructor form another type
    template<typename T2, typename D2,
        typename = enable_if_t<!is_array_v<T2>
            && is_convertible_v<typename unique_ptr<T2, D2>::pointer, pointer>
            && (is_reference_v<Deleter> ? is_same_v<D2, Deleter> : is_convertible_v<D2, Deleter>),
            void>>
    unique_ptr(unique_ptr<T2, D2>&& u) noexcept
    : p_(u.release()), d_(std::forward<D2>(u.get_deleter())) {}

    // Destructor
    ~unique_ptr() noexcept
    {
        if(p_ != pointer())
            get_deleter()(p_);
        p_ = pointer(); // clean it
    }

    // Assignment

    unique_ptr& operator=(unique_ptr&& u) noexcept
    {
        reset(u.release());
        get_deleter() = std::forward<deleter_type>(u.get_deleter());
        return *this;
    }

    /**
     * Assign by move rhs
     * Only if T2 is not an array type and unique_ptr<T2,D2>::pointer is implicitly convertible to
     * pointer and std::is_assignable<Deleter&, D2&&>::value is true. (Copied from cppreference.com)
    */
    template<typename T2, typename D2>
    enable_if_t<!is_array_v<T2>
        && is_assignable_v<Deleter, D2&&>
        && is_convertible_v<typename unique_ptr<T2, D2>::pointer, pointer>,
        unique_ptr&>
    operator=(unique_ptr<T2, D2>&& rhs) noexcept
    {
        reset(rhs.release());
        d_ = std::forward<D2>(rhs.get_deleter());
        return *this;
    }

    unique_ptr& operator=(nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    add_lvalue_reference<element_type> operator*() const
    {
        return *get();
    }

    pointer operator->() const noexcept
    {
        return get();
    }

    pointer get() const noexcept { return p_; }

    deleter_type& get_deleter() noexcept { return d_; }

    const deleter_type& get_deleter() const noexcept { return d_; }

    explicit operator bool() const noexcept
    {
        return get() == pointer() ? false : true;
    }

    pointer release() noexcept
    {
        pointer ret = p_;
        p_ = pointer(); // clean it
        return ret;
    }

    void reset(pointer p = pointer()) noexcept
    {
        pointer old = p_;
        p_ = p;
        if(old != pointer())
            get_deleter()(old);
    }

    void swap(unique_ptr& u) noexcept
    {
        std::swap(u.p_, p_);
        std::swap(u.d_, d_);
    }

    // Disable copy from lvalue
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
};


template<typename T, typename Deleter>
class unique_ptr<T[], Deleter>
{
    // unique_ptr for array objects is not achieved!
    explicit unique_ptr() = delete; // todo: show a error info while compiling.
};


MYSTD_NS_END
