#ifndef _RZ_TYPE_TRAITS_H_
#define _RZ_TYPE_TRAITS_H_

namespace Razix {

    // decltype: https://en.cppreference.com/w/cpp/language/decltype.html

    // [source]: https://en.cppreference.com/w/cpp/types/remove_reference.html
    // this remote_refence is to convert a Type value from either value/ref/&& to a &&, used for custom move semantics
    // It removes & or && from a type so you can get the underlying base type
    template<typename T>
    struct rz_remove_reference
    {
        using type = T;
    };
    template<typename T>
    struct rz_remove_reference<T&>
    {
        using type = T;
    };
    template<typename T>
    struct rz_remove_reference<T&&>
    {
        using type = T;
    };

    template<class T>
    using rz_remove_reference_t = typename rz_remove_reference<T>::type;

    // [source]: https://en.cppreference.com/w/cpp/types/remove_cv.html
    template<class T>
    struct rz_remove_cv
    {
        typedef T type;
    };
    template<class T>
    struct rz_remove_cv<const T>
    {
        typedef T type;
    };
    template<class T>
    struct rz_remove_cv<volatile T>
    {
        typedef T type;
    };
    template<class T>
    struct rz_remove_cv<const volatile T>
    {
        typedef T type;
    };

    template<class T>
    struct rz_remove_const
    {
        typedef T type;
    };
    template<class T>
    struct rz_remove_const<const T>
    {
        typedef T type;
    };

    template<class T>
    struct rz_remove_volatile
    {
        typedef T type;
    };
    template<class T>
    struct rz_remove_volatile<volatile T>
    {
        typedef T type;
    };

    template<class T>
    using rz_remove_cv_t = typename rz_remove_cv<T>::type;

    template<class T>
    using rz_remove_const_t = typename rz_remove_const<T>::type;

    template<class T>
    using rz_remove_volatile_t = typename rz_remove_volatile<T>::type;

    // [source]: https://en.cppreference.com/w/cpp/types/remove_cvref.html
    template<class T>
    struct rz_remove_cvref
    {
        using type = rz_remove_cv_t<rz_remove_reference_t<T>>;
    };

    template<class T>
    using rz_remove_cvref_t = typename rz_remove_cvref<T>::type;

    // [source]: https://en.cppreference.com/w/cpp/types/add_cv.html
    template<class T>
    struct rz_add_cv
    {
        typedef const volatile T type;
    };
    template<class T>
    struct rz_add_const
    {
        typedef const T type;
    };
    template<class T>
    struct rz_add_volatile
    {
        typedef volatile T type;
    };

    template<class T>
    using rz_add_cv_t = typename rz_add_cv<T>::type;
    template<class T>
    using rz_add_const_t = typename rz_add_const<T>::type;
    template<class T>
    using rz_add_volatile_t = typename rz_add_volatile<T>::type;

    // [source]: https://en.cppreference.com/w/cpp/utility/as_const.html
    template<class T>
    inline constexpr rz_add_const_t<T>& rz_as_const(T& t) noexcept
    {
        return t;
    }

    template<class T>
    void rz_as_const(const T&&) = delete;

    //----------------------------------------------------------------------------
    //INTEGRAL CONSTANT & TRUE/FALSE type traits
    //----------------------------------------------------------------------------

    // [source]: https://en.cppreference.com/w/cpp/types/integral_constant.html
    template<typename T, T v>
    struct rz_integral_constant
    {
        static constexpr T value = v;
        using value_type         = T;
        using type               = rz_integral_constant;
        constexpr            operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };

    using rz_true_type  = rz_integral_constant<bool, true>;
    using rz_false_type = rz_integral_constant<bool, false>;

    // [source]: https://en.cppreference.com/w/cpp/types/is_same.html
    template<class T, class U>
    struct rz_is_same : rz_false_type
    {};

    template<class T>
    struct rz_is_same<T, T> : rz_true_type
    {};

    template<class T, class U>
    inline constexpr bool rz_is_same_v = rz_is_same<T, U>::value;

    //----------------------------------------------------------------------------
    //POINTER DETECTION
    //----------------------------------------------------------------------------

    template<typename T>
    struct rz_is_pointer_impl : rz_false_type
    {};

