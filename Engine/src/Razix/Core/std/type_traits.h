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

    //----------------------------------------------------------------------------
    // VOID_T - Maps any types to void (SFINAE helper)
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/types/void_t.html

    template<typename...>
    using rz_void_t = void;

    //----------------------------------------------------------------------------
    // VOID DETECTION
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/types/is_void.html

    template<typename T>
    struct rz_is_void_impl : rz_false_type
    {};

    template<>
    struct rz_is_void_impl<void> : rz_true_type
    {};

    // Main trait - strips cv-qualifiers before checking
    template<typename T>
    struct rz_is_void : rz_is_void_impl<rz_remove_cv_t<T>>
    {};

    template<typename T>
    inline constexpr bool rz_is_void_v = rz_is_void<T>::value;

    //----------------------------------------------------------------------------
    // CLASS DETECTION
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/types/is_class.html

    namespace detail {
        // This overload is chosen if T is a class type (member pointer is valid)
        template<typename T>
        rz_true_type test_is_class(int T::*);

        // This overload is chosen for all other types (fallback)
        template<typename>
        rz_false_type test_is_class(...);
    }    // namespace detail

    template<typename T>
    struct rz_is_class : decltype(detail::test_is_class<T>(nullptr))
    {};

    template<typename T>
    inline constexpr bool rz_is_class_v = rz_is_class<T>::value;

    //----------------------------------------------------------------------------
    // UNION DETECTION
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/types/is_union.html

    template<typename T>
    struct rz_is_union : rz_integral_constant<bool, __is_union(T)>
    {};

    template<typename T>
    inline constexpr bool rz_is_union_v = rz_is_union<T>::value;

    //----------------------------------------------------------------------------
    // ENABLE_IF - SFINAE helper
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/types/enable_if.html

    // Primary template - condition is false, no 'type' member defined
    template<bool B, typename T = void>
    struct rz_enable_if
    {};

    // Partial specialization - condition is true, 'type' member is defined
    template<typename T>
    struct rz_enable_if<true, T>
    {
        typedef T type;
    };

    // Helper alias
    template<bool B, typename T = void>
    using rz_enable_if_t = typename rz_enable_if<B, T>::type;

    //----------------------------------------------------------------------------
    // UNDERLYING_TYPE - Get underlying type of enum
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/types/underlying_type.html

    template<typename T>
    struct rz_underlying_type
    {
        typedef __underlying_type(T) type;
    };

    template<typename T>
    using rz_underlying_type_t = typename rz_underlying_type<T>::type;

    //----------------------------------------------------------------------------
    // ENUM DETECTION
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/types/is_enum.html

    template<typename T>
    struct rz_is_enum : rz_integral_constant<bool, __is_enum(T)>
    {};

    template<typename T>
    inline constexpr bool rz_is_enum_v = rz_is_enum<T>::value;

    //----------------------------------------------------------------------------
    // DECLVAL - Create reference to type without construction
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/utility/declval.html

    // Reference collapsing rules:
    // T&  &   -> T&  (lvalue & lvalue = lvalue)
    // T&  &&  -> T&  (lvalue & rvalue = lvalue)
    // T&& &   -> T&  (rvalue & lvalue = lvalue)
    // T&& &&  -> T&& (rvalue & rvalue = rvalue)

    namespace detail {
        // Try to return T&& (rvalue reference)
        template<typename T, typename U = T&&>
        U rz_declval_impl(int);

        // Fallback: return T (for types that can't be references, like void)
        template<typename T>
        T rz_declval_impl(long);
    }    // namespace detail

    template<typename T>
    auto rz_declval() noexcept -> decltype(detail::rz_declval_impl<T>(0));

    // Note: declval is intentionally not defined (only declared)
    // It's only meant to be used in unevaluated contexts (decltype, sizeof, etc.)
    // Returns the type, but NEVER actually calls the function!
    // (0) is int by default if it can form the overload T&& if not it falls back to long and takes the other SFINAE path

    //----------------------------------------------------------------------------
    // DEFAULT CONSTRUCTIBLE DETECTION
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/types/is_default_constructible.html

    namespace detail {
        // Helper to test if T() is valid
        template<typename T>
        auto test_default_constructible(int)
            -> decltype(T(), rz_true_type{});

        // Fallback when T() is invalid
        template<typename>
        auto test_default_constructible(...)
            -> rz_false_type;
    }    // namespace detail

    template<typename T>
    struct rz_is_default_constructible
        : decltype(detail::test_default_constructible<T>(0))
    {};

    template<typename T>
    inline constexpr bool rz_is_default_constructible_v = rz_is_default_constructible<T>::value;

    //----------------------------------------------------------------------------
    // MOVE CONSTRUCTIBLE DETECTION
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/types/is_move_constructible.html

    namespace detail {
        // Helper to test if T(T&&) is valid
        template<typename T>
        auto test_move_constructible(int)
            -> decltype(T(rz_declval<T>()), rz_true_type{});

        // Fallback when T(T&&) is invalid
        template<typename>
        auto test_move_constructible(...)
            -> rz_false_type;
    }    // namespace detail

    template<typename T>
    struct rz_is_move_constructible
        : decltype(detail::test_move_constructible<T>(0))
    {};

    template<typename T>
    inline constexpr bool rz_is_move_constructible_v = rz_is_move_constructible<T>::value;

    //----------------------------------------------------------------------------
    // COPY CONSTRUCTIBLE DETECTION
    //----------------------------------------------------------------------------
    // [source]: https://en.cppreference.com/w/cpp/types/is_copy_constructible.html

    namespace detail {
        // Helper to test if T(const T&) is valid
        // Reference collapsing rule: lvalue_ref && --> lvalue_ref
        template<typename T>
        auto test_copy_constructible(int)
            -> decltype(T(rz_declval<const T&>()), rz_true_type{});

        // Fallback when T(const T&) is invalid
        template<typename>
        auto test_copy_constructible(...)
            -> rz_false_type;
    }    // namespace detail

    template<typename T>
    struct rz_is_copy_constructible
        : decltype(detail::test_copy_constructible<T>(0))
    {};

    template<typename T>
    inline constexpr bool rz_is_copy_constructible_v = rz_is_copy_constructible<T>::value;

    /**
     * We need ways to emulate pure virtual function verification
     * We use SFINAE idiom and type traits as the base concept to do this 
     * 
     * Core Concept : SFINAE failure trigger redirection
     * 
     * SFINAE also does Struct/Type verification in addition to functions exist checks
     * 
     * SFINAE using safe substitution failure we manually trigger based on condition to choose the test we want
     * 
     * Note: These check don't consider the function signature
     * 
     * Note: We can do return type checks specialization inside the functions instead 
     * of just returning{}
     */

    // TODO: Add signature check when doing CHECK_TYPE_HAS_FUNCTION

    /**
     * RAZIX_CHECK_TYPE_HAS_FUNCTION
     * 
     * Working: Given a function name, it will check whether it has that function in it or not
     * If it's successful it will select the first specialization and return a true_type 
     * if not the first one will fail due to SFINAE and select the second type and return false_type
     */

