#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/std/type_traits.h"

#include <gtest/gtest.h>

namespace Razix {
    TEST(TypeTraitsTest, RemoveReference_ValueType)
    {
        EXPECT_TRUE((rz_is_same_v<rz_remove_reference_t<int>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_reference_t<float>, float>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_reference_t<double>, double>) );
    }

    TEST(TypeTraitsTest, RemoveReference_LValueReference)
    {
        EXPECT_TRUE((rz_is_same_v<rz_remove_reference_t<int&>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_reference_t<float&>, float>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_reference_t<const int&>, const int>) );
    }

    TEST(TypeTraitsTest, RemoveReference_RValueReference)
    {
        EXPECT_TRUE((rz_is_same_v<rz_remove_reference_t<int&&>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_reference_t<float&&>, float>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_reference_t<const int&&>, const int>) );
    }

    //============================================================================
    // REMOVE CV TESTS
    //============================================================================

    TEST(TypeTraitsTest, RemoveConst_BasicTypes)
    {
        EXPECT_TRUE((rz_is_same_v<rz_remove_const_t<int>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_const_t<const int>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_const_t<volatile int>, volatile int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_const_t<const volatile int>, volatile int>) );
    }

    TEST(TypeTraitsTest, RemoveVolatile_BasicTypes)
    {
        EXPECT_TRUE((rz_is_same_v<rz_remove_volatile_t<int>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_volatile_t<volatile int>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_volatile_t<const int>, const int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_volatile_t<const volatile int>, const int>) );
    }

    TEST(TypeTraitsTest, RemoveCV_AllCombinations)
    {
        EXPECT_TRUE((rz_is_same_v<rz_remove_cv_t<int>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_cv_t<const int>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_cv_t<volatile int>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_cv_t<const volatile int>, int>) );
    }

    TEST(TypeTraitsTest, RemoveCVRef_Combined)
    {
        EXPECT_TRUE((rz_is_same_v<rz_remove_cvref_t<int>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_cvref_t<const int&>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_cvref_t<volatile int&&>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_remove_cvref_t<const volatile int&>, int>) );
    }

    //============================================================================
    // ADD CV TESTS
    //============================================================================

    TEST(TypeTraitsTest, AddConst_BasicTypes)
    {
        EXPECT_TRUE((rz_is_same_v<rz_add_const_t<int>, const int>) );
        EXPECT_TRUE((rz_is_same_v<rz_add_const_t<const int>, const int>) );
    }

    TEST(TypeTraitsTest, AddVolatile_BasicTypes)
    {
        EXPECT_TRUE((rz_is_same_v<rz_add_volatile_t<int>, volatile int>) );
        EXPECT_TRUE((rz_is_same_v<rz_add_volatile_t<volatile int>, volatile int>) );
    }

    TEST(TypeTraitsTest, AddCV_BasicTypes)
    {
        EXPECT_TRUE((rz_is_same_v<rz_add_cv_t<int>, const volatile int>) );
    }

    //============================================================================
    // INTEGRAL CONSTANT TESTS
    //============================================================================

    TEST(TypeTraitsTest, IntegralConstant_Value)
    {
        using five = rz_integral_constant<int, 5>;
        EXPECT_EQ(five::value, 5);
        EXPECT_TRUE((rz_is_same_v<five::value_type, int>) );
    }

    TEST(TypeTraitsTest, TrueType_FalseType)
    {
        EXPECT_TRUE(rz_true_type::value);
        EXPECT_FALSE(rz_false_type::value);
    }

    TEST(TypeTraitsTest, IntegralConstant_Operators)
    {
        using five = rz_integral_constant<int, 5>;
        five f;
        EXPECT_EQ(f(), 5);
        EXPECT_EQ(static_cast<int>(f), 5);
    }

    //============================================================================
    // IS_SAME TESTS
    //============================================================================

    TEST(TypeTraitsTest, IsSame_IdenticalTypes)
    {
        EXPECT_TRUE((rz_is_same_v<int, int>) );
        EXPECT_TRUE((rz_is_same_v<float, float>) );
        EXPECT_TRUE((rz_is_same_v<const int, const int>) );
        EXPECT_TRUE((rz_is_same_v<int*, int*>) );
    }

    TEST(TypeTraitsTest, IsSame_DifferentTypes)
    {
        EXPECT_FALSE((rz_is_same_v<int, float>) );
        EXPECT_FALSE((rz_is_same_v<int, unsigned int>) );
        EXPECT_FALSE((rz_is_same_v<int, const int>) );
        EXPECT_FALSE((rz_is_same_v<int, int&>) );
        EXPECT_FALSE((rz_is_same_v<int, int*>) );
    }

    //============================================================================
    // IS_POINTER TESTS
    //============================================================================

    TEST(TypeTraitsTest, IsPointer_PointerTypes)
    {
        EXPECT_TRUE((rz_is_pointer_v<int*>) );
        EXPECT_TRUE((rz_is_pointer_v<const int*>) );
        EXPECT_TRUE((rz_is_pointer_v<volatile int*>) );
        EXPECT_TRUE((rz_is_pointer_v<const volatile int*>) );
        EXPECT_TRUE((rz_is_pointer_v<float*>) );
        EXPECT_TRUE((rz_is_pointer_v<void*>) );
    }

