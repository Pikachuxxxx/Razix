// Serialization.cpp
// Pikachuxxxx + AI-generated unit tests for the Type Registration classes

#include <Razix/Core/RZCore.h>

#include <Razix/Core/OS/RZFileSystem.h>
#include <Razix/Core/Reflection/RZReflection.h>
#include <Razix/Core/Serialization/RZSerializable.h>
#include <Razix/Core/UUID/RZUUID.h>

// Explicitly disable RHI profiling for these tests
#define RAZIX_RHI_EXT_PROFILER_DISABLE

#include <Razix/Asset/RZAnimationAsset.h>
#include <Razix/Asset/RZAssetRefAsset.h>
#include <Razix/Asset/RZAudioAsset.h>
#include <Razix/Asset/RZCameraAsset.h>
#include <Razix/Asset/RZClothAsset.h>
#include <Razix/Asset/RZGameDataAsset.h>
#include <Razix/Asset/RZLightAsset.h>
#include <Razix/Asset/RZLuaScriptAsset.h>
#include <Razix/Asset/RZMaterialAsset.h>
#include <Razix/Asset/RZMeshAsset.h>
#include <Razix/Asset/RZPhysicsMaterialAsset.h>
#include <Razix/Asset/RZTextureAsset.h>
#include <Razix/Asset/RZTransformAsset.h>
#include <Razix/Asset/RZVignerePuzzleAsset.h>

#include <Razix/Core/Utils/RZDateUtils.h>

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
        float4 complexData;
    };

    // Register the type
    RAZIX_REFLECT_TYPE_START(PlayerStats)
    RAZIX_REFLECT_PRIMITIVE(health)
    RAZIX_REFLECT_PRIMITIVE(rage)
    RAZIX_REFLECT_PRIMITIVE(stamina)
    RAZIX_REFLECT_PRIMITIVE(rank)
    RAZIX_REFLECT_PRIMITIVE(complexData)
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

