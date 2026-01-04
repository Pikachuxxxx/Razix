// TypeRegistryTests.cpp
// AI-generated unit tests for the Type Registration classes
#include <Razix/Core/Reflection/RZReflection.h>
#include <gtest/gtest.h>
#include <string>

namespace Razix {
    // Dummy struct for testing reflection
    struct TestStruct
    {
        int    intValue;
        float  floatValue;
        double doubleValue;
        char   charValue;
    };

    // Register the type
    RAZIX_REFLECT_TYPE_START(TestStruct)
    RAZIX_REFLECT_MEMBER(intValue)
    RAZIX_REFLECT_MEMBER(floatValue)
    RAZIX_REFLECT_MEMBER(doubleValue)
    RAZIX_REFLECT_MEMBER(charValue)
    RAZIX_REFLECT_TYPE_END(TestStruct)

    // Fixture for Reflection Tests
    class RZReflectionTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Ensure the type is registered (happens automatically via static block)
        }

        void TearDown() override
        {
            // Clean up after tests if necessary
        }
    };

    // Test: Type Registration
    TEST_F(RZReflectionTests, TypeRegistration)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<TestStruct>();
        ASSERT_NE(metaData, nullptr) << "Metadata for TestStruct should not be null.";
    }

    // Test: Metadata Integrity
    TEST_F(RZReflectionTests, MetadataIntegrity)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<TestStruct>();
        ASSERT_NE(metaData, nullptr);

        EXPECT_EQ(metaData->name, "TestStruct");
        EXPECT_EQ(metaData->typeName, typeid(TestStruct).name());
        EXPECT_EQ(metaData->size, sizeof(TestStruct));
    }

    // Test: Member Metadata
    TEST_F(RZReflectionTests, MemberMetadata)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<TestStruct>();
        ASSERT_NE(metaData, nullptr);

        ASSERT_EQ(metaData->members.size(), 4) << "TestStruct should have 4 members.";

        // Verify each member
        const auto& members = metaData->members;

        EXPECT_EQ(members[0].name, "intValue");
        EXPECT_EQ(members[0].typeName, typeid(int).name());
        EXPECT_EQ(members[0].offset, offsetof(TestStruct, intValue));
        EXPECT_EQ(members[0].size, sizeof(int));

        EXPECT_EQ(members[1].name, "floatValue");
        EXPECT_EQ(members[1].typeName, typeid(float).name());
        EXPECT_EQ(members[1].offset, offsetof(TestStruct, floatValue));
        EXPECT_EQ(members[1].size, sizeof(float));

        EXPECT_EQ(members[2].name, "doubleValue");
        EXPECT_EQ(members[2].typeName, typeid(double).name());
        EXPECT_EQ(members[2].offset, offsetof(TestStruct, doubleValue));
        EXPECT_EQ(members[2].size, sizeof(double));

        EXPECT_EQ(members[3].name, "charValue");
        EXPECT_EQ(members[3].typeName, typeid(char).name());
        EXPECT_EQ(members[3].offset, offsetof(TestStruct, charValue));
        EXPECT_EQ(members[3].size, sizeof(char));
    }

    // Test: Missing Type Metadata
    TEST_F(RZReflectionTests, MissingTypeMetadata)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<std::string>();
        EXPECT_EQ(metaData, nullptr) << "Metadata for unregistered type should be null.";
    }

    // Test: Registering Another Type
    struct AnotherStruct
    {
        bool flag;
        int  count;
    };

    RAZIX_REFLECT_TYPE_START(AnotherStruct)
    RAZIX_REFLECT_MEMBER(flag)
    RAZIX_REFLECT_MEMBER(count)
    RAZIX_REFLECT_TYPE_END(AnotherStruct)

    TEST_F(RZReflectionTests, RegisterAnotherType)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<AnotherStruct>();
        ASSERT_NE(metaData, nullptr);

        EXPECT_EQ(metaData->name, "AnotherStruct");
        EXPECT_EQ(metaData->typeName, typeid(AnotherStruct).name());
        EXPECT_EQ(metaData->size, sizeof(AnotherStruct));

        ASSERT_EQ(metaData->members.size(), 2) << "AnotherStruct should have 2 members.";

        const auto& members = metaData->members;

        EXPECT_EQ(members[0].name, "flag");
        EXPECT_EQ(members[0].typeName, typeid(bool).name());
        EXPECT_EQ(members[0].offset, offsetof(AnotherStruct, flag));
        EXPECT_EQ(members[0].size, sizeof(bool));

        EXPECT_EQ(members[1].name, "count");
        EXPECT_EQ(members[1].typeName, typeid(int).name());
        EXPECT_EQ(members[1].offset, offsetof(AnotherStruct, count));
        EXPECT_EQ(members[1].size, sizeof(int));
    }
}    // namespace Razix