    TEST(TypeTraitsTest, IsPointer_NonPointerTypes)
    {
        EXPECT_FALSE((rz_is_pointer_v<int>) );
        EXPECT_FALSE((rz_is_pointer_v<int&>) );
        EXPECT_FALSE((rz_is_pointer_v<int&&>) );
        EXPECT_FALSE((rz_is_pointer_v<const int>) );
    }

    //============================================================================
    // IS_INTEGRAL TESTS
    //============================================================================

    TEST(TypeTraitsTest, IsIntegral_IntegralTypes)
    {
        EXPECT_TRUE((rz_is_integral_v<bool>) );
        EXPECT_TRUE((rz_is_integral_v<char>) );
        EXPECT_TRUE((rz_is_integral_v<signed char>) );
        EXPECT_TRUE((rz_is_integral_v<unsigned char>) );
        EXPECT_TRUE((rz_is_integral_v<wchar_t>) );
        EXPECT_TRUE((rz_is_integral_v<char16_t>) );
        EXPECT_TRUE((rz_is_integral_v<char32_t>) );
        EXPECT_TRUE((rz_is_integral_v<short>) );
        EXPECT_TRUE((rz_is_integral_v<unsigned short>) );
        EXPECT_TRUE((rz_is_integral_v<int>) );
        EXPECT_TRUE((rz_is_integral_v<unsigned int>) );
        EXPECT_TRUE((rz_is_integral_v<long>) );
        EXPECT_TRUE((rz_is_integral_v<unsigned long>) );
        EXPECT_TRUE((rz_is_integral_v<long long>) );
        EXPECT_TRUE((rz_is_integral_v<unsigned long long>) );
    }

    TEST(TypeTraitsTest, IsIntegral_CVQualified)
    {
        EXPECT_TRUE((rz_is_integral_v<const int>) );
        EXPECT_TRUE((rz_is_integral_v<volatile int>) );
        EXPECT_TRUE((rz_is_integral_v<const volatile int>) );
        EXPECT_TRUE((rz_is_integral_v<const unsigned long>) );
    }

    TEST(TypeTraitsTest, IsIntegral_NonIntegralTypes)
    {
        EXPECT_FALSE((rz_is_integral_v<float>) );
        EXPECT_FALSE((rz_is_integral_v<double>) );
        EXPECT_FALSE((rz_is_integral_v<long double>) );
        EXPECT_FALSE((rz_is_integral_v<int*>) );
        EXPECT_FALSE((rz_is_integral_v<int&>) );
    }

    //============================================================================
    // IS_FLOATING_POINT TESTS
    //============================================================================

    TEST(TypeTraitsTest, IsFloatingPoint_FloatingTypes)
    {
        EXPECT_TRUE((rz_is_floating_point_v<float>) );
        EXPECT_TRUE((rz_is_floating_point_v<double>) );
        EXPECT_TRUE((rz_is_floating_point_v<long double>) );
    }

    TEST(TypeTraitsTest, IsFloatingPoint_CVQualified)
    {
        EXPECT_TRUE((rz_is_floating_point_v<const float>) );
        EXPECT_TRUE((rz_is_floating_point_v<volatile double>) );
        EXPECT_TRUE((rz_is_floating_point_v<const volatile long double>) );
    }

    TEST(TypeTraitsTest, IsFloatingPoint_NonFloatingTypes)
    {
        EXPECT_FALSE((rz_is_floating_point_v<int>) );
        EXPECT_FALSE((rz_is_floating_point_v<bool>) );
        EXPECT_FALSE((rz_is_floating_point_v<char>) );
        EXPECT_FALSE((rz_is_floating_point_v<float*>) );
        EXPECT_FALSE((rz_is_floating_point_v<float&>) );
    }

    //============================================================================
    // IS_ARITHMETIC TESTS
    //============================================================================

    TEST(TypeTraitsTest, IsArithmetic_IntegralTypes)
    {
        EXPECT_TRUE((rz_is_arithmetic_v<int>) );
        EXPECT_TRUE((rz_is_arithmetic_v<unsigned int>) );
        EXPECT_TRUE((rz_is_arithmetic_v<char>) );
        EXPECT_TRUE((rz_is_arithmetic_v<bool>) );
    }

    TEST(TypeTraitsTest, IsArithmetic_FloatingPointTypes)
    {
        EXPECT_TRUE((rz_is_arithmetic_v<float>) );
        EXPECT_TRUE((rz_is_arithmetic_v<double>) );
        EXPECT_TRUE((rz_is_arithmetic_v<long double>) );
    }

    TEST(TypeTraitsTest, IsArithmetic_CVQualified)
    {
        EXPECT_TRUE((rz_is_arithmetic_v<const int>) );
        EXPECT_TRUE((rz_is_arithmetic_v<volatile float>) );
        EXPECT_TRUE((rz_is_arithmetic_v<const volatile double>) );
    }