    template<typename T>
    struct rz_is_pointer_impl<T*> : rz_true_type
    {};

    template<typename T>
    struct rz_is_pointer : rz_is_pointer_impl<rz_remove_cv_t<T>>
    {};

    template<typename T>
    inline constexpr bool rz_is_pointer_v = rz_is_pointer<T>::value;

    //----------------------------------------------------------------------------
    //INTEGRAL TYPE DETECTION
    //----------------------------------------------------------------------------

    // Helper implementation - works with raw types (no cv-qualifiers)
    template<typename T>
    struct rz_is_integral_impl : rz_false_type
    {};

    // Specializations for all integral types
    template<>
    struct rz_is_integral_impl<bool> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<char> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<signed char> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<unsigned char> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<wchar_t> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<char16_t> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<char32_t> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<short> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<unsigned short> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<int> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<unsigned int> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<long> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<unsigned long> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<long long> : rz_true_type
    {};
    template<>
    struct rz_is_integral_impl<unsigned long long> : rz_true_type
    {};

    template<typename T>
    struct rz_is_integral : rz_is_integral_impl<rz_remove_cv_t<T>>
    {};

    template<typename T>
    inline constexpr bool rz_is_integral_v = rz_is_integral<T>::value;

    //----------------------------------------------------------------------------
    //FLOATING POINT DETECTION
    //----------------------------------------------------------------------------

    template<typename T>
    struct rz_is_floating_point_impl : rz_false_type
    {};

    template<>
    struct rz_is_floating_point_impl<float> : rz_true_type
    {};

    template<>
    struct rz_is_floating_point_impl<double> : rz_true_type
    {};

    template<>
    struct rz_is_floating_point_impl<long double> : rz_true_type
    {};

    template<typename T>
    struct rz_is_floating_point : rz_is_floating_point_impl<rz_remove_cv_t<T>>
    {};

    template<typename T>
    inline constexpr bool rz_is_floating_point_v = rz_is_floating_point<T>::value;

    //----------------------------------------------------------------------------
    //SIGNED/UNSIGNED DETECTION
    //----------------------------------------------------------------------------

    template<typename T>
    struct rz_is_arithmetic_impl : rz_integral_constant<bool,
                                       rz_is_integral_v<T> || rz_is_floating_point_v<T>>
    {};

    template<typename T>
    struct rz_is_arithmetic : rz_is_arithmetic_impl<rz_remove_cv_t<T>>
    {};

    template<typename T>
    inline constexpr bool rz_is_arithmetic_v = rz_is_arithmetic<T>::value;

    template<typename T, bool = rz_is_arithmetic_v<T>>
    struct rz_is_signed_impl : rz_false_type
    {};

    template<typename T>
    struct rz_is_signed_impl<T, true> : rz_integral_constant<bool, T(-1) < T(0)>
    {};

    template<typename T>
    struct rz_is_signed : rz_is_signed_impl<rz_remove_cv_t<T>>
    {};

    template<typename T>
    inline constexpr bool rz_is_signed_v = rz_is_signed<T>::value;

    template<typename T, bool = rz_is_arithmetic_v<T>>
    struct rz_is_unsigned_impl : rz_false_type
    {};

    template<typename T>
    struct rz_is_unsigned_impl<T, true> : rz_integral_constant<bool, T(0) < T(-1)>
    {};

    template<typename T>
    struct rz_is_unsigned : rz_is_unsigned_impl<rz_remove_cv_t<T>>
    {};

    template<typename T>
    inline constexpr bool rz_is_unsigned_v = rz_is_unsigned<T>::value;

    // [source]: https://en.cppreference.com/w/cpp/types/is_const.html
    template<class T>
    struct rz_is_const : rz_false_type
    {};
    template<class T>
    struct rz_is_const<const T> : rz_true_type
    {};

    template<class T>
    inline constexpr bool rz_is_const_v = rz_is_const<T>::value;

    template<class T>
    struct rz_is_volatile : rz_false_type
    {};
    template<class T>
    struct rz_is_volatile<volatile T> : rz_true_type
    {};

    template<class T>
    inline constexpr bool rz_is_volatile_v = rz_is_volatile<T>::value;

}    // namespace Razix

#endif    // _RZ_TYPE_TRAITS_H_
