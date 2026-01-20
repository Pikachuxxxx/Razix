// Serialization.cpp
// Pikachuxxxx + AI-generated unit tests for the Type Registration classes
#include <Razix/Core/OS/RZFileSystem.h>
#include <Razix/Core/Reflection/RZReflection.h>
#include <Razix/Core/Serialization/RZSerializable.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <string>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace Razix {
    // Dummy struct for testing reflection and serialization
    struct PlayerStats
    {
        int    health;
        float  rage;
        double stamina;
        char   rank;
    };

    // Register the type
    RAZIX_REFLECT_TYPE_START(PlayerStats)
    RAZIX_REFLECT_PRIMITIVE(health)
    RAZIX_REFLECT_PRIMITIVE(rage)
    RAZIX_REFLECT_PRIMITIVE(stamina)
    RAZIX_REFLECT_TYPE_END(PlayerStats)

    struct PlayerMetaData
    {
        char* pName;
        int   level;
        float experience;
        //RZString description;
    };

    RAZIX_REFLECT_TYPE_START(PlayerMetaData)
    RAZIX_REFLECT_BLOB(pName, 64 * sizeof(char))
    RAZIX_REFLECT_PRIMITIVE(level)
    RAZIX_REFLECT_PRIMITIVE(experience)
    RAZIX_REFLECT_TYPE_END(PlayerMetaData)

    struct MasterPlayerStats
    {
        PlayerStats stats;
        int         score;
    };

    RAZIX_REFLECT_TYPE_START(MasterPlayerStats)
    //RAZIX_REFLECT_MEMBER(stats)
    RAZIX_REFLECT_PRIMITIVE(score)
    RAZIX_REFLECT_TYPE_END(MasterPlayerStats)

    // Fixture for Serialization Tests
    class RZSerializationTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            Debug::RZLog::StartUp();
        }

        void TearDown() override
        {
            Debug::RZLog::Shutdown();
        }
    };

    // Test: Type Registration
    TEST_F(RZSerializationTests, PODSerializationHandshake)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<PlayerStats>();
        ASSERT_NE(metaData, nullptr) << "Metadata for PlayerStats should not be null.";

        EXPECT_EQ(metaData->name, "PlayerStats");
        EXPECT_EQ(metaData->typeName, typeid(PlayerStats).name());
        EXPECT_EQ(metaData->size, sizeof(PlayerStats));

        // now reflection is done we can test serialization
        PlayerStats playerOriginal = {};
        playerOriginal.health      = 100;
        playerOriginal.rage        = 75.5f;
        playerOriginal.stamina     = 50.25;
        playerOriginal.rank        = 'A';

        auto serializedData = RZSerializable<PlayerStats>::serializeToBinary(playerOriginal);
        EXPECT_GT(serializedData.size(), 0) << "Serialized data should not be empty.";

        fs::path tempPath = fs::temp_directory_path() / "playerstats.bin";
        // Write binary data
        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        PlayerStats playerNew = static_cast<PlayerStats>(RZSerializable<PlayerStats>::deserializeFromBinary(readBack));
        EXPECT_EQ(playerNew.health, playerOriginal.health);
        EXPECT_EQ(playerNew.rage, playerOriginal.rage);
        EXPECT_EQ(playerNew.stamina, playerOriginal.stamina);
        EXPECT_EQ(playerNew.rank, playerOriginal.rank);

        fs::remove(tempPath);
    }

    //TEST_F(RZSerializationTests, BlobTest)
    //{
    //    const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<PlayerMetaData>();
    //    ASSERT_NE(metaData, nullptr) << "Metadata for PlayerMetaData should not be null.";

    //    PlayerMetaData original = {};
    //    original.level          = 42;
    //    original.experience     = 9999.0f;

    //    constexpr size_t BlobSize = 64;

    //    original.pName = static_cast<char*>(rz_malloc_aligned(BlobSize));
    //    ASSERT_NE(original.pName, nullptr);

    //    std::memset(original.pName, 0, BlobSize);
    //    const char* description =
    //        "Kratos! Ghost of Sparta. Slayer of gods. Anger issues included.";
    //    std::strncpy(original.pName, description, BlobSize - 1);

    //    auto serializedData =
    //        RZSerializable<PlayerMetaData>::serializeToBinary(original);

    //    EXPECT_GT(serializedData.size(), 0);
    //    rz_free(original.pName);

    //    PlayerMetaData deserialized =
    //        RZSerializable<PlayerMetaData>::deserializeFromBinary(serializedData);

    //    // assume serializer allocates memory for blobs and members
    //    ASSERT_NE(deserialized.pName, nullptr);

    //    EXPECT_STREQ(deserialized.pName, original.pName);
    //    EXPECT_EQ(deserialized.level, original.level);
    //    EXPECT_EQ(deserialized.experience, original.experience);

    //    rz_free(deserialized.pName);
    //}
}    // namespace Razix