    TEST(TypeTraitsTest, IsArithmetic_NonArithmeticTypes)
    {
        EXPECT_FALSE((rz_is_arithmetic_v<int*>) );
        EXPECT_FALSE((rz_is_arithmetic_v<int&>) );
        EXPECT_FALSE((rz_is_arithmetic_v<void>) );
    }

    //============================================================================
    // IS_SIGNED TESTS
    //============================================================================

    TEST(TypeTraitsTest, IsSigned_SignedIntegralTypes)
    {
        EXPECT_TRUE((rz_is_signed_v<signed char>) );
        EXPECT_TRUE((rz_is_signed_v<short>) );
        EXPECT_TRUE((rz_is_signed_v<int>) );
        EXPECT_TRUE((rz_is_signed_v<long>) );
        EXPECT_TRUE((rz_is_signed_v<long long>) );
    }

    TEST(TypeTraitsTest, IsSigned_FloatingPointTypes)
    {
        EXPECT_TRUE((rz_is_signed_v<float>) );
        EXPECT_TRUE((rz_is_signed_v<double>) );
        EXPECT_TRUE((rz_is_signed_v<long double>) );
    }

    TEST(TypeTraitsTest, IsSigned_CVQualified)
    {
        EXPECT_TRUE((rz_is_signed_v<const int>) );
        EXPECT_TRUE((rz_is_signed_v<volatile int>) );
        EXPECT_TRUE((rz_is_signed_v<const volatile int>) );
        EXPECT_TRUE((rz_is_signed_v<const float>) );
    }

    TEST(TypeTraitsTest, IsSigned_UnsignedTypes)
    {
        EXPECT_FALSE((rz_is_signed_v<unsigned char>) );
        EXPECT_FALSE((rz_is_signed_v<unsigned short>) );
        EXPECT_FALSE((rz_is_signed_v<unsigned int>) );
        EXPECT_FALSE((rz_is_signed_v<unsigned long>) );
        EXPECT_FALSE((rz_is_signed_v<unsigned long long>) );
        EXPECT_FALSE((rz_is_signed_v<bool>) );
    }

    TEST(TypeTraitsTest, IsSigned_NonArithmeticTypes)
    {
        EXPECT_FALSE((rz_is_signed_v<int*>) );
        EXPECT_FALSE((rz_is_signed_v<int&>) );
        EXPECT_FALSE((rz_is_signed_v<void>) );
    }

    //============================================================================
    // IS_UNSIGNED TESTS
    //============================================================================

    TEST(TypeTraitsTest, IsUnsigned_UnsignedTypes)
    {
        EXPECT_TRUE((rz_is_unsigned_v<unsigned char>) );
        EXPECT_TRUE((rz_is_unsigned_v<unsigned short>) );
        EXPECT_TRUE((rz_is_unsigned_v<unsigned int>) );
        EXPECT_TRUE((rz_is_unsigned_v<unsigned long>) );
        EXPECT_TRUE((rz_is_unsigned_v<unsigned long long>) );
        EXPECT_TRUE((rz_is_unsigned_v<bool>) );
    }

    TEST(TypeTraitsTest, IsUnsigned_CVQualified)
    {
        EXPECT_TRUE((rz_is_unsigned_v<const unsigned int>) );
        EXPECT_TRUE((rz_is_unsigned_v<volatile unsigned int>) );
        EXPECT_TRUE((rz_is_unsigned_v<const volatile unsigned int>) );
    }

    TEST(TypeTraitsTest, IsUnsigned_SignedTypes)
    {
        EXPECT_FALSE((rz_is_unsigned_v<signed char>) );
        EXPECT_FALSE((rz_is_unsigned_v<short>) );
        EXPECT_FALSE((rz_is_unsigned_v<int>) );
        EXPECT_FALSE((rz_is_unsigned_v<long>) );
        EXPECT_FALSE((rz_is_unsigned_v<long long>) );
    }

    TEST(TypeTraitsTest, IsUnsigned_FloatingPointTypes)
    {
        EXPECT_FALSE((rz_is_unsigned_v<float>) );
        EXPECT_FALSE((rz_is_unsigned_v<double>) );
        EXPECT_FALSE((rz_is_unsigned_v<long double>) );
    }

    TEST(TypeTraitsTest, IsUnsigned_NonArithmeticTypes)
    {
        EXPECT_FALSE((rz_is_unsigned_v<int*>) );
        EXPECT_FALSE((rz_is_unsigned_v<int&>) );
        EXPECT_FALSE((rz_is_unsigned_v<void>) );
    }

    //============================================================================
    // IS_CONST / IS_VOLATILE TESTS
    //============================================================================

    TEST(TypeTraitsTest, IsConst_ConstTypes)
    {
        EXPECT_TRUE((rz_is_const_v<const int>) );
        EXPECT_TRUE((rz_is_const_v<const float>) );
        EXPECT_TRUE((rz_is_const_v<const volatile int>) );
    }

    TEST(TypeTraitsTest, IsConst_NonConstTypes)
    {
        EXPECT_FALSE((rz_is_const_v<int>) );
        EXPECT_FALSE((rz_is_const_v<volatile int>) );
        EXPECT_FALSE((rz_is_const_v<int*>) );
        // Note: const int* is pointer to const, not const pointer
        EXPECT_FALSE((rz_is_const_v<const int*>) );
    }

