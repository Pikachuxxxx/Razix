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
}    // namespace Razix