#define PLAYER_BLOB_COUNT 5

    struct PlayerBlobTest
    {
        PlayerMetaData* pMetaDatas;
    };

    RAZIX_REFLECT_TYPE_START(PlayerBlobTest)
    RAZIX_REFLECT_BLOB(pMetaDatas, PLAYER_BLOB_COUNT * sizeof(PlayerMetaData))
    RAZIX_REFLECT_TYPE_END(PlayerBlobTest)

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
    // String test structs
    struct PlayerProfile
    {
        RZString playerName;
        RZString bio;
    };

    RAZIX_REFLECT_TYPE_START(PlayerProfile)
    RAZIX_REFLECT_STRING(playerName)
    RAZIX_REFLECT_STRING(bio)
    RAZIX_REFLECT_TYPE_END(PlayerProfile)

    //-------------------------------------------------------------------------
    // HashMap test struct

    struct PlayerSettings
    {
        RZHashMap<int, int> settings;
    };

    RAZIX_REFLECT_TYPE_START(PlayerSettings)
    RAZIX_REFLECT_HASHMAP(settings)
    RAZIX_REFLECT_TYPE_END(PlayerSettings)

    //-------------------------------------------------------------------------
    // Nested struct test
    struct MasterPlayerStats
    {
        PlayerStats stats;
        int         score;
    };

    RAZIX_REFLECT_TYPE_START(MasterPlayerStats)
    RAZIX_REFLECT_OBJECT(stats)    // ?? Figure out how to reflect nested structs that (trivial and non-trivial types)
    RAZIX_REFLECT_PRIMITIVE(score)
    RAZIX_REFLECT_TYPE_END(MasterPlayerStats)

    //-------------------------------------------------------------------------
    // UUID test struct
    struct PlayerIDs
    {
        RZUUID id;
    };

    RAZIX_REFLECT_TYPE_START(PlayerIDs)
    RAZIX_REFLECT_UUID(id)
    RAZIX_REFLECT_TYPE_END(PlayerIDs)

    //-------------------------------------------------------------------------
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
        playerOriginal.complexData = float4{1.0f, 2.0f, 3.0f, 4.0f};

        auto serializedData = RZSerializable<PlayerStats>::serializeToBinary(playerOriginal);
        EXPECT_GT(serializedData.size(), 0) << "Serialized data should not be empty.";

        fs::path tempPath = fs::temp_directory_path() / "playerstats.bin";
        RAZIX_CORE_INFO("Temporary path for PODSerializationHandshake: {}", tempPath.string().c_str());
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
    }

    TEST_F(RZSerializationTests, BlobTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<PlayerMetaData>();
        ASSERT_NE(metaData, nullptr) << "Metadata for PlayerMetaData should not be null.";

        PlayerMetaData original = {};
        original.level          = 42;
        original.experience     = 9999.0f;

        constexpr size_t BlobSize = 256 * sizeof(char);

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

    TEST_F(RZSerializationTests, BlobTypeTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<PlayerBlobTest>();
        ASSERT_NE(metaData, nullptr) << "Metadata for PlayerBlobTest should not be null.";
        PlayerBlobTest original = {};

        original.pMetaDatas = static_cast<PlayerMetaData*>(rz_malloc_aligned(sizeof(PlayerMetaData) * PLAYER_BLOB_COUNT));
        ASSERT_NE(original.pMetaDatas, nullptr);
        for (size_t i = 0; i < PLAYER_BLOB_COUNT; ++i) {
            original.pMetaDatas[i].level      = static_cast<int>(i * 10);
            original.pMetaDatas[i].experience = static_cast<float>(i * 1000);
            original.pMetaDatas[i].pName      = static_cast<char*>(rz_malloc_aligned(128 * sizeof(char)));
            ASSERT_NE(original.pMetaDatas[i].pName, nullptr);
            std::memset(original.pMetaDatas[i].pName, 0, 128 * sizeof(char));
            std::snprintf(original.pMetaDatas[i].pName, 128, "Player_%zu", i);
        }
        auto serializedData = RZSerializable<PlayerBlobTest>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);
        fs::path tempPath = fs::temp_directory_path() / "playerblobtest.bin";
        RAZIX_CORE_INFO("Temporary path for BlobTypeTest: {}", tempPath.string().c_str());
        // Write binary data
        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());
        RZDynamicArray<u8> readBack;
        i64                size = RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);
        PlayerBlobTest deserialized = RZSerializable<PlayerBlobTest>::deserializeFromBinary(readBack);

        for (size_t i = 0; i < PLAYER_BLOB_COUNT; ++i) {
            ASSERT_NE(deserialized.pMetaDatas[i].pName, nullptr);
            EXPECT_STREQ(deserialized.pMetaDatas[i].pName, original.pMetaDatas[i].pName);
            EXPECT_EQ(deserialized.pMetaDatas[i].level, original.pMetaDatas[i].level);
            EXPECT_EQ(deserialized.pMetaDatas[i].experience, original.pMetaDatas[i].experience);
            rz_free(original.pMetaDatas[i].pName);
            // FIXME: Who tf is allocating this? we should be able to free this properly
            //rz_free(deserialized.pMetaDatas[i].pName);
        }

        rz_free(original.pMetaDatas);
        rz_free(deserialized.pMetaDatas);
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
            original.weaponIDs.push_back(static_cast<int>(i));    // increase size
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

    TEST_F(RZSerializationTests, StringTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<PlayerProfile>();
        ASSERT_NE(metaData, nullptr) << "Metadata for PlayerProfile should not be null.";

        PlayerProfile original = {};
        original.playerName    = RZString("Jon Snow");
        original.bio           = RZString("A fearless warrior from the north.");

        auto serializedData = RZSerializable<PlayerProfile>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "playerprofile.bin";
        RAZIX_CORE_INFO("Temporary path for StringTest: {}", tempPath.string().c_str());
        // Write binary data
        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        PlayerProfile deserialized = RZSerializable<PlayerProfile>::deserializeFromBinary(readBack);

        EXPECT_STREQ(deserialized.playerName.c_str(), original.playerName.c_str());
        EXPECT_STREQ(deserialized.bio.c_str(), original.bio.c_str());
    }

    // Hashmap test
    TEST_F(RZSerializationTests, PrimitiveHashMapTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<PlayerSettings>();
        ASSERT_NE(metaData, nullptr) << "Metadata for PlayerSettings should not be null.";

        PlayerSettings original = {};
        for (int i = 1; i <= 20; ++i) {
            original.settings.insert(i, i * 100);
        }

        auto serializedData = RZSerializable<PlayerSettings>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "playersettings.bin";
        RAZIX_CORE_INFO("Temporary path for HashMapTest: {}", tempPath.string().c_str());
        // Write binary data
        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        PlayerSettings deserialized = RZSerializable<PlayerSettings>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.settings.size(), original.settings.size());
        for (const auto& [key, value]: original.settings) {
            auto it = deserialized.settings.find(key);
            ASSERT_NE(it, deserialized.settings.end());
            EXPECT_EQ(it->second, value);
        }
    }

    // Nested struct tests
    TEST_F(RZSerializationTests, TrivialNestedStructTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<MasterPlayerStats>();
        ASSERT_NE(metaData, nullptr) << "Metadata for MasterPlayerStats should not be null.";

        MasterPlayerStats original = {};
        original.stats.health      = 100;
        original.stats.rage        = 80.0f;
        original.stats.stamina     = 60.5;
        original.stats.rank        = 'A';
        original.score             = 99999;

        auto serializedData = RZSerializable<MasterPlayerStats>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "masterplayerstats.bin";
        RAZIX_CORE_INFO("Temporary path for NestedStructTest: {}", tempPath.string().c_str());
        // Write binary data
        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        MasterPlayerStats deserialized = RZSerializable<MasterPlayerStats>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.score, original.score);
        EXPECT_EQ(deserialized.stats.health, original.stats.health);
        EXPECT_FLOAT_EQ(deserialized.stats.rage, original.stats.rage);
        EXPECT_DOUBLE_EQ(deserialized.stats.stamina, original.stats.stamina);
        EXPECT_EQ(deserialized.stats.rank, original.stats.rank);
    }

    TEST_F(RZSerializationTests, UUIDTest)
    {
        RZUUID uuid;

        PlayerIDs original = {};
        original.id        = uuid;

        auto serializedData = RZSerializable<PlayerIDs>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "playerid.bin";
        RAZIX_CORE_INFO("Temporary path for NestedStructTest: {}", tempPath.string().c_str());
        // Write binary data
        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        PlayerIDs deserialized = RZSerializable<PlayerIDs>::deserializeFromBinary(readBack);

        EXPECT_EQ(original.id, deserialized.id) << "UUIDs changed across serialization";
    }

    //-------------------------------------------------------------------------

    class RZAssetHeaderSerializationTests : public ::testing::Test
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

    TEST_F(RZAssetHeaderSerializationTests, AssetHotDataTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<Razix::RZAssetHotData>();
        ASSERT_NE(metaData, nullptr) << "Metadata for RZAssetHotData should not be null.";

        RZAssetHotData original    = {};
        original.UUID              = RZUUID();
        original.referenceCount    = 42;
        original.type              = RZAssetType::kTexture;
        original.storagePreference = RZAssetStorageType::kGPUBacked;
        original.flags             = (RZAssetFlags) (RZ_ASSET_FLAG_COMPRESSED | RZ_ASSET_FLAG_READONLY);

        auto serializedData = RZSerializable<RZAssetHotData>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzassethotdata.bin";
        RAZIX_CORE_INFO("Temporary path for AssetHotDataTest: {}", tempPath.string().c_str());

        // Write binary data
        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());
        RZDynamicArray<u8> readBack;
        i64                size = RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);
        RZAssetHotData deserialized = RZSerializable<RZAssetHotData>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.UUID, original.UUID);
        EXPECT_EQ(deserialized.referenceCount, original.referenceCount);
        EXPECT_EQ(deserialized.type, original.type);
        EXPECT_EQ(deserialized.storagePreference, original.storagePreference);
        EXPECT_EQ(deserialized.flags, original.flags);
    }

    TEST_F(RZAssetHeaderSerializationTests, AssetMetadataTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<Razix::RZAssetMetadata>();
        ASSERT_NE(metaData, nullptr) << "Metadata for RZAssetMetadata should not be null.";

        Razix::RZAssetMetadata original = {};
        original.name                   = "GoldenCube";
        original.author                 = "Pikachuxxxx";
        original.description            = "A fancy asset";
        original.commitHash             = "deadbeefcafe";
        original.version.revisionID     = RZUUID();
        original.version.major          = 44;
        original.packlastModified       = rz_date_pack({20203, 12, 23});
        original.createdDate            = rz_date_pack({20203, 12, 23});
        original.department             = Razix::Department::Core;

        auto serializedData = RZSerializable<Razix::RZAssetMetadata>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzassetmetadata.bin";
        RAZIX_CORE_INFO("Temporary path for AssetMetadataTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());
        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        Razix::RZAssetMetadata deserialized = RZSerializable<Razix::RZAssetMetadata>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.name, original.name);
        EXPECT_EQ(deserialized.author, original.author);
        EXPECT_EQ(deserialized.description, original.description);
        EXPECT_EQ(deserialized.commitHash, original.commitHash);
        EXPECT_EQ(deserialized.version.revisionID, original.version.revisionID);
        EXPECT_EQ(deserialized.version.major, original.version.major);
        EXPECT_EQ(deserialized.packlastModified, original.packlastModified);
        EXPECT_EQ(deserialized.createdDate, original.createdDate);
        EXPECT_EQ(deserialized.department, original.department);
    }

    TEST_F(RZAssetHeaderSerializationTests, AssetDependencyTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<Razix::RZAssetDependecy>();
        ASSERT_NE(metaData, nullptr) << "Metadata for RZAssetDependecy should not be null.";

        Razix::RZAssetDependecy original = {};
        original.assetID                 = RZUUID();
        original.type                    = Razix::RZAssetType::kAnimation;

        auto serializedData = RZSerializable<Razix::RZAssetDependecy>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzassetdependency.bin";
        RAZIX_CORE_INFO("Temporary path for AssetDependencyTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());
        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        Razix::RZAssetDependecy deserialized = RZSerializable<Razix::RZAssetDependecy>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.assetID, original.assetID);
        EXPECT_EQ(deserialized.type, original.type);
    }

    TEST_F(RZAssetHeaderSerializationTests, AssetColdDataTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<Razix::RZAssetColdData>();
        ASSERT_NE(metaData, nullptr) << "Metadata for RZAssetColdData should not be null.";

        Razix::RZAssetColdData original = {};

        // Fill dependencies array
        Razix::RZAssetDependecy dep1 = {};
        dep1.assetID                 = RZUUID();
        dep1.type                    = Razix::RZAssetType::kLuaScript;
        Razix::RZAssetDependecy dep2 = {};
        dep2.assetID                 = RZUUID();
        dep2.type                    = Razix::RZAssetType::kMesh;
        original.dependencies.push_back(dep1);
        original.dependencies.push_back(dep2);

        // Fill metadata
        original.metadata.name               = "CoolAsset";
        original.metadata.author             = "Pikachuxxxx";
        original.metadata.description        = "A dependency test asset";
        original.metadata.commitHash         = "abcdef";
        original.metadata.version.revisionID = RZUUID();
        original.metadata.version.major      = 99;
        original.metadata.packlastModified   = rz_date_pack({20203, 12, 23});
        original.metadata.createdDate        = rz_date_pack({20203, 12, 23});
        original.metadata.department         = Razix::Department::Core;

        auto serializedData = RZSerializable<Razix::RZAssetColdData>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzassetcolddata.bin";
        RAZIX_CORE_INFO("Temporary path for AssetColdDataTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());
        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        Razix::RZAssetColdData deserialized = RZSerializable<Razix::RZAssetColdData>::deserializeFromBinary(readBack);

        // Check dependencies array
        EXPECT_EQ(deserialized.dependencies.size(), original.dependencies.size());
        for (size_t i = 0; i < original.dependencies.size(); ++i) {
            EXPECT_EQ(deserialized.dependencies[i].assetID, original.dependencies[i].assetID);
            EXPECT_EQ(deserialized.dependencies[i].type, original.dependencies[i].type);
        }
        // Check metadata fields
        EXPECT_EQ(deserialized.metadata.name, original.metadata.name);
        EXPECT_EQ(deserialized.metadata.author, original.metadata.author);
        EXPECT_EQ(deserialized.metadata.description, original.metadata.description);
        EXPECT_EQ(deserialized.metadata.commitHash, original.metadata.commitHash);
        EXPECT_EQ(deserialized.metadata.version.revisionID, original.metadata.version.revisionID);
        EXPECT_EQ(deserialized.metadata.version.major, original.metadata.version.major);
        EXPECT_EQ(deserialized.metadata.packlastModified, original.metadata.packlastModified);
        EXPECT_EQ(deserialized.metadata.createdDate, original.metadata.createdDate);
        EXPECT_EQ(deserialized.metadata.department, original.metadata.department);
    }

    TEST_F(RZAssetHeaderSerializationTests, AssetFullRoundtripTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<Razix::RZAsset>();
        ASSERT_NE(metaData, nullptr) << "Metadata for RZAsset should not be null.";

        void* mem      = rz_malloc(sizeof(RZAssetColdData), RAZIX_CACHE_LINE_ALIGN);
        auto* coldData = new (mem) Razix::RZAssetColdData();

        Razix::RZAssetMetadata meta = {};
        meta.name                   = "ComprehensiveAsset";
        meta.author                 = "Pikachuxxxx";
        meta.description            = "All fields fully filled";
        meta.commitHash             = "babe123deadbeef";
        meta.version.revisionID     = RZUUID();
        meta.version.major          = 123456789;
        meta.packlastModified       = rz_date_pack({20203, 12, 23});
        meta.createdDate            = rz_date_pack({20203, 12, 23});
        meta.department             = Razix::Department::Audio;

        Razix::RZAssetType assetType = Razix::RZAssetType::kCloth;
        Razix::RZAsset     asset(assetType, coldData);

        RZUUID uuid = RZUUID();
        asset.setUUID(uuid);

        asset.setType(assetType);
        asset.setStoragePreference(Razix::RZAssetStorageType::kGPUBacked);
        asset.setReferenceCount(67890ull);

        asset.setFlags(static_cast<Razix::RZAssetFlags>(0));
        asset.addFlags(Razix::RZ_ASSET_FLAG_COMPRESSED);
        asset.addFlags(Razix::RZ_ASSET_FLAG_READONLY);
        asset.addFlags(Razix::RZ_ASSET_FLAG_DIRTY);

        asset.setMetadata(meta);

        for (int i = 0; i < 3; ++i) {
            Razix::RZAssetDependecy dep = {};
            dep.assetID                 = RZUUID();
            dep.type                    = static_cast<Razix::RZAssetType>(((int) Razix::RZAssetType::kLuaScript + i) % (int) Razix::RZAssetType::COUNT);
            asset.addDependency(dep.type, dep.assetID);
        }

        auto serializedData = RZSerializable<Razix::RZAsset>::serializeToBinary(asset);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzasset.bin";
        RAZIX_CORE_INFO("Temporary path for AssetFullRoundtripTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);

        void*           assetMemoryBacking   = rz_malloc(sizeof(Razix::RZAsset), RAZIX_CACHE_LINE_ALIGN);
        void*           assetColdDataBacking = rz_malloc(sizeof(Razix::RZAssetColdData), RAZIX_CACHE_LINE_ALIGN);
        Razix::RZAsset* pDeserialized        = (RZAsset*) RZSerializable<Razix::RZAsset>::deserializeAssetFromBinary(readBack, assetMemoryBacking, assetColdDataBacking);

        EXPECT_EQ(pDeserialized->getUUID(), uuid);
        EXPECT_EQ(pDeserialized->getType(), assetType);
        EXPECT_EQ(pDeserialized->getStoragePreference(), Razix::RZAssetStorageType::kGPUBacked);
        EXPECT_EQ(pDeserialized->getReferenceCount(), 67890ull);

        Razix::RZAssetFlags expectedFlags = static_cast<Razix::RZAssetFlags>(
            Razix::RZ_ASSET_FLAG_COMPRESSED | Razix::RZ_ASSET_FLAG_READONLY | Razix::RZ_ASSET_FLAG_DIRTY);
        EXPECT_EQ(pDeserialized->getFlags(), expectedFlags);

        EXPECT_TRUE(pDeserialized->isCompressed());
        EXPECT_TRUE(pDeserialized->isReadOnly());
        EXPECT_TRUE(pDeserialized->isDirty());

        const auto& dMeta = pDeserialized->getMetadata();
        EXPECT_EQ(dMeta.name, meta.name);
        EXPECT_EQ(dMeta.author, meta.author);
        EXPECT_EQ(dMeta.description, meta.description);
        EXPECT_EQ(dMeta.commitHash, meta.commitHash);
        EXPECT_EQ(dMeta.version.revisionID, meta.version.revisionID);
        EXPECT_EQ(dMeta.version.major, meta.version.major);
        EXPECT_EQ(dMeta.packlastModified, meta.packlastModified);
        EXPECT_EQ(dMeta.createdDate, meta.createdDate);
        EXPECT_EQ(dMeta.department, meta.department);

        const auto& deps = pDeserialized->getDependencies();
        ASSERT_EQ(deps.size(), coldData->dependencies.size());
        for (size_t i = 0; i < deps.size(); ++i) {
            EXPECT_EQ(deps[i].assetID, coldData->dependencies[i].assetID);
            EXPECT_EQ(deps[i].type, coldData->dependencies[i].type);
            for (int j = 0; j < 12; ++j)
                EXPECT_EQ(deps[i]._pad0[j], coldData->dependencies[i]._pad0[j]);
        }
        rz_free(coldData);
        pDeserialized->destroy();
        // Don't do this, double-free, just de-allocate the asset mem backing 
        // rz_free(pDeserialized);
        rz_free(assetColdDataBacking);
        rz_free(assetMemoryBacking);
    }

    //-------------------------------------------------------------------------

    class RZAssetPayloadSerializationTests : public ::testing::Test
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

    TEST_F(RZAssetPayloadSerializationTests, AnimationAssetSerializationTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZAnimationAsset>();
        ASSERT_NE(metaData, nullptr);

        RZAnimationAsset original    = {};
        original.FrameCount          = 60;
        original.BoneCount           = 42;
        original.Duration            = 3.5f;
        original.FrameRate           = 24.0f;
        original.SkeletonHandle      = rz_handle{1234};
        original.AnimationDataHandle = rz_handle{5678};
        original.bShouldLoop         = true;

        auto serializedData =
            RZSerializable<RZAnimationAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzanimationasset.bin";
        RAZIX_CORE_INFO("Temporary path for AnimationAssetSerializationTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZAnimationAsset deserialized =
            RZSerializable<RZAnimationAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.FrameCount, original.FrameCount);
        EXPECT_EQ(deserialized.BoneCount, original.BoneCount);
        EXPECT_FLOAT_EQ(deserialized.Duration, original.Duration);
        EXPECT_FLOAT_EQ(deserialized.FrameRate, original.FrameRate);
        EXPECT_EQ(deserialized.SkeletonHandle, original.SkeletonHandle);
        EXPECT_EQ(deserialized.AnimationDataHandle, original.AnimationDataHandle);
        EXPECT_EQ(deserialized.bShouldLoop, original.bShouldLoop);
    }

    // Add tests for other asset types
    TEST_F(RZAssetPayloadSerializationTests, AssetRefTest)
    {
        const TypeMetaData* metaData = RZTypeRegistry::getTypeMetaData<Razix::RZAssetRefAsset>();
        ASSERT_NE(metaData, nullptr) << "Metadata for RZAssetRefAsset should not be null.";

        RZAssetRefAsset original = {};
        original.AssetUUID       = RZUUID();
        auto serializedData      = RZSerializable<RZAssetRefAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzassetrefasset.bin";
        RAZIX_CORE_INFO("Temporary path for AssetRefTest: {}", tempPath.string().c_str());
        // Write binary data
        Razix::RZFileSystem::WriteFile(RZString(tempPath.string().c_str()), serializedData.data(), serializedData.size());
        RZDynamicArray<u8> readBack;
        i64                size = RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);
        Razix::RZFileSystem::ReadFile(tempPath.string().c_str(), readBack.data(), size);
        RZAssetRefAsset deserialized = RZSerializable<RZAssetRefAsset>::deserializeFromBinary(readBack);
        EXPECT_TRUE(deserialized.AssetUUID == original.AssetUUID) << "Equality operator failed for equivalent AssetRef UUIDs.";
    }

    // RZAudioAsset test
    TEST_F(RZAssetPayloadSerializationTests, AudioAssetTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZAudioAsset>();
        ASSERT_NE(metaData, nullptr);

        RZAudioAsset original  = {};
        original.AudioFilePath = RZString("Assets/Audio/BackgroundMusic.rzaudio");
        original.Duration      = 120.0f;
        original.SampleRate    = 44100;
        original.ChannelCount  = 2;
        original.SampleCount   = 5292000;
        original.Type          = RZ_AUDIO_TYPE_MUSIC;
        original.bIsLooping    = true;

        auto serializedData =
            RZSerializable<RZAudioAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzaudioasset.bin";
        RAZIX_CORE_INFO("Temporary path for AudioAssetTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZAudioAsset deserialized =
            RZSerializable<RZAudioAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.AudioFilePath, original.AudioFilePath);
        EXPECT_FLOAT_EQ(deserialized.Duration, original.Duration);
        EXPECT_EQ(deserialized.SampleRate, original.SampleRate);
        EXPECT_EQ(deserialized.ChannelCount, original.ChannelCount);
        EXPECT_EQ(deserialized.SampleCount, original.SampleCount);
        EXPECT_EQ(deserialized.Type, original.Type);
        EXPECT_EQ(deserialized.bIsLooping, original.bIsLooping);
    }

    // RZCameraAsset Test
    TEST_F(RZAssetPayloadSerializationTests, CameraAssetTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZCameraAsset>();
        ASSERT_NE(metaData, nullptr);

        RZCameraAsset original = {};
        original.Position      = float3{1.0f, 2.0f, 3.0f};
        original.Front         = float3{0.0f, 0.0f, -1.0f};
        original.Up            = float3{0.0f, 1.0f, 0.0f};
        original.Right         = float3{1.0f, 0.0f, 0.0f};
        original.WorldUp       = float3{0.0f, 1.0f, 0.0f};

        original.Yaw   = -90.0f;
        original.Pitch = -10.0f;

        original.MovementSpeed    = 6.5f;
        original.MouseSensitivity = 0.12f;
        original.Zoom             = 45.0f;
        original.DampingFactor    = 0.85f;

        original.AspectRatio     = 16.0f / 9.0f;
        original.PerspectiveFOV  = 60.0f;
        original.PerspectiveNear = 0.1f;
        original.PerspectiveFar  = 1000.0f;

        original.OrthographicSize = 10.0f;
        original.OrthographicNear = -10.0f;
        original.OrthographicFar  = 10.0f;

        original.ProjectionType =
            RZCamera3D::ProjectionType::kPerspective;

        auto serializedData =
            RZSerializable<RZCameraAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzcameraasset.bin";
        RAZIX_CORE_INFO("Temporary path for CameraAssetTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZCameraAsset deserialized =
            RZSerializable<RZCameraAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.Position, original.Position);
        EXPECT_EQ(deserialized.Front, original.Front);
        EXPECT_EQ(deserialized.Up, original.Up);
        EXPECT_EQ(deserialized.Right, original.Right);
        EXPECT_EQ(deserialized.WorldUp, original.WorldUp);

        EXPECT_FLOAT_EQ(deserialized.Yaw, original.Yaw);
        EXPECT_FLOAT_EQ(deserialized.Pitch, original.Pitch);

        EXPECT_FLOAT_EQ(deserialized.MovementSpeed, original.MovementSpeed);
        EXPECT_FLOAT_EQ(deserialized.MouseSensitivity, original.MouseSensitivity);
        EXPECT_FLOAT_EQ(deserialized.Zoom, original.Zoom);
        EXPECT_FLOAT_EQ(deserialized.DampingFactor, original.DampingFactor);

        EXPECT_FLOAT_EQ(deserialized.AspectRatio, original.AspectRatio);
        EXPECT_FLOAT_EQ(deserialized.PerspectiveFOV, original.PerspectiveFOV);
        EXPECT_FLOAT_EQ(deserialized.PerspectiveNear, original.PerspectiveNear);
        EXPECT_FLOAT_EQ(deserialized.PerspectiveFar, original.PerspectiveFar);

        EXPECT_FLOAT_EQ(deserialized.OrthographicSize, original.OrthographicSize);
        EXPECT_FLOAT_EQ(deserialized.OrthographicNear, original.OrthographicNear);
        EXPECT_FLOAT_EQ(deserialized.OrthographicFar, original.OrthographicFar);

        EXPECT_EQ(deserialized.ProjectionType, original.ProjectionType);
    }

    TEST_F(RZAssetPayloadSerializationTests, ClothAssetSerializationTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZClothAsset>();
        ASSERT_NE(metaData, nullptr);

        RZClothAsset original    = {};
        original.VertexCount     = 1024;
        original.TriangleCount   = 2048;
        original.ConstraintCount = 4096;
        original.Mass            = 2.5f;
        original.Damping         = 0.15f;
        original.GravityScale    = 1.0f;
        original.bIsSimulating   = true;

        auto serializedData =
            RZSerializable<RZClothAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzclothasset.bin";
        RAZIX_CORE_INFO("Temporary path for ClothAssetSerializationTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZClothAsset deserialized =
            RZSerializable<RZClothAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.VertexCount, original.VertexCount);
        EXPECT_EQ(deserialized.TriangleCount, original.TriangleCount);
        EXPECT_EQ(deserialized.ConstraintCount, original.ConstraintCount);
        EXPECT_FLOAT_EQ(deserialized.Mass, original.Mass);
        EXPECT_FLOAT_EQ(deserialized.Damping, original.Damping);
        EXPECT_FLOAT_EQ(deserialized.GravityScale, original.GravityScale);
        EXPECT_EQ(deserialized.bIsSimulating, original.bIsSimulating);
    }

    TEST_F(RZAssetPayloadSerializationTests, GameDataAssetSerializationTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZGameDataAsset>();
        ASSERT_NE(metaData, nullptr);

        RZGameDataAsset original    = {};
        original.SaveSlotCount      = 3;
        original.TotalPlayTime      = 7260;
        original.DifficultyLevel    = 2;
        original.bIsDLCEnabled      = true;
        original.CheckpointPosition = float3{12.5f, 0.0f, -7.25f};
        original.LastPlayedLevel    = RZString("Levels/DesertOutpost.rzlevel");

        auto serializedData =
            RZSerializable<RZGameDataAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzgamedataasset.bin";
        RAZIX_CORE_INFO("Temporary path for GameDataAssetSerializationTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZGameDataAsset deserialized =
            RZSerializable<RZGameDataAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.SaveSlotCount, original.SaveSlotCount);
        EXPECT_EQ(deserialized.TotalPlayTime, original.TotalPlayTime);
        EXPECT_EQ(deserialized.DifficultyLevel, original.DifficultyLevel);
        EXPECT_EQ(deserialized.bIsDLCEnabled, original.bIsDLCEnabled);
        EXPECT_EQ(deserialized.CheckpointPosition, original.CheckpointPosition);
        EXPECT_EQ(deserialized.LastPlayedLevel, original.LastPlayedLevel);
    }

    TEST_F(RZAssetPayloadSerializationTests, LightAssetSerializationTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZLightAsset>();
        ASSERT_NE(metaData, nullptr);

        RZLightAsset original        = {};
        original.Position_Range      = float4{0.0f, 10.0f, 0.0f, 25.0f};
        original.Color_Intensity     = float4{1.0f, 0.9f, 0.8f, 5.0f};
        original.Direction           = float4{0.0f, -1.0f, 0.0f, 0.0f};
        original.ConstantAttenuation = 1.0f;
        original.LinearAttenuation   = 0.09f;
        original.Quadratic           = 0.032f;
        original.InnerConeAngle      = 15.0f;
        original.OuterConeAngle      = 30.0f;
        original.Type                = RZ_LIGHT_TYPE_SPOT;

        auto serializedData =
            RZSerializable<RZLightAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzlightasset.bin";
        RAZIX_CORE_INFO("Temporary path for LightAssetSerializationTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZLightAsset deserialized =
            RZSerializable<RZLightAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.Position_Range, original.Position_Range);
        EXPECT_EQ(deserialized.Color_Intensity, original.Color_Intensity);
        EXPECT_EQ(deserialized.Direction, original.Direction);
        EXPECT_FLOAT_EQ(deserialized.ConstantAttenuation, original.ConstantAttenuation);
        EXPECT_FLOAT_EQ(deserialized.LinearAttenuation, original.LinearAttenuation);
        EXPECT_FLOAT_EQ(deserialized.Quadratic, original.Quadratic);
        EXPECT_FLOAT_EQ(deserialized.InnerConeAngle, original.InnerConeAngle);
        EXPECT_FLOAT_EQ(deserialized.OuterConeAngle, original.OuterConeAngle);
        EXPECT_EQ(deserialized.Type, original.Type);
    }

    TEST_F(RZAssetPayloadSerializationTests, LuaScriptAssetSerializationTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZLuaScriptAsset>();
        ASSERT_NE(metaData, nullptr);

        RZLuaScriptAsset original = {};
        original.ScriptPath       = RZString("Scripts/Gameplay/PlayerController.lua");
        original.LastModifiedTime = 1712345678ull;
        original.bIsCompiled      = true;
        original.bAutoReload      = false;

        auto serializedData =
            RZSerializable<RZLuaScriptAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzluascriptasset.bin";
        RAZIX_CORE_INFO("Temporary path for LuaScriptAssetSerializationTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZLuaScriptAsset deserialized =
            RZSerializable<RZLuaScriptAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.ScriptPath, original.ScriptPath);
        EXPECT_EQ(deserialized.LastModifiedTime, original.LastModifiedTime);
        EXPECT_EQ(deserialized.bIsCompiled, original.bIsCompiled);
        EXPECT_EQ(deserialized.bAutoReload, original.bAutoReload);
    }

    TEST_F(RZAssetPayloadSerializationTests, MaterialAssetSerializationTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZMaterialAsset>();
        ASSERT_NE(metaData, nullptr);

        RZMaterialAsset original   = {};
        original.Albedo            = float4{0.8f, 0.7f, 0.6f, 1.0f};
        original.Metallic          = 0.9f;
        original.Roughness         = 0.25f;
        original.AmbientOcclusion  = 1.0f;
        original.NormalStrength    = 1.2f;
        original.EmissiveIntensity = 3.5f;

        original.AlbedoTextureHandle           = rz_handle{101};
        original.NormalTextureHandle           = rz_handle{102};
        original.MetallicTextureHandle         = rz_handle{103};
        original.RoughnessTextureHandle        = rz_handle{104};
        original.AmbientOcclusionTextureHandle = rz_handle{105};
        original.EmissiveTextureHandle         = rz_handle{106};

        original.EmissiveColor = float3{1.0f, 0.2f, 0.1f};

        original.TexturePaths.AlbedoPath           = RZString("Textures/Albedo.png");
        original.TexturePaths.NormalPath           = RZString("Textures/Normal.png");
        original.TexturePaths.MetallicPath         = RZString("Textures/Metallic.png");
        original.TexturePaths.RoughnessPath        = RZString("Textures/Roughness.png");
        original.TexturePaths.AmbientOcclusionPath = RZString("Textures/AO.png");
        original.TexturePaths.EmissivePath         = RZString("Textures/Emissive.png");

        auto serializedData =
            RZSerializable<RZMaterialAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzmaterialasset.bin";
        RAZIX_CORE_INFO("Temporary path for MaterialAssetSerializationTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZMaterialAsset deserialized =
            RZSerializable<RZMaterialAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.Albedo, original.Albedo);
        EXPECT_FLOAT_EQ(deserialized.Metallic, original.Metallic);
        EXPECT_FLOAT_EQ(deserialized.Roughness, original.Roughness);
        EXPECT_FLOAT_EQ(deserialized.AmbientOcclusion, original.AmbientOcclusion);
        EXPECT_FLOAT_EQ(deserialized.NormalStrength, original.NormalStrength);
        EXPECT_FLOAT_EQ(deserialized.EmissiveIntensity, original.EmissiveIntensity);
        EXPECT_EQ(deserialized.EmissiveColor, original.EmissiveColor);
        EXPECT_EQ(deserialized.TexturePaths.AlbedoPath, original.TexturePaths.AlbedoPath);
        EXPECT_EQ(deserialized.TexturePaths.NormalPath, original.TexturePaths.NormalPath);
        EXPECT_EQ(deserialized.TexturePaths.MetallicPath, original.TexturePaths.MetallicPath);
        EXPECT_EQ(deserialized.TexturePaths.RoughnessPath, original.TexturePaths.RoughnessPath);
        EXPECT_EQ(deserialized.TexturePaths.AmbientOcclusionPath, original.TexturePaths.AmbientOcclusionPath);
        EXPECT_EQ(deserialized.TexturePaths.EmissivePath, original.TexturePaths.EmissivePath);
    }

    TEST_F(RZAssetPayloadSerializationTests, MeshAssetSerializationTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZMeshAsset>();
        ASSERT_NE(metaData, nullptr);

        RZMeshAsset original        = {};
        original.VertexCount        = 24576;
        original.IndexCount         = 49152;
        original.VertexBufferHandle = 301;
        original.IndexBufferHandle  = 302;

        original.BoundsMin    = float3{-1.0f, -1.0f, -1.0f};
        original.BoundsMax    = float3{1.0f, 1.0f, 1.0f};
        original.BoundsRadius = 1.732f;

        original.MeshPath     = RZString("Meshes/Environment/Crate.rzmesh");
        original.MaterialPath = RZString("Materials/Crate.rzmat");

        auto serializedData =
            RZSerializable<RZMeshAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzmeshasset.bin";
        RAZIX_CORE_INFO("Temporary path for MeshAssetSerializationTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZMeshAsset deserialized =
            RZSerializable<RZMeshAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.VertexCount, original.VertexCount);
        EXPECT_EQ(deserialized.IndexCount, original.IndexCount);
        EXPECT_EQ(deserialized.VertexBufferHandle, original.VertexBufferHandle);
        EXPECT_EQ(deserialized.IndexBufferHandle, original.IndexBufferHandle);

        EXPECT_EQ(deserialized.BoundsMin, original.BoundsMin);
        EXPECT_EQ(deserialized.BoundsMax, original.BoundsMax);
        EXPECT_FLOAT_EQ(deserialized.BoundsRadius, original.BoundsRadius);

        EXPECT_EQ(deserialized.MeshPath, original.MeshPath);
        EXPECT_EQ(deserialized.MaterialPath, original.MaterialPath);
    }

    TEST_F(RZAssetPayloadSerializationTests, PhysicsMaterialAssetSerializationTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZPhysicsMaterialAsset>();
        ASSERT_NE(metaData, nullptr);

        RZPhysicsMaterialAsset original = {};
        original.Density                = 1000.0f;
        original.StaticFriction         = 0.8f;
        original.DynamicFriction        = 0.6f;
        original.Restitution            = 0.2f;
        original.LinearDamping          = 0.05f;
        original.AngularDamping         = 0.1f;
        original.bIsTrigger             = false;

        auto serializedData =
            RZSerializable<RZPhysicsMaterialAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzphysicsmaterialasset.bin";
        RAZIX_CORE_INFO("Temporary path for PhysicsMaterialAssetSerializationTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZPhysicsMaterialAsset deserialized =
            RZSerializable<RZPhysicsMaterialAsset>::deserializeFromBinary(readBack);

        EXPECT_FLOAT_EQ(deserialized.Density, original.Density);
        EXPECT_FLOAT_EQ(deserialized.StaticFriction, original.StaticFriction);
        EXPECT_FLOAT_EQ(deserialized.DynamicFriction, original.DynamicFriction);
        EXPECT_FLOAT_EQ(deserialized.Restitution, original.Restitution);
        EXPECT_FLOAT_EQ(deserialized.LinearDamping, original.LinearDamping);
        EXPECT_FLOAT_EQ(deserialized.AngularDamping, original.AngularDamping);
        EXPECT_EQ(deserialized.bIsTrigger, original.bIsTrigger);
    }

    TEST_F(RZAssetPayloadSerializationTests, TextureAssetSerializationTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZTextureAsset>();
        ASSERT_NE(metaData, nullptr);

        RZTextureAsset original = {};
        original.TexturePath    = RZString("Textures/UI/HUD_Diffuse.dds");
        original.TextureHandle  = rz_handle{777};

        original.Desc.width         = 1024;
        original.Desc.height        = 1024;
        original.Desc.depth         = 1;
        original.Desc.mipLevels     = 10;
        original.Desc.format        = RZ_GFX_FORMAT_B8G8R8A8_UNORM;    // whatever enum/value your gfx layer uses
        original.Desc.textureType   = RZ_GFX_TEXTURE_TYPE_2D;
        original.Desc.resourceHints = RZ_GFX_RESOURCE_VIEW_FLAG_NONE;

        auto serializedData =
            RZSerializable<RZTextureAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rztextureasset.bin";
        RAZIX_CORE_INFO("Temporary path for TextureAssetSerializationTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZTextureAsset deserialized =
            RZSerializable<RZTextureAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.TexturePath, original.TexturePath);
        EXPECT_EQ(deserialized.TextureHandle, original.TextureHandle);

        EXPECT_EQ(deserialized.Desc.width, original.Desc.width);
        EXPECT_EQ(deserialized.Desc.height, original.Desc.height);
        EXPECT_EQ(deserialized.Desc.depth, original.Desc.depth);
        EXPECT_EQ(deserialized.Desc.mipLevels, original.Desc.mipLevels);
        EXPECT_EQ(deserialized.Desc.format, original.Desc.format);
        EXPECT_EQ(deserialized.Desc.textureType, original.Desc.textureType);
        EXPECT_EQ(deserialized.Desc.resourceHints, original.Desc.resourceHints);
    }

    TEST_F(RZAssetPayloadSerializationTests, VigenerePuzzleAssetSerializationTest)
    {
        const TypeMetaData* metaData =
            RZTypeRegistry::getTypeMetaData<Razix::RZVignerePuzzleAsset>();
        ASSERT_NE(metaData, nullptr);

        RZVignerePuzzleAsset original = {};
        original.PlainText            = RZString("THE EAGLE FLIES AT MIDNIGHT");
        original.Key                  = RZString("RAZIX");
        original.bIsSolved            = false;

        auto serializedData =
            RZSerializable<RZVignerePuzzleAsset>::serializeToBinary(original);
        EXPECT_GT(serializedData.size(), 0);

        fs::path tempPath = fs::temp_directory_path() / "rzvigenerepuzzleasset.bin";
        RAZIX_CORE_INFO("Temporary path for VigenerePuzzleAssetSerializationTest: {}", tempPath.string().c_str());

        Razix::RZFileSystem::WriteFile(
            RZString(tempPath.string().c_str()),
            serializedData.data(),
            serializedData.size());

        RZDynamicArray<u8> readBack;
        i64                size = Razix::RZFileSystem::GetFileSize(tempPath.string().c_str());
        readBack.resize(size);

        Razix::RZFileSystem::ReadFile(
            tempPath.string().c_str(),
            readBack.data(),
            size);

        RZVignerePuzzleAsset deserialized =
            RZSerializable<RZVignerePuzzleAsset>::deserializeFromBinary(readBack);

        EXPECT_EQ(deserialized.PlainText, original.PlainText);
        EXPECT_EQ(deserialized.Key, original.Key);
        EXPECT_EQ(deserialized.bIsSolved, original.bIsSolved);
    }
}    // namespace Razix