    TEST(TypeTraitsTest, IsVolatile_VolatileTypes)
    {
        EXPECT_TRUE((rz_is_volatile_v<volatile int>) );
        EXPECT_TRUE((rz_is_volatile_v<volatile float>) );
        EXPECT_TRUE((rz_is_volatile_v<const volatile int>) );
    }

    TEST(TypeTraitsTest, IsVolatile_NonVolatileTypes)
    {
        EXPECT_FALSE((rz_is_volatile_v<int>) );
        EXPECT_FALSE((rz_is_volatile_v<const int>) );
        EXPECT_FALSE((rz_is_volatile_v<int*>) );
    }

    //============================================================================
    // COMPLEX COMBINATION TESTS
    //============================================================================

    TEST(TypeTraitsTest, ComplexCombinations_PointerAndCV)
    {
        // int* is a pointer, but not const
        EXPECT_TRUE((rz_is_pointer_v<int*>) );
        EXPECT_FALSE((rz_is_const_v<int*>) );

        // int* const is a const pointer
        EXPECT_TRUE((rz_is_pointer_v<int* const>) );
        EXPECT_TRUE((rz_is_const_v<int* const>) );

        // const int* is a pointer to const
        EXPECT_TRUE((rz_is_pointer_v<const int*>) );
        EXPECT_FALSE((rz_is_const_v<const int*>) );
    }

    TEST(TypeTraitsTest, ComplexCombinations_TypeTransformations)
    {
        using Type1    = const volatile int&;
        using Stripped = rz_remove_cvref_t<Type1>;

        EXPECT_TRUE((rz_is_same_v<Stripped, int>) );
        EXPECT_TRUE((rz_is_integral_v<Stripped>) );
        EXPECT_TRUE((rz_is_signed_v<Stripped>) );
        EXPECT_FALSE((rz_is_const_v<Stripped>) );
    }

    TEST(TypeTraitsTest, ComplexCombinations_ArithmeticChecks)
    {
        // Signed and unsigned should be mutually exclusive for integral types
        EXPECT_TRUE((rz_is_signed_v<int> && !rz_is_unsigned_v<int>) );
        EXPECT_TRUE((rz_is_unsigned_v<unsigned int> && !rz_is_signed_v<unsigned int>) );

        // Floating point should be signed but not unsigned
        EXPECT_TRUE((rz_is_signed_v<float> && !rz_is_unsigned_v<float>) );

        // Both should be arithmetic
        EXPECT_TRUE((rz_is_arithmetic_v<int>) );
        EXPECT_TRUE((rz_is_arithmetic_v<unsigned int>) );
        EXPECT_TRUE((rz_is_arithmetic_v<float>) );
    }

    //============================================================================
    // IS_VOID TESTS
    //============================================================================

    TEST(TypeTraitsTest, IsVoid_VoidTypes)
    {
        EXPECT_TRUE((rz_is_void_v<void>) );
        EXPECT_TRUE((rz_is_void_v<const void>) );
        EXPECT_TRUE((rz_is_void_v<volatile void>) );
        EXPECT_TRUE((rz_is_void_v<const volatile void>) );
    }

    TEST(TypeTraitsTest, IsVoid_NonVoidTypes)
    {
        EXPECT_FALSE((rz_is_void_v<int>) );
        EXPECT_FALSE((rz_is_void_v<float>) );
        EXPECT_FALSE((rz_is_void_v<void*>) );
        EXPECT_FALSE((rz_is_void_v<void()>) );    // function type returning void
    }

    //============================================================================
    // IS_CLASS TESTS
    //============================================================================

    class TestClass
    {};
    struct TestStruct
    {};
    union TestUnion
    {
        int   a;
        float b;
    };
    enum TestEnum
    {
        A,
        B,
        C
    };
    enum class TestEnumClass
    {
        X,
        Y,
        Z
    };

    TEST(TypeTraitsTest, IsClass_ClassTypes)
    {
        EXPECT_TRUE((rz_is_class_v<TestClass>) );
        EXPECT_TRUE((rz_is_class_v<TestStruct>) );    // struct IS a class
        EXPECT_TRUE((rz_is_class_v<const TestClass>) );
        EXPECT_TRUE((rz_is_class_v<volatile TestStruct>) );
    }

    TEST(TypeTraitsTest, IsClass_NonClassTypes)
    {
        EXPECT_FALSE((rz_is_class_v<int>) );
        EXPECT_FALSE((rz_is_class_v<int*>) );
        EXPECT_FALSE((rz_is_class_v<int&>) );
        //EXPECT_FALSE((rz_is_class_v<TestUnion>) );    // unions are NOT classes
        EXPECT_FALSE((rz_is_class_v<TestEnum>) );
        EXPECT_FALSE((rz_is_class_v<TestEnumClass>) );
        EXPECT_FALSE((rz_is_class_v<void>) );
    }

    //============================================================================
    // IS_UNION TESTS (if implemented)
    //============================================================================

