// DataTypesTests.cpp
// Unit tests for the RZDataTypes type definitions
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"

#include <gtest/gtest.h>
#include <type_traits>

namespace Razix {

    class RZDataTypesTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // No special setup needed for type tests
        }

        void TearDown() override
        {
            // No cleanup needed for type tests
        }
    };

    // Test case for integer type sizes
    TEST_F(RZDataTypesTests, IntegerTypeSizes)
    {
        // Test 8-bit types
        EXPECT_EQ(sizeof(i8), 1);
        EXPECT_EQ(sizeof(u8), 1);
        
        // Test 16-bit types
        EXPECT_EQ(sizeof(i16), 2);
        EXPECT_EQ(sizeof(u16), 2);
        
        // Test 32-bit types
        EXPECT_EQ(sizeof(i32), 4);
        EXPECT_EQ(sizeof(u32), 4);
        
        // Test 64-bit types
        EXPECT_EQ(sizeof(i64), 8);
        EXPECT_EQ(sizeof(u64), 8);
    }

    // Test case for floating point type sizes
    TEST_F(RZDataTypesTests, FloatingPointTypeSizes)
    {
        EXPECT_EQ(sizeof(f32), 4);  // float
        EXPECT_EQ(sizeof(d32), 8);  // double
    }

    // Test case for special types
    TEST_F(RZDataTypesTests, SpecialTypes)
    {
        EXPECT_EQ(sizeof(ch), 1);    // char
        EXPECT_TRUE(std::is_pointer_v<cstr>);  // const char*
        EXPECT_EQ(sizeof(sz), sizeof(size_t)); // size_t
        EXPECT_EQ(sizeof(ulong), sizeof(unsigned long));
    }

    // Test case for signed/unsigned properties
    TEST_F(RZDataTypesTests, SignedUnsignedProperties)
    {
        // Test signed types
        EXPECT_TRUE(std::is_signed_v<i8>);
        EXPECT_TRUE(std::is_signed_v<i16>);
        EXPECT_TRUE(std::is_signed_v<i32>);
        EXPECT_TRUE(std::is_signed_v<i64>);
        EXPECT_TRUE(std::is_signed_v<f32>);
        EXPECT_TRUE(std::is_signed_v<d32>);
        
        // Test unsigned types
        EXPECT_TRUE(std::is_unsigned_v<u8>);
        EXPECT_TRUE(std::is_unsigned_v<u16>);
        EXPECT_TRUE(std::is_unsigned_v<u32>);
        EXPECT_TRUE(std::is_unsigned_v<u64>);
        EXPECT_TRUE(std::is_unsigned_v<sz>);
        EXPECT_TRUE(std::is_unsigned_v<ulong>);
    }

    // Test case for value ranges
    TEST_F(RZDataTypesTests, ValueRanges)
    {
        // Test that types can hold expected values
        i8 signed8 = -128;
        EXPECT_EQ(signed8, -128);
        
        u8 unsigned8 = 255;
        EXPECT_EQ(unsigned8, 255);
        
        i16 signed16 = -32768;
        EXPECT_EQ(signed16, -32768);
        
        u16 unsigned16 = 65535;
        EXPECT_EQ(unsigned16, 65535);
        
        i32 signed32 = -2147483648;
        EXPECT_EQ(signed32, -2147483648);
        
        u32 unsigned32 = 4294967295U;
        EXPECT_EQ(unsigned32, 4294967295U);
        
        f32 float32 = 3.14159f;
        EXPECT_NEAR(float32, 3.14159f, 0.00001f);
        
        d32 double32 = 3.141592653589793;
        EXPECT_NEAR(double32, 3.141592653589793, 0.000000000000001);
    }

    // Test case for GLM vector types
    TEST_F(RZDataTypesTests, GLMVectorTypes)
    {
        // Test uint vector types
        uint1 u1(42);
        EXPECT_EQ(u1.x, 42);
        
        uint2 u2(1, 2);
        EXPECT_EQ(u2.x, 1);
        EXPECT_EQ(u2.y, 2);
        
        uint3 u3(1, 2, 3);
        EXPECT_EQ(u3.x, 1);
        EXPECT_EQ(u3.y, 2);
        EXPECT_EQ(u3.z, 3);
        
        uint4 u4(1, 2, 3, 4);
        EXPECT_EQ(u4.x, 1);
        EXPECT_EQ(u4.y, 2);
        EXPECT_EQ(u4.z, 3);
        EXPECT_EQ(u4.w, 4);
    }

    // Test case for type assignment and arithmetic
    TEST_F(RZDataTypesTests, TypeArithmetic)
    {
        // Test basic arithmetic operations work correctly
        u32 a = 10;
        u32 b = 20;
        u32 sum = a + b;
        EXPECT_EQ(sum, 30);
        
        i32 c = -5;
        i32 d = 15;
        i32 diff = d + c;
        EXPECT_EQ(diff, 10);
        
        f32 e = 2.5f;
        f32 f = 4.0f;
        f32 product = e * f;
        EXPECT_NEAR(product, 10.0f, 0.001f);
    }

    // Test case for type conversions
    TEST_F(RZDataTypesTests, TypeConversions)
    {
        // Test safe conversions
        u8 smallVal = 100;
        u32 largeVal = smallVal;
        EXPECT_EQ(largeVal, 100);
        
        f32 floatVal = 3.14f;
        d32 doubleVal = floatVal;
        EXPECT_NEAR(doubleVal, 3.14, 0.001);
        
        // Test const char* usage
        cstr testStr = "Hello Razix";
        EXPECT_STREQ(testStr, "Hello Razix");
    }

}    // namespace Razix