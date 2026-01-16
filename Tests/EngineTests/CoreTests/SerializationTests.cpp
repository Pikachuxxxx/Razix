// Serialization.cpp
// Pikachuxxxx + AI-generated unit tests for the Type Registration classes
#include <Razix/Core/Reflection/RZReflection.h>
#include <Razix/Core/Serialization/RZSerializable.h>
#include <gtest/gtest.h>
#include <string>
#include <stdio.h>

namespace Razix {
    // Dummy struct for testing reflection and serialization
    struct PlayerStruct
    {
        int    health;
        float  rage;
        char*  name;
        double stamina;
    };

    // Register the type
    RAZIX_REFLECT_TYPE_START(PlayerStruct)
    RAZIX_REFLECT_MEMBER(health)
    RAZIX_REFLECT_MEMBER(rage)
    RAZIX_REFLECT_MEMBER(name)
    RAZIX_REFLECT_MEMBER(stamina)
    RAZIX_REFLECT_TYPE_END(PlayerStruct)

    // Fixture for Serialization Tests
    class RZSerializationTests : public ::testing::Test
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
    TEST_F(RZSerializationTests, PODSerializationHandshake)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<PlayerStruct>();
        ASSERT_NE(metaData, nullptr) << "Metadata for PlayerStruct should not be null.";

        EXPECT_EQ(metaData->name, "PlayerStruct");
        EXPECT_EQ(metaData->typeName, typeid(PlayerStruct).name());
        EXPECT_EQ(metaData->size, sizeof(PlayerStruct));

        // now reflection is done we can test serialization
        PlayerStruct playerOriginal;
        playerOriginal.health  = 100;
        playerOriginal.rage    = 75.5f;
        playerOriginal.stamina = 50.25;
        playerOriginal.name    = const_cast<char*>("Hero, Kratos is god hello ik tghis is a long string");

        auto serializedData = RZSerializable<PlayerStruct>::serializeToBinary(playerOriginal);
        EXPECT_GT(serializedData.size(), 0) << "Serialized data should not be empty.";

        PlayerStruct playerNew = static_cast<PlayerStruct>(RZSerializable<PlayerStruct>::deserializeFromBinary(serializedData));
        EXPECT_EQ(playerNew.health, playerOriginal.health);
        EXPECT_EQ(playerNew.rage, playerOriginal.rage);
        EXPECT_EQ(playerNew.stamina, playerOriginal.stamina);
        EXPECT_STREQ(playerNew.name, playerOriginal.name);
    }
}    // namespace Razix