    TEST(TypeTraitsTest, IsUnion_UnionTypes)
    {
        EXPECT_TRUE((rz_is_union_v<TestUnion>) );
        EXPECT_TRUE((rz_is_union_v<const TestUnion>) );
    }

    TEST(TypeTraitsTest, IsUnion_NonUnionTypes)
    {
        EXPECT_FALSE((rz_is_union_v<TestClass>) );
        EXPECT_FALSE((rz_is_union_v<TestStruct>) );
        EXPECT_FALSE((rz_is_union_v<int>) );
    }

    //============================================================================
    // ENABLE_IF TESTS
    //============================================================================

    template<typename T>
    rz_enable_if_t<rz_is_integral_v<T>, int>
    helper_integral(T)
    {
        return 1;
    }

    template<typename T>
    rz_enable_if_t<rz_is_floating_point_v<T>, int>
    helper_floating(T)
    {
        return 2;
    }

    TEST(TypeTraitsTest, EnableIf_ConditionalOverloads)
    {
        EXPECT_EQ(helper_integral(42), 1);
        EXPECT_EQ(helper_floating(3.14), 2);

        // These would fail to compile (correctly):
        // helper_integral(3.14);  // Error: no matching function
        // helper_floating(42);    // Error: no matching function
    }

    TEST(TypeTraitsTest, EnableIf_TypeExists)
    {
        // When condition is true, type exists
        EXPECT_TRUE((rz_is_same_v<rz_enable_if_t<true, int>, int>) );
        EXPECT_TRUE((rz_is_same_v<rz_enable_if_t<true>, void>) );    // Default is void

        // When condition is false, there's no 'type' member
        // This would fail to compile:
        // using BadType = rz_enable_if_t<false, int>;  // Error: no member 'type'
    }

    //============================================================================
    // IS_ENUM TESTS
    //============================================================================

    enum class TestEnum2 : uint8_t
    {
        X,
        Y,
        Z
    };
    enum class TestEnum3
    {
        P,
        Q,
        R
    };

    TEST(TypeTraitsTest, IsEnum_EnumTypes)
    {
        EXPECT_TRUE((rz_is_enum_v<TestEnum>) );
        EXPECT_TRUE((rz_is_enum_v<TestEnum2>) );
        EXPECT_TRUE((rz_is_enum_v<TestEnum3>) );
        EXPECT_TRUE((rz_is_enum_v<const TestEnum>) );
    }

    TEST(TypeTraitsTest, IsEnum_NonEnumTypes)
    {
        EXPECT_FALSE((rz_is_enum_v<int>) );
        EXPECT_FALSE((rz_is_enum_v<float>) );
        EXPECT_FALSE((rz_is_enum_v<TestClass>) );
        EXPECT_FALSE((rz_is_enum_v<void>) );
    }

    //============================================================================
    // UNDERLYING_TYPE TESTS
    //============================================================================

    enum class SmallEnum : uint8_t
    {
        S1,
        S2
    };
    enum class LargeEnum : uint64_t
    {
        L1 = 1ULL << 40
    };

    TEST(TypeTraitsTest, UnderlyingType_ExplicitTypes)
    {
        EXPECT_TRUE((rz_is_same_v<rz_underlying_type_t<SmallEnum>, uint8_t>) );
        EXPECT_TRUE((rz_is_same_v<rz_underlying_type_t<LargeEnum>, uint64_t>) );
    }

    // TEST(TypeTraitsTest, ToUnderlying_Conversion)
    // {
    //     SmallEnum e     = SmallEnum::S2;
    //     uint8_t   value = rz_to_underlying(e);
    //     EXPECT_EQ(value, static_cast<uint8_t>(SmallEnum::S2));
    //
    //     // Compile-time conversion
    //     constexpr auto compile_time = rz_to_underlying(LargeEnum::L1);
    //     EXPECT_EQ(compile_time, 1ULL << 40);
    // }

// 1. Default constructible only
struct DefaultOnly {
    DefaultOnly() = default;
    DefaultOnly(const DefaultOnly&) = delete;
    DefaultOnly(DefaultOnly&&) = delete;
};

// 2. Copy constructible only (move deleted)
struct CopyOnly {
    CopyOnly() = default;
    CopyOnly(const CopyOnly& other) {
        (void)other;  // Suppress unused parameter warning
    }
    CopyOnly(CopyOnly&&) = delete;
};

// 3. Move constructible only (copy deleted)
struct MoveOnly {
    MoveOnly() = default;
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&& other) noexcept {
        (void)other;
    }
};

// 4. Both copy and move constructible
struct CopyAndMove {
    CopyAndMove() = default;
    CopyAndMove(const CopyAndMove& other) {
        (void)other;
    }
    CopyAndMove(CopyAndMove&& other) noexcept {
        (void)other;
    }
};

// 5. No default constructor, but has copy and move
struct NoDefault {
    NoDefault(int x) : value(x) {}
    NoDefault(const NoDefault& other) : value(other.value) {}
    NoDefault(NoDefault&& other) noexcept : value(other.value) {}
    int value;
};