#define RAZIX_CHECK_TYPE_HAS_FUNCTION(T, funcName)                 \
    template<typename T>                                           \
    class has_##funcName                                           \
    {                                                              \
    private:                                                       \
        template<typename C>                                       \
        static constexpr rz_true_type test(decltype(&C::funcName)) \
        {                                                          \
            return {};                                             \
        }                                                          \
                                                                   \
        template<typename C>                                       \
        static constexpr rz_false_type test(...)                   \
        {                                                          \
            return {};                                             \
        }                                                          \
                                                                   \
    public:                                                        \
        static constexpr bool value = test<T>(0);                  \
    };                                                             \
    template<typename T>                                           \
    inline constexpr bool has_##funcName##_v = has_##funcName<T>::value;

#define RAZIX_TYPE_HAS_FUNCTION_V(T, funcName) \
    has_##funcName##_v<T>

    /**
     * RAZIX_CHECK_TYPE_HAS_SUBTYPE
     * 
     * Working: Given a type and subtype, if the type has the subtype it will choose the second 
     * specialization and return true_type if not it will choose the first one as default and 
     * return false_type
     * 
     */

#define RAZIX_CHECK_TYPE_HAS_SUBTYPE(T, U)                             \
    template<typename Type_, typename = void>                          \
    struct has_##U : rz_false_type                                     \
    {                                                                  \
    };                                                                 \
    template<typename Type_>                                           \
    struct has_##U<Type_, rz_void_t<typename Type_::U>> : rz_true_type \
    {                                                                  \
    };                                                                 \
    template<typename Type_>                                           \
    inline constexpr bool has_##U##_v = has_##U<Type_>::value

#define RAZIX_TYPE_HAS_SUB_TYPE_V(T, U) \
    has_##U##_v<T>

#define RAZIX_CHECK_IF_TYPE_IS_DEFINED(T, msg) static_assert(rz_is_class_v<T>(), msg)

    /**
     * SFINAE_TYPE_ERASURE_CONCEPT_CHECK
     */

    /**
     * SFINAE ENUM CLASS |/& OPERATOR CHECK
     */
#define RAZIX_ENUM_CHECK_FOR_BITWISE_OPS(E)                                   \
    template<typename T, bool = rz_is_enum<T>::value>                         \
    struct E;                                                                 \
                                                                              \
    template<typename T>                                                      \
    struct E<T, true> : rz_false_type                                         \
    {};                                                                       \
                                                                              \
    template<typename T, typename rz_enable_if<E<T>::value>::type* = nullptr> \
    T operator|(T lhs, T rhs)                                                 \
    {                                                                         \
        using u_t = typename rz_underlying_type<T>::type;                     \
        return static_cast<T>(static_cast<u_t>(lhs) | static_cast<u_t>(rhs)); \
    }

    /**
     * Bitwise OR and AND for enum class type
     */
#define RAZIX_ENUM_CLASS_BITWISE_COMPATIBLE(E)                                      \
    static E operator|(E a, E b)                                                    \
    {                                                                               \
        return static_cast<E>(static_cast<unsigned>(a) | static_cast<unsigned>(b)); \
    }                                                                               \
                                                                                    \
    static E operator&(E a, E b)                                                    \
    {                                                                               \
        return static_cast<E>(static_cast<unsigned>(a) & static_cast<unsigned>(b)); \
    }                                                                               \
                                                                                    \
    static bool operator!(E a)                                                      \
    {                                                                               \
        return static_cast<unsigned>(a) == 0;                                       \
    }                                                                               \
                                                                                    \
    static bool operator&&(E a, E b)                                                \
    {                                                                               \
        return static_cast<unsigned>(a) && static_cast<unsigned>(b);                \
    }                                                                               \
                                                                                    \
    static bool operator||(E a, E b)                                                \
    {                                                                               \
        return static_cast<unsigned>(a) || static_cast<unsigned>(b);                \
    }

    //bool operator|(E a, E b)                                                        \
    //{                                                                               \
    //    return static_cast<unsigned>(a) | static_cast<unsigned>(b);                 \
    //}                                                                               \
    //                                                                                \
    //bool operator&(E a, E b)                                                        \
    //{                                                                               \
    //    return static_cast<unsigned>(a) & static_cast<unsigned>(b);                 \
    //}

}    // namespace Razix

#endif    // _RZ_TYPE_TRAITS_H_
