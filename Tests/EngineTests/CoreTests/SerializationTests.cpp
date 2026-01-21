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

    //-------------------------------------------------------------------------
    // Blob test struct
    struct PlayerMetaData
    {
        char* pName;
        int   level;
        float experience;
        //RZString description;
    };

    RAZIX_REFLECT_TYPE_START(PlayerMetaData)
    RAZIX_REFLECT_BLOB(pName, 128 * sizeof(char))
    RAZIX_REFLECT_PRIMITIVE(level)
    RAZIX_REFLECT_PRIMITIVE(experience)
    RAZIX_REFLECT_TYPE_END(PlayerMetaData)

    //-------------------------------------------------------------------------
    // Array test struct
    struct PlayerInventory
    {
        RZDynamicArray<int>    itemIDs;
        RZDynamicArray<float>  itemWeights;
        RZFixedArray<int, 100> weaponIDs;
    };

    RAZIX_REFLECT_TYPE_START(PlayerInventory)
    RAZIX_REFLECT_ARRAY(itemIDs)
    RAZIX_REFLECT_ARRAY(itemWeights)
    RAZIX_REFLECT_FIXED_ARRAY(weaponIDs)
    RAZIX_REFLECT_TYPE_END(PlayerInventory)

    //-------------------------------------------------------------------------
    // Nested struct test
    struct MasterPlayerStats
    {
        PlayerStats stats;
        int         score;
    };

    RAZIX_REFLECT_TYPE_START(MasterPlayerStats)
    //RAZIX_REFLECT_MEMBER(stats) // ?? Figure out how to reflect nested structs that (trivial and non-trivial types)
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

    TEST_F(RZSerializationTests, BlobTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<PlayerMetaData>();
        ASSERT_NE(metaData, nullptr) << "Metadata for PlayerMetaData should not be null.";

        PlayerMetaData original = {};
        original.level          = 42;
        original.experience     = 9999.0f;

        constexpr size_t BlobSize = 64;

        original.pName = static_cast<char*>(rz_malloc_aligned(BlobSize));
        ASSERT_NE(original.pName, nullptr);

        std::memset(original.pName, 0, BlobSize);
        const char* description =
            "Kratos! Ghost of Sparta. Slayer of gods. Anger issues included.";
        std::strncpy(original.pName, description, BlobSize - 1 /* strlen(description) */);

        auto serializedData = RZSerializable<PlayerMetaData>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "playermetadata.bin";
        RAZIX_CORE_INFO("Temporary path for BlobTest: {}", tempPath.string().c_str());
        // Write binary data
        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        PlayerMetaData deserialized = RZSerializable<PlayerMetaData>::deserializeFromBinary(readBack);

        // assume serializer allocates memory for blobs and members
        ASSERT_NE(deserialized.pName, nullptr);

        EXPECT_STREQ(deserialized.pName, original.pName);
        EXPECT_EQ(deserialized.level, original.level);
        EXPECT_EQ(deserialized.experience, original.experience);

        rz_free(original.pName);
        rz_free(deserialized.pName);
    }
    
    TEST_F(RZSerializationTests, ArrayTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<PlayerInventory>();
        ASSERT_NE(metaData, nullptr) << "Metadata for PlayerInventory should not be null.";

        PlayerInventory original = {};
        // Fill dynamic arrays
        for (int i = 0; i < 10; ++i) {
            original.itemIDs.push_back(i);
            original.itemWeights.push_back(i * 1.5f);
        }
        // Fill fixed array
        for (size_t i = 0; i < original.weaponIDs.capacity(); ++i) {
            original.weaponIDs[i] = static_cast<int>(i);
        }

        auto serializedData = RZSerializable<PlayerInventory>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "playerinventory.bin";
        RAZIX_CORE_INFO("Temporary path for ArrayTest: {}", tempPath.string().c_str());
        // Write binary data
        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        PlayerInventory deserialized = RZSerializable<PlayerInventory>::deserializeFromBinary(readBack);

        // Validate dynamic arrays
        ASSERT_EQ(deserialized.itemIDs.size(), original.itemIDs.size());
        ASSERT_EQ(deserialized.itemWeights.size(), original.itemWeights.size());
        for (size_t i = 0; i < original.itemIDs.size(); ++i) {
            EXPECT_EQ(deserialized.itemIDs[i], original.itemIDs[i]);
            EXPECT_FLOAT_EQ(deserialized.itemWeights[i], original.itemWeights[i]);
        }
        // Validate fixed array
        for (size_t i = 0; i < original.weaponIDs.size(); ++i) {
            EXPECT_EQ(deserialized.weaponIDs[i], original.weaponIDs[i]);
        }
    }

}    // namespace Razix