// 6. All constructors deleted
struct NoCtor {
    NoCtor() = delete;
    NoCtor(const NoCtor&) = delete;
    NoCtor(NoCtor&&) = delete;
};

// 7. Implicit copy/move (compiler generated)
struct ImplicitCopyMove {
    int x;
    double y;
};

// 8. Copy constructor throws
struct ThrowingCopy {
    ThrowingCopy() = default;
    ThrowingCopy(const ThrowingCopy&) { throw 42; }
    ThrowingCopy(ThrowingCopy&&) noexcept = default;
};

// 9. Move constructor throws
struct ThrowingMove {
    ThrowingMove() = default;
    ThrowingMove(const ThrowingMove&) = default;
    ThrowingMove(ThrowingMove&&) { throw 42; }
};

// 10. Private copy constructor
class PrivateCopy {
public:
    PrivateCopy() = default;
    PrivateCopy(PrivateCopy&&) = default;
private:
    PrivateCopy(const PrivateCopy&) = default;
};

// 11. Private move constructor
class PrivateMove {
public:
    PrivateMove() = default;
    PrivateMove(const PrivateMove&) = default;
private:
    PrivateMove(PrivateMove&&) = default;
};

// 12. Abstract class (has pure virtual function)
class AbstractClass {
public:
    virtual ~AbstractClass() = default;
    virtual void pureVirtual() = 0;
};

// 13. Class with deleted copy but implicit move
struct DeletedCopyImplicitMove {
    DeletedCopyImplicitMove() = default;
    DeletedCopyImplicitMove(const DeletedCopyImplicitMove&) = delete;
    // Move constructor implicitly deleted when copy is deleted
};

//============================================================================
// IS_COPY_CONSTRUCTIBLE TESTS - Basic Types
//============================================================================

TEST(TypeTraitsTest, IsCopyConstructible_PrimitiveTypes)
{
    EXPECT_TRUE((rz_is_copy_constructible_v<bool>));
    EXPECT_TRUE((rz_is_copy_constructible_v<char>));
    EXPECT_TRUE((rz_is_copy_constructible_v<signed char>));
    EXPECT_TRUE((rz_is_copy_constructible_v<unsigned char>));
    EXPECT_TRUE((rz_is_copy_constructible_v<short>));
    EXPECT_TRUE((rz_is_copy_constructible_v<unsigned short>));
    EXPECT_TRUE((rz_is_copy_constructible_v<int>));
    EXPECT_TRUE((rz_is_copy_constructible_v<unsigned int>));
    EXPECT_TRUE((rz_is_copy_constructible_v<long>));
    EXPECT_TRUE((rz_is_copy_constructible_v<unsigned long>));
    EXPECT_TRUE((rz_is_copy_constructible_v<long long>));
    EXPECT_TRUE((rz_is_copy_constructible_v<unsigned long long>));
    EXPECT_TRUE((rz_is_copy_constructible_v<float>));
    EXPECT_TRUE((rz_is_copy_constructible_v<double>));
    EXPECT_TRUE((rz_is_copy_constructible_v<long double>));
}

TEST(TypeTraitsTest, IsCopyConstructible_Pointers)
{
    EXPECT_TRUE((rz_is_copy_constructible_v<int*>));
    EXPECT_TRUE((rz_is_copy_constructible_v<const int*>));
    EXPECT_TRUE((rz_is_copy_constructible_v<void*>));
    EXPECT_TRUE((rz_is_copy_constructible_v<char*>));
    EXPECT_TRUE((rz_is_copy_constructible_v<ImplicitCopyMove*>));
}

//============================================================================
// IS_COPY_CONSTRUCTIBLE TESTS - References
//============================================================================

TEST(TypeTraitsTest, IsCopyConstructible_LValueReferences)
{
    // Lvalue references ARE copy constructible (can bind to const&)
    EXPECT_TRUE((rz_is_copy_constructible_v<int&>));
    EXPECT_TRUE((rz_is_copy_constructible_v<const int&>));
    EXPECT_TRUE((rz_is_copy_constructible_v<CopyAndMove&>));
}

TEST(TypeTraitsTest, IsCopyConstructible_RValueReferences)
{
    // Rvalue references ARE copy constructible (can bind to const&)
    EXPECT_TRUE((rz_is_copy_constructible_v<int&&>));
    EXPECT_TRUE((rz_is_copy_constructible_v<CopyAndMove&&>));
}

//============================================================================
// IS_COPY_CONSTRUCTIBLE TESTS - Classes
//============================================================================

TEST(TypeTraitsTest, IsCopyConstructible_UserDefinedClasses)
{
    EXPECT_FALSE((rz_is_copy_constructible_v<DefaultOnly>));
    EXPECT_TRUE((rz_is_copy_constructible_v<CopyOnly>));
    EXPECT_FALSE((rz_is_copy_constructible_v<MoveOnly>));
    EXPECT_TRUE((rz_is_copy_constructible_v<CopyAndMove>));
    EXPECT_TRUE((rz_is_copy_constructible_v<NoDefault>));
    EXPECT_FALSE((rz_is_copy_constructible_v<NoCtor>));
    EXPECT_TRUE((rz_is_copy_constructible_v<ImplicitCopyMove>));
}

TEST(TypeTraitsTest, IsCopyConstructible_ThrowingConstructors)
{
    // Even if copy constructor throws, it's still copy constructible
    EXPECT_TRUE((rz_is_copy_constructible_v<ThrowingCopy>));
    EXPECT_TRUE((rz_is_copy_constructible_v<ThrowingMove>));
}

TEST(TypeTraitsTest, IsCopyConstructible_PrivateConstructors)
{
    // Private copy constructor means NOT copy constructible
    EXPECT_FALSE((rz_is_copy_constructible_v<PrivateCopy>));
    EXPECT_TRUE((rz_is_copy_constructible_v<PrivateMove>));
}

TEST(TypeTraitsTest, IsCopyConstructible_DeletedCopy)
{
    EXPECT_FALSE((rz_is_copy_constructible_v<DeletedCopyImplicitMove>));
}

//============================================================================
// IS_COPY_CONSTRUCTIBLE TESTS - Special Types
//============================================================================

TEST(TypeTraitsTest, IsCopyConstructible_Arrays)
{
    // Arrays are NOT copy constructible
    EXPECT_FALSE((rz_is_copy_constructible_v<int[5]>));
    EXPECT_FALSE((rz_is_copy_constructible_v<int[3][4]>));
    EXPECT_FALSE((rz_is_copy_constructible_v<CopyAndMove[2]>));
}

TEST(TypeTraitsTest, IsCopyConstructible_Void)
{
    EXPECT_FALSE((rz_is_copy_constructible_v<void>));
    EXPECT_FALSE((rz_is_copy_constructible_v<const void>));
}

TEST(TypeTraitsTest, IsCopyConstructible_AbstractClass)
{
    // Abstract classes cannot be instantiated, but trait checks if copy constructor exists
    EXPECT_FALSE((rz_is_copy_constructible_v<AbstractClass>));
}

//============================================================================
// IS_MOVE_CONSTRUCTIBLE TESTS - Basic Types
//============================================================================

TEST(TypeTraitsTest, IsMoveConstructible_PrimitiveTypes)
{
    EXPECT_TRUE((rz_is_move_constructible_v<bool>));
    EXPECT_TRUE((rz_is_move_constructible_v<char>));
    EXPECT_TRUE((rz_is_move_constructible_v<signed char>));
    EXPECT_TRUE((rz_is_move_constructible_v<unsigned char>));
    EXPECT_TRUE((rz_is_move_constructible_v<short>));
    EXPECT_TRUE((rz_is_move_constructible_v<unsigned short>));
    EXPECT_TRUE((rz_is_move_constructible_v<int>));
    EXPECT_TRUE((rz_is_move_constructible_v<unsigned int>));
    EXPECT_TRUE((rz_is_move_constructible_v<long>));
    EXPECT_TRUE((rz_is_move_constructible_v<unsigned long>));
    EXPECT_TRUE((rz_is_move_constructible_v<long long>));
    EXPECT_TRUE((rz_is_move_constructible_v<unsigned long long>));
    EXPECT_TRUE((rz_is_move_constructible_v<float>));
    EXPECT_TRUE((rz_is_move_constructible_v<double>));
    EXPECT_TRUE((rz_is_move_constructible_v<long double>));
}

TEST(TypeTraitsTest, IsMoveConstructible_Pointers)
{
    EXPECT_TRUE((rz_is_move_constructible_v<int*>));
    EXPECT_TRUE((rz_is_move_constructible_v<const int*>));
    EXPECT_TRUE((rz_is_move_constructible_v<void*>));
    EXPECT_TRUE((rz_is_move_constructible_v<char*>));
    EXPECT_TRUE((rz_is_move_constructible_v<ImplicitCopyMove*>));
}

// TEST(TypeTraitsTest, IsMoveConstructible_CVQualified)
// {
//     EXPECT_TRUE((rz_is_move_constructible_v<const int>));
//     EXPECT_TRUE((rz_is_move_constructible_v<volatile int>));
//     EXPECT_TRUE((rz_is_move_constructible_v<const volatile int>));
//     EXPECT_TRUE((rz_is_move_constructible_v<const CopyAndMove>));
//     EXPECT_TRUE((rz_is_move_constructible_v<volatile CopyAndMove>));
// }

//============================================================================
// IS_MOVE_CONSTRUCTIBLE TESTS - References
//============================================================================

TEST(TypeTraitsTest, IsMoveConstructible_LValueReferences)
{
    // Lvalue references ARE move constructible (rvalue can bind to lvalue ref)
    EXPECT_TRUE((rz_is_move_constructible_v<int&>));
    EXPECT_TRUE((rz_is_move_constructible_v<const int&>));
    EXPECT_TRUE((rz_is_move_constructible_v<CopyAndMove&>));
}

TEST(TypeTraitsTest, IsMoveConstructible_RValueReferences)
{
    // Rvalue references ARE move constructible
    EXPECT_TRUE((rz_is_move_constructible_v<int&&>));
    EXPECT_TRUE((rz_is_move_constructible_v<CopyAndMove&&>));
}

//============================================================================
// IS_MOVE_CONSTRUCTIBLE TESTS - Classes
//============================================================================

TEST(TypeTraitsTest, IsMoveConstructible_UserDefinedClasses)
{
    EXPECT_FALSE((rz_is_move_constructible_v<DefaultOnly>));
    EXPECT_FALSE((rz_is_move_constructible_v<CopyOnly>));
    EXPECT_TRUE((rz_is_move_constructible_v<MoveOnly>));
    EXPECT_TRUE((rz_is_move_constructible_v<CopyAndMove>));
    EXPECT_TRUE((rz_is_move_constructible_v<NoDefault>));
    EXPECT_FALSE((rz_is_move_constructible_v<NoCtor>));
    EXPECT_TRUE((rz_is_move_constructible_v<ImplicitCopyMove>));
}

TEST(TypeTraitsTest, IsMoveConstructible_ThrowingConstructors)
{
    // Even if move constructor throws, it's still move constructible
    EXPECT_TRUE((rz_is_move_constructible_v<ThrowingCopy>));
    EXPECT_TRUE((rz_is_move_constructible_v<ThrowingMove>));
}

TEST(TypeTraitsTest, IsMoveConstructible_PrivateConstructors)
{
    // Private move constructor means NOT move constructible
    EXPECT_TRUE((rz_is_move_constructible_v<PrivateCopy>));
    EXPECT_FALSE((rz_is_move_constructible_v<PrivateMove>));
}

TEST(TypeTraitsTest, IsMoveConstructible_DeletedCopy)
{
    // When copy is deleted, move is implicitly deleted too (if not explicitly defined)
    EXPECT_FALSE((rz_is_move_constructible_v<DeletedCopyImplicitMove>));
}

//============================================================================
// IS_MOVE_CONSTRUCTIBLE TESTS - Special Types
//============================================================================

TEST(TypeTraitsTest, IsMoveConstructible_Arrays)
{
    // Arrays are NOT move constructible
    EXPECT_FALSE((rz_is_move_constructible_v<int[5]>));
    EXPECT_FALSE((rz_is_move_constructible_v<int[3][4]>));
    EXPECT_FALSE((rz_is_move_constructible_v<CopyAndMove[2]>));
}
//
// TEST(TypeTraitsTest, IsMoveConstructible_Void)
// {
//     EXPECT_FALSE((rz_is_move_constructible_v<void>));
//     EXPECT_FALSE((rz_is_move_constructible_v<const void>));
// }

TEST(TypeTraitsTest, IsMoveConstructible_AbstractClass)
{
    // Abstract classes cannot be instantiated
    EXPECT_FALSE((rz_is_move_constructible_v<AbstractClass>));
}

//============================================================================
// COMBINED TESTS - Copy vs Move Matrix
//============================================================================

TEST(TypeTraitsTest, ConstructibilityMatrix_DefaultOnly)
{
    EXPECT_FALSE((rz_is_copy_constructible_v<DefaultOnly>));
    EXPECT_FALSE((rz_is_move_constructible_v<DefaultOnly>));
}

TEST(TypeTraitsTest, ConstructibilityMatrix_CopyOnly)
{
    EXPECT_TRUE((rz_is_copy_constructible_v<CopyOnly>));
    EXPECT_FALSE((rz_is_move_constructible_v<CopyOnly>));
}

TEST(TypeTraitsTest, ConstructibilityMatrix_MoveOnly)
{
    EXPECT_FALSE((rz_is_copy_constructible_v<MoveOnly>));
    EXPECT_TRUE((rz_is_move_constructible_v<MoveOnly>));
}

TEST(TypeTraitsTest, ConstructibilityMatrix_CopyAndMove)
{
    EXPECT_TRUE((rz_is_copy_constructible_v<CopyAndMove>));
    EXPECT_TRUE((rz_is_move_constructible_v<CopyAndMove>));
}

TEST(TypeTraitsTest, ConstructibilityMatrix_NoCtor)
{
    EXPECT_FALSE((rz_is_copy_constructible_v<NoCtor>));
    EXPECT_FALSE((rz_is_move_constructible_v<NoCtor>));
}

TEST(TypeTraitsTest, ConstructibilityMatrix_ImplicitCopyMove)
{
    EXPECT_TRUE((rz_is_copy_constructible_v<ImplicitCopyMove>));
    EXPECT_TRUE((rz_is_move_constructible_v<ImplicitCopyMove>));
}

//============================================================================
// EDGE CASES
//============================================================================

TEST(TypeTraitsTest, EdgeCase_PointerToArray)
{
    using ArrayPtr = int(*)[5];
    EXPECT_TRUE((rz_is_copy_constructible_v<ArrayPtr>));
    EXPECT_TRUE((rz_is_move_constructible_v<ArrayPtr>));
}

TEST(TypeTraitsTest, EdgeCase_ReferenceToReference)
{
    // Reference to reference collapses
    using RefRef = int&;
    EXPECT_TRUE((rz_is_copy_constructible_v<RefRef>));
    EXPECT_TRUE((rz_is_move_constructible_v<RefRef>));
}

}    // namespace Razix
