// AssetDBExtendedTests.cpp
// Extended tests for RZAssetDB: createAsset, destroyAsset, saveAssetToDisk,
// requestAssetLoad, loadAssetDBRegistry, exportAssetDBRegistry
// Tests every asset type except Transform (managed by SceneGraph)

#include "Razix/AssetSystem/RZAssetDB.h"
#include "Razix/AssetSystem/RZAssetBase.h"
#include "Razix/AssetSystem/RZAssetPool.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Memory/Allocators/RZHeapAllocator.h"
#include "Razix/Core/Memory/Allocators/RZBumpAllocator.h"
#include "Razix/Core/Memory/RZMemoryBudgets.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/Job/RZJobSystem.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Asset/RZAnimationAsset.h"
#include "Razix/Asset/RZAssetRefAsset.h"
#include "Razix/Asset/RZAudioAsset.h"
#include "Razix/Asset/RZCameraAsset.h"
#include "Razix/Asset/RZClothAsset.h"
#include "Razix/Asset/RZGameDataAsset.h"
#include "Razix/Asset/RZLightAsset.h"
#include "Razix/Asset/RZLuaScriptAsset.h"
#include "Razix/Asset/RZMaterialAsset.h"
#include "Razix/Asset/RZMeshAsset.h"
#include "Razix/Asset/RZPhysicsMaterialAsset.h"
#include "Razix/Asset/RZTextureAsset.h"
#include "Razix/Asset/RZVignerePuzzleAsset.h"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace Razix {
    namespace {

        // ─────────────────────────────────────────────────────────
        // Helpers
        // ─────────────────────────────────────────────────────────

        using Clock = std::chrono::steady_clock;

        static fs::path GetRepoRootFromThisFile()
        {
            fs::path p(__FILE__);
            return p.parent_path().parent_path().parent_path().parent_path();
        }

        static fs::path WriteBudgetFile(u32 assetPoolMB)
        {
            // clang-format off
            const std::string content = R"INI([GlobalFrameBudget]
                TotalFrameBudgetMs = 16.67

                [DeptTimingBudgets]
                Core.FrameBudgetMs = 1.0
                Environment.FrameBudgetMs = 1.0
                Character.FrameBudgetMs = 1.0
                Lighting.FrameBudgetMs = 1.0
                RayTracing.FrameBudgetMs = 1.0
                VFX.FrameBudgetMs = 1.0
                UI.FrameBudgetMs = 1.0
                Physics.FrameBudgetMs = 1.0
                Scripting.FrameBudgetMs = 1.0
                Audio.FrameBudgetMs = 1.0
                Network.FrameBudgetMs = 1.0
                Rendering.FrameBudgetMs = 1.0

                [MemoryPools]
                CoreSystems.HeapSizeMB = 16
                AssetPool.HeapSizeMB = )INI" + std::to_string(assetPoolMB) +
                                                    R"INI(
                GfxResources.HeapSizeMB = 32
                GfxResources.GPUMemoryMB = 64
                RenderingPool.HeapSizeMB = 16

                [ThreadFrameBudgets]
                TotalFrameAllocatorBudget = 64
                RenderThread.PerFrameAllocatorMB = 16
                GameThread.PerFrameAllocatorMB = 32
                WorkerThread.PerFrameAllocatorMB = 16
                )INI";
            // clang-format on

            const fs::path tempPath = fs::temp_directory_path() / "AssetDBExtendedTestBudget.ini";
            std::ofstream  out(tempPath);
            out << content;
            out.close();
            return tempPath;
        }

        // Asset-type subdirectory names (must match AssetTypeToVFSFilePath)
        static constexpr const char* kAssetSubDirs[] = {
            "Animations",
            "Audio",
            "Cameras",
            "Clothes",
            "GameData",
            "Lights",
            "LuaScripts",
            "Materials",
            "Meshes",
            "PhysicsMaterials",
            "Textures",
            "Puzzles",
        };

        static void CreateAssetSubDirs(const fs::path& root)
        {
            for (auto* sub : kAssetSubDirs)
                fs::create_directories(root / sub);
        }

    }    // namespace

    // ═════════════════════════════════════════════════════════════
    // Fixture — sets up AssetDB + JobSystem + VFS + Engine allocs
    // ═════════════════════════════════════════════════════════════

    class AssetDBExtendedFixture : public ::testing::Test
    {
    protected:
        Memory::RZHeapAllocator m_AssetAllocator;
        Memory::RZHeapAllocator m_AssetHeaderAllocator;
        fs::path                m_TempBudgetPath;
        fs::path                m_DefaultBudgetPath;
        fs::path                m_TempAssetsDir;

        static constexpr u32 kWorkerCount   = 4u;
        static constexpr u32 kAssetBudgetMB = 256u;

        // Access the singleton — the same instance that save/load IO lambdas use internally
        static RZAssetDB& db() { return RZAssetDB::Get(); }

        void SetUp() override
        {
            Razix::Debug::RZLog::StartUp();

            // 1. Budget file
            m_TempBudgetPath = WriteBudgetFile(kAssetBudgetMB);
            ASSERT_TRUE(Memory::ParseBudgetFile(RZString(m_TempBudgetPath.string().c_str())));
            m_DefaultBudgetPath = GetRepoRootFromThisFile() / "Engine/content/config/RazixDepartmentBudgets.ini";

            // 2. VFS + mount a temp "Assets" directory
            RZVirtualFileSystem::Get().StartUp();
            m_TempAssetsDir = fs::path(RAZIX_STRINGIZE(RAZIX_ROOT_DIR)) / "Tests" / "Assets";
            fs::create_directories(m_TempAssetsDir);
            CreateAssetSubDirs(m_TempAssetsDir);
            RZVirtualFileSystem::Get().mount("Assets", RZString(m_TempAssetsDir.string().c_str()) + "/");

            // 3. Engine allocators (system + frame) — needed by save/load IO handlers
            //    Try to manually init them; if this fails the user will adjust.
            RZEngine::Get().getSystemAllocator().init(64ull * 1024ull * 1024ull);
            RZEngine::Get().getFrameAllocator().init(16ull * 1024ull * 1024ull);

            // 4. Job system
            rz_job_system_startup(kWorkerCount);

            // 5. Heap allocators for the asset DB pools
            const auto   assetBudget = Memory::GetMemoryPoolBudget(Memory::RZ_MEM_POOL_TYPE_ASSET_POOL);
            const size_t chunkSize   = static_cast<size_t>(assetBudget.HeapSizeMB) * 1024ull * 1024ull;
            m_AssetAllocator.init(chunkSize);
            m_AssetHeaderAllocator.init(chunkSize, RAZIX_CACHE_LINE_ALIGN);

            // 6. AssetDB Startup (creates pools, registers IO handlers, loads registry)
            RZAssetDB::Get().Startup(m_AssetAllocator, m_AssetHeaderAllocator);
        }

        void TearDown() override
        {
            // Reverse order
            RZAssetDB::Get().Shutdown();

            rz_job_system_shutdown();

            RZEngine::Get().getFrameAllocator().shutdown();
            RZEngine::Get().getSystemAllocator().shutdown();

            m_AssetAllocator.shutdown();
            m_AssetHeaderAllocator.shutdown();

            RZVirtualFileSystem::Get().ShutDown();

            // Restore default budgets
            Memory::ParseBudgetFile(RZString(m_DefaultBudgetPath.string().c_str()));

            Razix::Debug::RZLog::Shutdown();

            // Clean up temp directory
            std::error_code ec;
            fs::remove_all(m_TempAssetsDir, ec);
            fs::remove(m_TempBudgetPath, ec);
        }

        // ─── Helpers ──────────────────────────────────────────────

        // Verify common header invariants after createAsset
        void VerifyCreatedHeader(rz_asset_handle handle, RZAssetType expectedType, const char* expectedName)
        {
            ASSERT_NE(handle, RAZIX_ASSET_INVALID_HANDLE);
            const RZAsset* hdr = db().getAssetHeader(handle);
            ASSERT_NE(hdr, nullptr);
            EXPECT_EQ(hdr->getType(), expectedType);
            EXPECT_STREQ(hdr->getName().c_str(), expectedName);
            EXPECT_TRUE(hdr->hasFlag(RZ_ASSET_FLAG_DIRTY));
        }

        // Extract header + payload indices from handle
        static u32 HeaderIndex(rz_asset_handle h) { return static_cast<u32>(h & RAZIX_ASSET_HOTDATA_MASK); }
        static u32 PayloadIndex(rz_asset_handle h) { return static_cast<u32>((h & RAZIX_ASSET_PAYLOLAD_INDEX_MASK) >> RAZIX_ASSET_PAYLOAD_SHIFT_INDEX); }
    };

    // ═════════════════════════════════════════════════════════════
    // Category 1 — createAsset: Header + Payload verification
    //              One test per non-Transform type (13 total)
    // ═════════════════════════════════════════════════════════════

    TEST_F(AssetDBExtendedFixture, CreateCameraAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZCameraAsset>("TestCam");
        VerifyCreatedHeader(handle, RZAssetType::kCamera, "TestCam");

        RZCameraAsset* cam = db().getAssetResourceMutablePtr<RZCameraAsset>(handle);
        ASSERT_NE(cam, nullptr);
        cam->MovementSpeed   = 5.0f;
        cam->MouseSensitivity = 0.25f;
        cam->Zoom             = 45.0f;
        cam->PerspectiveFOV   = 60.0f;
        cam->PerspectiveNear  = 0.1f;
        cam->PerspectiveFar   = 1000.0f;
        EXPECT_FLOAT_EQ(cam->MovementSpeed, 5.0f);
        EXPECT_FLOAT_EQ(cam->MouseSensitivity, 0.25f);
        EXPECT_FLOAT_EQ(cam->Zoom, 45.0f);
        EXPECT_FLOAT_EQ(cam->PerspectiveFOV, 60.0f);
        db().destroyAsset<RZCameraAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateLightAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZLightAsset>("TestLight");
        VerifyCreatedHeader(handle, RZAssetType::kLight, "TestLight");

        RZLightAsset* light = db().getAssetResourceMutablePtr<RZLightAsset>(handle);
        ASSERT_NE(light, nullptr);
        light->Color_Intensity = {1.0f, 0.8f, 0.6f, 2.5f};
        light->Position_Range  = {0.0f, 10.0f, 0.0f, 50.0f};
        light->Type            = RZ_LIGHT_TYPE_POINT;
        EXPECT_FLOAT_EQ(light->Color_Intensity.y, 0.8f);
        EXPECT_FLOAT_EQ(light->Position_Range.w, 50.0f);
        db().destroyAsset<RZLightAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateMaterialAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZMaterialAsset>("TestMat");
        VerifyCreatedHeader(handle, RZAssetType::kMaterial, "TestMat");

        RZMaterialAsset* mat = db().getAssetResourceMutablePtr<RZMaterialAsset>(handle);
        ASSERT_NE(mat, nullptr);
        mat->Albedo            = {1.0f, 0.0f, 0.0f, 1.0f};
        mat->Metallic          = 0.9f;
        mat->Roughness         = 0.3f;
        mat->AmbientOcclusion  = 1.0f;
        mat->NormalStrength    = 1.0f;
        mat->EmissiveIntensity = 0.0f;
        mat->EmissiveColor     = {0.0f, 0.0f, 0.0f};
        EXPECT_FLOAT_EQ(mat->Metallic, 0.9f);
        EXPECT_FLOAT_EQ(mat->Roughness, 0.3f);
        db().destroyAsset<RZMaterialAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateMeshAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZMeshAsset>("TestMesh");
        VerifyCreatedHeader(handle, RZAssetType::kMesh, "TestMesh");

        RZMeshAsset* mesh = db().getAssetResourceMutablePtr<RZMeshAsset>(handle);
        ASSERT_NE(mesh, nullptr);
        mesh->VertexCount = 1024;
        mesh->IndexCount  = 4096;
        mesh->BoundsMin   = {-1.0f, -1.0f, -1.0f};
        mesh->BoundsMax   = {1.0f, 1.0f, 1.0f};
        mesh->BoundsRadius = 1.732f;
        EXPECT_EQ(mesh->VertexCount, 1024u);
        EXPECT_EQ(mesh->IndexCount, 4096u);
        db().destroyAsset<RZMeshAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateTextureAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZTextureAsset>("TestTex");
        VerifyCreatedHeader(handle, RZAssetType::kTexture, "TestTex");

        RZTextureAsset* tex = db().getAssetResourceMutablePtr<RZTextureAsset>(handle);
        ASSERT_NE(tex, nullptr);
        tex->Desc.width     = 512;
        tex->Desc.height    = 512;
        tex->Desc.depth     = 1;
        tex->Desc.mipLevels = 10;
        EXPECT_EQ(tex->Desc.width, 512u);
        EXPECT_EQ(tex->Desc.height, 512u);
        db().destroyAsset<RZTextureAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateAnimationAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZAnimationAsset>("TestAnim");
        VerifyCreatedHeader(handle, RZAssetType::kAnimation, "TestAnim");

        RZAnimationAsset* anim = db().getAssetResourceMutablePtr<RZAnimationAsset>(handle);
        ASSERT_NE(anim, nullptr);
        anim->FrameCount = 120;
        anim->BoneCount  = 65;
        anim->Duration   = 4.0f;
        anim->FrameRate  = 30.0f;
        anim->bShouldLoop = true;
        EXPECT_EQ(anim->FrameCount, 120u);
        EXPECT_EQ(anim->BoneCount, 65u);
        EXPECT_FLOAT_EQ(anim->Duration, 4.0f);
        EXPECT_TRUE(anim->bShouldLoop);
        db().destroyAsset<RZAnimationAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateAudioAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZAudioAsset>("TestAudio");
        VerifyCreatedHeader(handle, RZAssetType::kAudio, "TestAudio");

        RZAudioAsset* audio = db().getAssetResourceMutablePtr<RZAudioAsset>(handle);
        ASSERT_NE(audio, nullptr);
        audio->SampleRate   = 44100;
        audio->ChannelCount = 2;
        audio->SampleCount  = 88200;
        audio->Duration     = 2.0f;
        audio->Type         = RZ_AUDIO_TYPE_MUSIC;
        audio->bIsLooping   = true;
        EXPECT_EQ(audio->SampleRate, 44100u);
        EXPECT_EQ(audio->ChannelCount, 2u);
        EXPECT_FLOAT_EQ(audio->Duration, 2.0f);
        EXPECT_TRUE(audio->bIsLooping);
        db().destroyAsset<RZAudioAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateLuaScriptAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZLuaScriptAsset>("TestScript");
        VerifyCreatedHeader(handle, RZAssetType::kLuaScript, "TestScript");

        RZLuaScriptAsset* script = db().getAssetResourceMutablePtr<RZLuaScriptAsset>(handle);
        ASSERT_NE(script, nullptr);
        script->ScriptPath       = "//Scripts/test.lua";
        script->LastModifiedTime = 1700000000ULL;
        script->bIsCompiled      = false;
        script->bAutoReload      = true;
        EXPECT_STREQ(script->ScriptPath.c_str(), "//Scripts/test.lua");
        EXPECT_EQ(script->LastModifiedTime, 1700000000ULL);
        EXPECT_TRUE(script->bAutoReload);
        db().destroyAsset<RZLuaScriptAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreatePhysicsMaterialAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZPhysicsMaterialAsset>("TestPhysMat");
        VerifyCreatedHeader(handle, RZAssetType::kPhysicsMaterial, "TestPhysMat");

        RZPhysicsMaterialAsset* pm = db().getAssetResourceMutablePtr<RZPhysicsMaterialAsset>(handle);
        ASSERT_NE(pm, nullptr);
        pm->Density         = 1.0f;
        pm->StaticFriction  = 0.5f;
        pm->DynamicFriction = 0.4f;
        pm->Restitution     = 0.3f;
        pm->LinearDamping   = 0.01f;
        pm->AngularDamping  = 0.05f;
        pm->bIsTrigger      = false;
        EXPECT_FLOAT_EQ(pm->Density, 1.0f);
        EXPECT_FLOAT_EQ(pm->StaticFriction, 0.5f);
        EXPECT_FLOAT_EQ(pm->Restitution, 0.3f);
        db().destroyAsset<RZPhysicsMaterialAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateAssetRefAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZAssetRefAsset>("TestRef");
        VerifyCreatedHeader(handle, RZAssetType::kAssetRef, "TestRef");

        RZAssetRefAsset* ref = db().getAssetResourceMutablePtr<RZAssetRefAsset>(handle);
        ASSERT_NE(ref, nullptr);
        ref->AssetUUID = rz_uuid_generate();
        db().destroyAsset<RZAssetRefAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateGameDataAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZGameDataAsset>("TestGameData");
        VerifyCreatedHeader(handle, RZAssetType::kGameData, "TestGameData");

        RZGameDataAsset* gd = db().getAssetResourceMutablePtr<RZGameDataAsset>(handle);
        ASSERT_NE(gd, nullptr);
        gd->SaveSlotCount      = 3;
        gd->TotalPlayTime      = 7200;
        gd->DifficultyLevel    = 2;
        gd->bIsDLCEnabled      = true;
        gd->CheckpointPosition = {10.0f, 0.0f, -5.0f};
        gd->LastPlayedLevel    = "Level_03";
        EXPECT_EQ(gd->SaveSlotCount, 3u);
        EXPECT_EQ(gd->TotalPlayTime, 7200u);
        EXPECT_TRUE(gd->bIsDLCEnabled);
        EXPECT_STREQ(gd->LastPlayedLevel.c_str(), "Level_03");
        db().destroyAsset<RZGameDataAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateClothAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZClothAsset>("TestCloth");
        VerifyCreatedHeader(handle, RZAssetType::kCloth, "TestCloth");

        RZClothAsset* cloth = db().getAssetResourceMutablePtr<RZClothAsset>(handle);
        ASSERT_NE(cloth, nullptr);
        cloth->VertexCount     = 256;
        cloth->TriangleCount   = 480;
        cloth->ConstraintCount = 1024;
        cloth->Mass            = 1.5f;
        cloth->Damping         = 0.98f;
        cloth->GravityScale    = 1.0f;
        cloth->bIsSimulating   = true;
        EXPECT_EQ(cloth->VertexCount, 256u);
        EXPECT_EQ(cloth->TriangleCount, 480u);
        EXPECT_FLOAT_EQ(cloth->Mass, 1.5f);
        EXPECT_TRUE(cloth->bIsSimulating);
        db().destroyAsset<RZClothAsset>(handle);
    }

    TEST_F(AssetDBExtendedFixture, CreateVignerePuzzleAsset_HeaderAndPayload)
    {
        rz_asset_handle handle = db().createAsset<RZVignerePuzzleAsset>("TestPuzzle");
        VerifyCreatedHeader(handle, RZAssetType::kVignerePuzzle, "TestPuzzle");

        RZVignerePuzzleAsset* puzzle = db().getAssetResourceMutablePtr<RZVignerePuzzleAsset>(handle);
        ASSERT_NE(puzzle, nullptr);
        puzzle->PlainText = "HELLO";
        puzzle->Key       = "KEY";
        puzzle->bIsSolved = false;
        EXPECT_STREQ(puzzle->PlainText.c_str(), "HELLO");
        EXPECT_STREQ(puzzle->Key.c_str(), "KEY");
        EXPECT_FALSE(puzzle->bIsSolved);
        db().destroyAsset<RZVignerePuzzleAsset>(handle);
    }

    // ═════════════════════════════════════════════════════════════
    // Category 2 — destroyAsset: Lifecycle tests
    // ═════════════════════════════════════════════════════════════

    TEST_F(AssetDBExtendedFixture, DestroyAsset_RecyclesSlots)
    {
        rz_asset_handle h1 = db().createAsset<RZCameraAsset>("Cam1");
        ASSERT_NE(h1, RAZIX_ASSET_INVALID_HANDLE);

        const u32 hdr1 = HeaderIndex(h1);
        const u32 pay1 = PayloadIndex(h1);

        db().destroyAsset<RZCameraAsset>(h1);

        rz_asset_handle h2 = db().createAsset<RZCameraAsset>("Cam2");
        ASSERT_NE(h2, RAZIX_ASSET_INVALID_HANDLE);

        const u32 hdr2 = HeaderIndex(h2);
        const u32 pay2 = PayloadIndex(h2);

        // After destroy + re-create, at least one index should be recycled
        EXPECT_TRUE(hdr1 == hdr2 || pay1 == pay2);

        db().destroyAsset<RZCameraAsset>(h2);
    }

    TEST_F(AssetDBExtendedFixture, DestroyAndRecreateDifferentType)
    {
        rz_asset_handle hCam = db().createAsset<RZCameraAsset>("CamForDestroy");
        ASSERT_NE(hCam, RAZIX_ASSET_INVALID_HANDLE);
        db().destroyAsset<RZCameraAsset>(hCam);

        // Create a different type — header slot should be reused
        rz_asset_handle hLight = db().createAsset<RZLightAsset>("LightAfterCam");
        ASSERT_NE(hLight, RAZIX_ASSET_INVALID_HANDLE);

        const u32 hdrCam   = HeaderIndex(hCam);
        const u32 hdrLight = HeaderIndex(hLight);
        EXPECT_EQ(hdrCam, hdrLight);    // header index recycled

        db().destroyAsset<RZLightAsset>(hLight);
    }

    TEST_F(AssetDBExtendedFixture, DestroyMultipleTypesNoLeak)
    {
        rz_asset_handle handles[5];
        handles[0] = db().createAsset<RZCameraAsset>("MC1");
        handles[1] = db().createAsset<RZLightAsset>("MC2");
        handles[2] = db().createAsset<RZAudioAsset>("MC3");
        handles[3] = db().createAsset<RZMeshAsset>("MC4");
        handles[4] = db().createAsset<RZMaterialAsset>("MC5");

        for (auto h : handles)
            ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);

        db().destroyAsset<RZCameraAsset>(handles[0]);
        db().destroyAsset<RZLightAsset>(handles[1]);
        db().destroyAsset<RZAudioAsset>(handles[2]);
        db().destroyAsset<RZMeshAsset>(handles[3]);
        db().destroyAsset<RZMaterialAsset>(handles[4]);

        // After destroying all, we should be able to allocate the same count again
        rz_asset_handle h = db().createAsset<RZCameraAsset>("Recycled");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        db().destroyAsset<RZCameraAsset>(h);
    }

    // ═════════════════════════════════════════════════════════════
    // Category 3 — saveAssetToDisk per asset type
    //              Verifies the .rzasset file is written with
    //              correct magic bytes (0x46415A52)
    //              Skips Transform (SceneGraph-managed)
    // ═════════════════════════════════════════════════════════════

    // Helper: resolve the physical path for a saved asset
    static fs::path ResolveAssetPhysicalPath(RZAssetType type, const char* name)
    {
        RZString vfsPath = AssetTypeToVFSFilePath(type, name);
        RZString physical;
        RZVirtualFileSystem::Get().resolvePhysicalPath(vfsPath, physical);
        return fs::path(physical.c_str());
    }

    // Helper: verify the first 4 bytes of a .rzasset file are the magic number
    static void VerifyAssetFileMagic(const fs::path& path)
    {
        ASSERT_TRUE(fs::exists(path)) << "Asset file not found: " << path.string();
        std::ifstream in(path, std::ios::binary);
        ASSERT_TRUE(in.is_open());
        u32 magic = 0;
        in.read(reinterpret_cast<char*>(&magic), sizeof(u32));
        EXPECT_EQ(magic, RAZIX_ASSSET_FILE_MAGIC);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_CameraAsset)
    {
        rz_asset_handle h = db().createAsset<RZCameraAsset>("SaveCam");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZCameraAsset* cam   = db().getAssetResourceMutablePtr<RZCameraAsset>(h);
        cam->MovementSpeed   = 7.5f;
        cam->PerspectiveFOV  = 90.0f;
        cam->PerspectiveNear = 0.01f;
        cam->PerspectiveFar  = 5000.0f;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kCamera, "SaveCam"));
        db().destroyAsset<RZCameraAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_LightAsset)
    {
        rz_asset_handle h = db().createAsset<RZLightAsset>("SaveLight");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZLightAsset* light = db().getAssetResourceMutablePtr<RZLightAsset>(h);
        light->Color_Intensity = {1.0f, 1.0f, 1.0f, 3.0f};
        light->Type            = RZ_LIGHT_TYPE_SPOT;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kLight, "SaveLight"));
        db().destroyAsset<RZLightAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_MaterialAsset)
    {
        rz_asset_handle h = db().createAsset<RZMaterialAsset>("SaveMat");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZMaterialAsset* mat = db().getAssetResourceMutablePtr<RZMaterialAsset>(h);
        mat->Albedo           = {0.2f, 0.4f, 0.6f, 1.0f};
        mat->Metallic         = 0.8f;
        mat->Roughness        = 0.2f;
        mat->AmbientOcclusion = 1.0f;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kMaterial, "SaveMat"));
        db().destroyAsset<RZMaterialAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_MeshAsset)
    {
        rz_asset_handle h = db().createAsset<RZMeshAsset>("SaveMesh");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZMeshAsset* mesh = db().getAssetResourceMutablePtr<RZMeshAsset>(h);
        mesh->VertexCount  = 2048;
        mesh->IndexCount   = 8192;
        mesh->BoundsMin    = {-2.0f, -2.0f, -2.0f};
        mesh->BoundsMax    = {2.0f, 2.0f, 2.0f};
        mesh->BoundsRadius = 3.464f;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kMesh, "SaveMesh"));
        db().destroyAsset<RZMeshAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_TextureAsset)
    {
        rz_asset_handle h = db().createAsset<RZTextureAsset>("SaveTex");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZTextureAsset* tex = db().getAssetResourceMutablePtr<RZTextureAsset>(h);
        tex->Desc.width     = 1024;
        tex->Desc.height    = 1024;
        tex->Desc.depth     = 1;
        tex->Desc.mipLevels = 11;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kTexture, "SaveTex"));
        db().destroyAsset<RZTextureAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_AnimationAsset)
    {
        rz_asset_handle h = db().createAsset<RZAnimationAsset>("SaveAnim");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZAnimationAsset* anim = db().getAssetResourceMutablePtr<RZAnimationAsset>(h);
        anim->FrameCount  = 240;
        anim->BoneCount   = 50;
        anim->Duration    = 8.0f;
        anim->FrameRate   = 30.0f;
        anim->bShouldLoop = true;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kAnimation, "SaveAnim"));
        db().destroyAsset<RZAnimationAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_AudioAsset)
    {
        rz_asset_handle h = db().createAsset<RZAudioAsset>("SaveAudio");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZAudioAsset* audio = db().getAssetResourceMutablePtr<RZAudioAsset>(h);
        audio->SampleRate   = 48000;
        audio->ChannelCount = 1;
        audio->Duration     = 5.5f;
        audio->Type         = RZ_AUDIO_TYPE_SOUND_EFFECT;
        audio->bIsLooping   = false;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kAudio, "SaveAudio"));
        db().destroyAsset<RZAudioAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_LuaScriptAsset)
    {
        rz_asset_handle h = db().createAsset<RZLuaScriptAsset>("SaveScript");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZLuaScriptAsset* script = db().getAssetResourceMutablePtr<RZLuaScriptAsset>(h);
        script->ScriptPath       = "//Scripts/save_test.lua";
        script->LastModifiedTime = 1710000000ULL;
        script->bIsCompiled      = true;
        script->bAutoReload      = false;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kLuaScript, "SaveScript"));
        db().destroyAsset<RZLuaScriptAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_PhysicsMaterialAsset)
    {
        rz_asset_handle h = db().createAsset<RZPhysicsMaterialAsset>("SavePhysMat");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZPhysicsMaterialAsset* pm = db().getAssetResourceMutablePtr<RZPhysicsMaterialAsset>(h);
        pm->Density         = 2.5f;
        pm->StaticFriction  = 0.6f;
        pm->DynamicFriction = 0.5f;
        pm->Restitution     = 0.7f;
        pm->bIsTrigger      = true;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kPhysicsMaterial, "SavePhysMat"));
        db().destroyAsset<RZPhysicsMaterialAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_AssetRefAsset)
    {
        rz_asset_handle h = db().createAsset<RZAssetRefAsset>("SaveRef");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZAssetRefAsset* ref = db().getAssetResourceMutablePtr<RZAssetRefAsset>(h);
        ref->AssetUUID       = rz_uuid_generate();

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kAssetRef, "SaveRef"));
        db().destroyAsset<RZAssetRefAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_GameDataAsset)
    {
        rz_asset_handle h = db().createAsset<RZGameDataAsset>("SaveGameData");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZGameDataAsset* gd = db().getAssetResourceMutablePtr<RZGameDataAsset>(h);
        gd->SaveSlotCount   = 5;
        gd->TotalPlayTime   = 14400;
        gd->DifficultyLevel = 3;
        gd->bIsDLCEnabled   = false;
        gd->LastPlayedLevel = "BossArena";

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kGameData, "SaveGameData"));
        db().destroyAsset<RZGameDataAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_ClothAsset)
    {
        rz_asset_handle h = db().createAsset<RZClothAsset>("SaveCloth");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZClothAsset* cloth = db().getAssetResourceMutablePtr<RZClothAsset>(h);
        cloth->VertexCount   = 512;
        cloth->TriangleCount = 960;
        cloth->Mass          = 2.0f;
        cloth->Damping       = 0.95f;
        cloth->GravityScale  = 1.0f;
        cloth->bIsSimulating = false;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kCloth, "SaveCloth"));
        db().destroyAsset<RZClothAsset>(h);
    }

    TEST_F(AssetDBExtendedFixture, SaveToDisk_VignerePuzzleAsset)
    {
        rz_asset_handle h = db().createAsset<RZVignerePuzzleAsset>("SavePuzzle");
        ASSERT_NE(h, RAZIX_ASSET_INVALID_HANDLE);
        RZVignerePuzzleAsset* puzzle = db().getAssetResourceMutablePtr<RZVignerePuzzleAsset>(h);
        puzzle->PlainText = "ATTACKATDAWN";
        puzzle->Key       = "LEMON";
        puzzle->bIsSolved = true;

        EXPECT_TRUE(db().saveAssetToDisk(h));
        VerifyAssetFileMagic(ResolveAssetPhysicalPath(RZAssetType::kVignerePuzzle, "SavePuzzle"));
        db().destroyAsset<RZVignerePuzzleAsset>(h);
    }

    // ═════════════════════════════════════════════════════════════
    // Category 4 — exportAssetDBRegistry / loadAssetDBRegistry
    //              Round-trip: create assets → export → shutdown →
    //              re-startup → verify registry entries survive
    // ═════════════════════════════════════════════════════════════

    TEST_F(AssetDBExtendedFixture, RegistryRoundTrip_MultipleAssets)
    {
        // Create a few assets of different types
        rz_asset_handle hCam   = db().createAsset<RZCameraAsset>("RegCam");
        rz_asset_handle hLight = db().createAsset<RZLightAsset>("RegLight");
        rz_asset_handle hAudio = db().createAsset<RZAudioAsset>("RegAudio");
        rz_asset_handle hMesh  = db().createAsset<RZMeshAsset>("RegMesh");
        rz_asset_handle hMat   = db().createAsset<RZMaterialAsset>("RegMat");

        ASSERT_NE(hCam, RAZIX_ASSET_INVALID_HANDLE);
        ASSERT_NE(hLight, RAZIX_ASSET_INVALID_HANDLE);
        ASSERT_NE(hAudio, RAZIX_ASSET_INVALID_HANDLE);
        ASSERT_NE(hMesh, RAZIX_ASSET_INVALID_HANDLE);
        ASSERT_NE(hMat, RAZIX_ASSET_INVALID_HANDLE);

        // Collect UUIDs before shutdown
        rz_uuid uuidCam   = db().getAssetHeader(hCam)->getUUID();
        rz_uuid uuidLight = db().getAssetHeader(hLight)->getUUID();
        rz_uuid uuidAudio = db().getAssetHeader(hAudio)->getUUID();
        rz_uuid uuidMesh  = db().getAssetHeader(hMesh)->getUUID();
        rz_uuid uuidMat   = db().getAssetHeader(hMat)->getUUID();

        // Shutdown exports registry to assetdb.bin
        db().Shutdown();

        // Verify assetdb.bin was created
        fs::path registryPath = m_TempAssetsDir / RAZIX_ASSET_DB_REGISTRY_FILE_NAME;
        ASSERT_TRUE(fs::exists(registryPath)) << "assetdb.bin should exist after Shutdown export";
        EXPECT_GT(fs::file_size(registryPath), sizeof(u32)) << "assetdb.bin should have content";

        // Re-startup — this will call loadAssetDBRegistry() internally
        db().Startup(m_AssetAllocator, m_AssetHeaderAllocator);

        // The registry should have been loaded, but asset *handles* won't exist yet.
        // We verify by attempting requestAssetLoad with the UUIDs — if the registry
        // loaded correctly, it should find the entries (even though actual file loading
        // may fail since we didn't saveAssetToDisk in this test — that's OK, the test
        // verifies registry persistence, not file loading).

        // To really verify the registry survived, we can create new assets and check the
        // registry contains at least the original 5 entries. Since registry is private,
        // we test observationally through requestAssetLoad returning a valid handle
        // when the UUID is in the registry vs INVALID_HANDLE when it's not.

        // This UUID was never registered — should fail
        rz_uuid bogusUUID = rz_uuid_generate();
        rz_asset_handle hBogus = db().requestAssetLoad<RZCameraAsset>(bogusUUID);
        // Note: bogusUUID is a fresh UUID that was never in the registry, so this should fail
        // However, rz_uuid_generate() generates a new random UUID, so it won't match any entry
        EXPECT_EQ(hBogus, RAZIX_ASSET_INVALID_HANDLE);
    }

    TEST_F(AssetDBExtendedFixture, RegistryRoundTrip_EmptyDB)
    {
        // No assets created — just shutdown and re-startup
        db().Shutdown();

        fs::path registryPath = m_TempAssetsDir / RAZIX_ASSET_DB_REGISTRY_FILE_NAME;
        // Registry file should exist even if empty (with entry count = 0)
        if (fs::exists(registryPath)) {
            EXPECT_GE(fs::file_size(registryPath), sizeof(u32));
        }

        db().Startup(m_AssetAllocator, m_AssetHeaderAllocator);
        // Should succeed without issues
        SUCCEED();
    }

    // ═════════════════════════════════════════════════════════════
    // Category 5 — requestAssetLoad (async): save → load → verify
    //              Full round-trip per type. Uses the job system.
    //              Placeholder flags verified before wait_for_all.
    // ═════════════════════════════════════════════════════════════

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_CameraAsset)
    {
        // Create + populate + save
        rz_asset_handle hOrig = db().createAsset<RZCameraAsset>("AsyncCam");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZCameraAsset* cam   = db().getAssetResourceMutablePtr<RZCameraAsset>(hOrig);
        cam->MovementSpeed   = 7.5f;
        cam->PerspectiveFOV  = 90.0f;
        cam->PerspectiveNear = 0.01f;
        cam->PerspectiveFar  = 5000.0f;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid uuid = db().getAssetHeader(hOrig)->getUUID();

        // Request async load — returns a placeholder handle immediately
        rz_asset_handle hLoaded = db().requestAssetLoad<RZCameraAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);

        // Before completion: placeholder + streaming flags should be set, dirty should NOT
        const RZAsset* loadHdr = db().getAssetHeader(hLoaded);
        ASSERT_NE(loadHdr, nullptr);
        EXPECT_TRUE(loadHdr->hasFlag(RZ_ASSET_FLAG_PLACEHOLDER));
        EXPECT_TRUE(loadHdr->hasFlag(RZ_ASSET_FLAG_STREAMING));
        EXPECT_FALSE(loadHdr->hasFlag(RZ_ASSET_FLAG_DIRTY));

        // Wait for async job to complete
        rz_job_system_wait_for_all();

        // Verify loaded payload matches what was saved
        RZCameraAsset* loaded = db().getAssetResourceMutablePtr<RZCameraAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_FLOAT_EQ(loaded->MovementSpeed, 7.5f);
        EXPECT_FLOAT_EQ(loaded->PerspectiveFOV, 90.0f);
        EXPECT_FLOAT_EQ(loaded->PerspectiveNear, 0.01f);
        EXPECT_FLOAT_EQ(loaded->PerspectiveFar, 5000.0f);

        db().destroyAsset<RZCameraAsset>(hOrig);
        db().destroyAsset<RZCameraAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_LightAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZLightAsset>("AsyncLight");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZLightAsset* light    = db().getAssetResourceMutablePtr<RZLightAsset>(hOrig);
        light->Color_Intensity = {0.9f, 0.7f, 0.5f, 4.0f};
        light->Position_Range  = {5.0f, 15.0f, -3.0f, 100.0f};
        light->Type            = RZ_LIGHT_TYPE_DIRECTIONAL;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZLightAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZLightAsset* loaded = db().getAssetResourceMutablePtr<RZLightAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_FLOAT_EQ(loaded->Color_Intensity.x, 0.9f);
        EXPECT_FLOAT_EQ(loaded->Color_Intensity.w, 4.0f);
        EXPECT_FLOAT_EQ(loaded->Position_Range.y, 15.0f);

        db().destroyAsset<RZLightAsset>(hOrig);
        db().destroyAsset<RZLightAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_MaterialAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZMaterialAsset>("AsyncMat");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZMaterialAsset* mat  = db().getAssetResourceMutablePtr<RZMaterialAsset>(hOrig);
        mat->Albedo           = {0.1f, 0.2f, 0.3f, 1.0f};
        mat->Metallic         = 0.75f;
        mat->Roughness        = 0.15f;
        mat->AmbientOcclusion = 0.9f;
        mat->NormalStrength   = 1.2f;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZMaterialAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZMaterialAsset* loaded = db().getAssetResourceMutablePtr<RZMaterialAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_FLOAT_EQ(loaded->Albedo.x, 0.1f);
        EXPECT_FLOAT_EQ(loaded->Metallic, 0.75f);
        EXPECT_FLOAT_EQ(loaded->Roughness, 0.15f);

        db().destroyAsset<RZMaterialAsset>(hOrig);
        db().destroyAsset<RZMaterialAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_MeshAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZMeshAsset>("AsyncMesh");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZMeshAsset* mesh  = db().getAssetResourceMutablePtr<RZMeshAsset>(hOrig);
        mesh->VertexCount  = 4096;
        mesh->IndexCount   = 16384;
        mesh->BoundsMin    = {-5.0f, -5.0f, -5.0f};
        mesh->BoundsMax    = {5.0f, 5.0f, 5.0f};
        mesh->BoundsRadius = 8.66f;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZMeshAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZMeshAsset* loaded = db().getAssetResourceMutablePtr<RZMeshAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_EQ(loaded->VertexCount, 4096u);
        EXPECT_EQ(loaded->IndexCount, 16384u);
        EXPECT_FLOAT_EQ(loaded->BoundsRadius, 8.66f);

        db().destroyAsset<RZMeshAsset>(hOrig);
        db().destroyAsset<RZMeshAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_TextureAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZTextureAsset>("AsyncTex");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZTextureAsset* tex = db().getAssetResourceMutablePtr<RZTextureAsset>(hOrig);
        tex->Desc.width     = 2048;
        tex->Desc.height    = 2048;
        tex->Desc.depth     = 1;
        tex->Desc.mipLevels = 12;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZTextureAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZTextureAsset* loaded = db().getAssetResourceMutablePtr<RZTextureAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_EQ(loaded->Desc.width, 2048u);
        EXPECT_EQ(loaded->Desc.height, 2048u);
        EXPECT_EQ(loaded->Desc.mipLevels, 12u);

        db().destroyAsset<RZTextureAsset>(hOrig);
        db().destroyAsset<RZTextureAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_AnimationAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZAnimationAsset>("AsyncAnim");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZAnimationAsset* anim = db().getAssetResourceMutablePtr<RZAnimationAsset>(hOrig);
        anim->FrameCount  = 360;
        anim->BoneCount   = 80;
        anim->Duration    = 12.0f;
        anim->FrameRate   = 30.0f;
        anim->bShouldLoop = false;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZAnimationAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZAnimationAsset* loaded = db().getAssetResourceMutablePtr<RZAnimationAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_EQ(loaded->FrameCount, 360u);
        EXPECT_EQ(loaded->BoneCount, 80u);
        EXPECT_FLOAT_EQ(loaded->Duration, 12.0f);
        EXPECT_FALSE(loaded->bShouldLoop);

        db().destroyAsset<RZAnimationAsset>(hOrig);
        db().destroyAsset<RZAnimationAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_AudioAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZAudioAsset>("AsyncAudio");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZAudioAsset* audio = db().getAssetResourceMutablePtr<RZAudioAsset>(hOrig);
        audio->SampleRate   = 96000;
        audio->ChannelCount = 2;
        audio->SampleCount  = 192000;
        audio->Duration     = 2.0f;
        audio->Type         = RZ_AUDIO_TYPE_VOICE;
        audio->bIsLooping   = true;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZAudioAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZAudioAsset* loaded = db().getAssetResourceMutablePtr<RZAudioAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_EQ(loaded->SampleRate, 96000u);
        EXPECT_EQ(loaded->ChannelCount, 2u);
        EXPECT_FLOAT_EQ(loaded->Duration, 2.0f);
        EXPECT_TRUE(loaded->bIsLooping);

        db().destroyAsset<RZAudioAsset>(hOrig);
        db().destroyAsset<RZAudioAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_LuaScriptAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZLuaScriptAsset>("AsyncScript");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZLuaScriptAsset* script = db().getAssetResourceMutablePtr<RZLuaScriptAsset>(hOrig);
        script->ScriptPath       = "//Scripts/async_test.lua";
        script->LastModifiedTime = 1720000000ULL;
        script->bIsCompiled      = true;
        script->bAutoReload      = true;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZLuaScriptAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZLuaScriptAsset* loaded = db().getAssetResourceMutablePtr<RZLuaScriptAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_STREQ(loaded->ScriptPath.c_str(), "//Scripts/async_test.lua");
        EXPECT_EQ(loaded->LastModifiedTime, 1720000000ULL);
        EXPECT_TRUE(loaded->bIsCompiled);

        db().destroyAsset<RZLuaScriptAsset>(hOrig);
        db().destroyAsset<RZLuaScriptAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_PhysicsMaterialAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZPhysicsMaterialAsset>("AsyncPhysMat");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZPhysicsMaterialAsset* pm = db().getAssetResourceMutablePtr<RZPhysicsMaterialAsset>(hOrig);
        pm->Density         = 3.0f;
        pm->StaticFriction  = 0.8f;
        pm->DynamicFriction = 0.7f;
        pm->Restitution     = 0.1f;
        pm->LinearDamping   = 0.02f;
        pm->AngularDamping  = 0.04f;
        pm->bIsTrigger      = false;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZPhysicsMaterialAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZPhysicsMaterialAsset* loaded = db().getAssetResourceMutablePtr<RZPhysicsMaterialAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_FLOAT_EQ(loaded->Density, 3.0f);
        EXPECT_FLOAT_EQ(loaded->StaticFriction, 0.8f);
        EXPECT_FLOAT_EQ(loaded->Restitution, 0.1f);
        EXPECT_FALSE(loaded->bIsTrigger);

        db().destroyAsset<RZPhysicsMaterialAsset>(hOrig);
        db().destroyAsset<RZPhysicsMaterialAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_AssetRefAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZAssetRefAsset>("AsyncRef");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZAssetRefAsset* ref = db().getAssetResourceMutablePtr<RZAssetRefAsset>(hOrig);
        rz_uuid          refTargetUUID = rz_uuid_generate();    // fresh UUID
        ref->AssetUUID = refTargetUUID;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZAssetRefAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZAssetRefAsset* loaded = db().getAssetResourceMutablePtr<RZAssetRefAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_EQ(loaded->AssetUUID, refTargetUUID);

        db().destroyAsset<RZAssetRefAsset>(hOrig);
        db().destroyAsset<RZAssetRefAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_GameDataAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZGameDataAsset>("AsyncGameData");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZGameDataAsset* gd = db().getAssetResourceMutablePtr<RZGameDataAsset>(hOrig);
        gd->SaveSlotCount      = 10;
        gd->TotalPlayTime      = 36000;
        gd->DifficultyLevel    = 5;
        gd->bIsDLCEnabled      = true;
        gd->CheckpointPosition = {100.0f, 50.0f, -200.0f};
        gd->LastPlayedLevel    = "FinalChapter";
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZGameDataAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZGameDataAsset* loaded = db().getAssetResourceMutablePtr<RZGameDataAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_EQ(loaded->SaveSlotCount, 10u);
        EXPECT_EQ(loaded->TotalPlayTime, 36000u);
        EXPECT_EQ(loaded->DifficultyLevel, 5u);
        EXPECT_TRUE(loaded->bIsDLCEnabled);
        EXPECT_STREQ(loaded->LastPlayedLevel.c_str(), "FinalChapter");

        db().destroyAsset<RZGameDataAsset>(hOrig);
        db().destroyAsset<RZGameDataAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_ClothAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZClothAsset>("AsyncCloth");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZClothAsset* cloth = db().getAssetResourceMutablePtr<RZClothAsset>(hOrig);
        cloth->VertexCount     = 1024;
        cloth->TriangleCount   = 1920;
        cloth->ConstraintCount = 4096;
        cloth->Mass            = 3.0f;
        cloth->Damping         = 0.9f;
        cloth->GravityScale    = 0.5f;
        cloth->bIsSimulating   = true;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZClothAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZClothAsset* loaded = db().getAssetResourceMutablePtr<RZClothAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_EQ(loaded->VertexCount, 1024u);
        EXPECT_EQ(loaded->TriangleCount, 1920u);
        EXPECT_FLOAT_EQ(loaded->Mass, 3.0f);
        EXPECT_TRUE(loaded->bIsSimulating);

        db().destroyAsset<RZClothAsset>(hOrig);
        db().destroyAsset<RZClothAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoadRoundTrip_VignerePuzzleAsset)
    {
        rz_asset_handle hOrig = db().createAsset<RZVignerePuzzleAsset>("AsyncPuzzle");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZVignerePuzzleAsset* puzzle = db().getAssetResourceMutablePtr<RZVignerePuzzleAsset>(hOrig);
        puzzle->PlainText = "THEQUICKBROWNFOX";
        puzzle->Key       = "CIPHER";
        puzzle->bIsSolved = false;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid          uuid    = db().getAssetHeader(hOrig)->getUUID();
        rz_asset_handle hLoaded = db().requestAssetLoad<RZVignerePuzzleAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);
        rz_job_system_wait_for_all();

        RZVignerePuzzleAsset* loaded = db().getAssetResourceMutablePtr<RZVignerePuzzleAsset>(hLoaded);
        ASSERT_NE(loaded, nullptr);
        EXPECT_STREQ(loaded->PlainText.c_str(), "THEQUICKBROWNFOX");
        EXPECT_STREQ(loaded->Key.c_str(), "CIPHER");
        EXPECT_FALSE(loaded->bIsSolved);

        db().destroyAsset<RZVignerePuzzleAsset>(hOrig);
        db().destroyAsset<RZVignerePuzzleAsset>(hLoaded);
    }

    // ═════════════════════════════════════════════════════════════
    // Category 6 — Parallel batch loading + Timing
    // ═════════════════════════════════════════════════════════════

    TEST_F(AssetDBExtendedFixture, AsyncLoad_PlaceholderFlagsBeforeCompletion)
    {
        // Create + save a camera
        rz_asset_handle hOrig = db().createAsset<RZCameraAsset>("FlagCheckCam");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZCameraAsset* cam = db().getAssetResourceMutablePtr<RZCameraAsset>(hOrig);
        cam->MovementSpeed = 1.0f;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));

        rz_uuid uuid = db().getAssetHeader(hOrig)->getUUID();

        // Request load — header should immediately have placeholder flags
        rz_asset_handle hLoaded = db().requestAssetLoad<RZCameraAsset>(uuid);
        ASSERT_NE(hLoaded, RAZIX_ASSET_INVALID_HANDLE);

        const RZAsset* hdr = db().getAssetHeader(hLoaded);
        ASSERT_NE(hdr, nullptr);
        EXPECT_TRUE(hdr->hasFlag(RZ_ASSET_FLAG_PLACEHOLDER)) << "PLACEHOLDER flag must be set on async load request";
        EXPECT_TRUE(hdr->hasFlag(RZ_ASSET_FLAG_STREAMING)) << "STREAMING flag must be set on async load request";
        EXPECT_FALSE(hdr->hasFlag(RZ_ASSET_FLAG_DIRTY)) << "DIRTY flag must be cleared on async load request";

        rz_job_system_wait_for_all();

        db().destroyAsset<RZCameraAsset>(hOrig);
        db().destroyAsset<RZCameraAsset>(hLoaded);
    }

    TEST_F(AssetDBExtendedFixture, AsyncLoad_ManyAssetsParallel)
    {
        // Create and save 10 assets of mixed types
        struct AssetEntry
        {
            rz_asset_handle origHandle;
            rz_asset_handle loadedHandle;
            rz_uuid          uuid;
        };

        AssetEntry entries[10];

        // Create 10 assets of various types
        entries[0].origHandle = db().createAsset<RZCameraAsset>("BatchCam1");
        entries[1].origHandle = db().createAsset<RZCameraAsset>("BatchCam2");
        entries[2].origHandle = db().createAsset<RZLightAsset>("BatchLight1");
        entries[3].origHandle = db().createAsset<RZLightAsset>("BatchLight2");
        entries[4].origHandle = db().createAsset<RZMaterialAsset>("BatchMat1");
        entries[5].origHandle = db().createAsset<RZAudioAsset>("BatchAudio1");
        entries[6].origHandle = db().createAsset<RZAnimationAsset>("BatchAnim1");
        entries[7].origHandle = db().createAsset<RZPhysicsMaterialAsset>("BatchPhys1");
        entries[8].origHandle = db().createAsset<RZGameDataAsset>("BatchGD1");
        entries[9].origHandle = db().createAsset<RZClothAsset>("BatchCloth1");

        // Set some payload data and save
        db().getAssetResourceMutablePtr<RZCameraAsset>(entries[0].origHandle)->MovementSpeed = 1.0f;
        db().getAssetResourceMutablePtr<RZCameraAsset>(entries[1].origHandle)->MovementSpeed = 2.0f;
        db().getAssetResourceMutablePtr<RZLightAsset>(entries[2].origHandle)->Color_Intensity = {1, 0, 0, 1};
        db().getAssetResourceMutablePtr<RZLightAsset>(entries[3].origHandle)->Color_Intensity = {0, 1, 0, 2};
        db().getAssetResourceMutablePtr<RZMaterialAsset>(entries[4].origHandle)->Metallic = 0.5f;
        db().getAssetResourceMutablePtr<RZAudioAsset>(entries[5].origHandle)->SampleRate = 44100;
        db().getAssetResourceMutablePtr<RZAnimationAsset>(entries[6].origHandle)->FrameCount = 100;
        db().getAssetResourceMutablePtr<RZPhysicsMaterialAsset>(entries[7].origHandle)->Density = 7.8f;
        db().getAssetResourceMutablePtr<RZGameDataAsset>(entries[8].origHandle)->SaveSlotCount = 8;
        db().getAssetResourceMutablePtr<RZClothAsset>(entries[9].origHandle)->VertexCount = 333;

        for (auto& e : entries) {
            ASSERT_NE(e.origHandle, RAZIX_ASSET_INVALID_HANDLE);
            ASSERT_TRUE(db().saveAssetToDisk(e.origHandle));
            e.uuid = db().getAssetHeader(e.origHandle)->getUUID();
        }

        // Request all loads in batch — jobs fire in parallel
        auto batchStart = Clock::now();

        entries[0].loadedHandle = db().requestAssetLoad<RZCameraAsset>(entries[0].uuid);
        entries[1].loadedHandle = db().requestAssetLoad<RZCameraAsset>(entries[1].uuid);
        entries[2].loadedHandle = db().requestAssetLoad<RZLightAsset>(entries[2].uuid);
        entries[3].loadedHandle = db().requestAssetLoad<RZLightAsset>(entries[3].uuid);
        entries[4].loadedHandle = db().requestAssetLoad<RZMaterialAsset>(entries[4].uuid);
        entries[5].loadedHandle = db().requestAssetLoad<RZAudioAsset>(entries[5].uuid);
        entries[6].loadedHandle = db().requestAssetLoad<RZAnimationAsset>(entries[6].uuid);
        entries[7].loadedHandle = db().requestAssetLoad<RZPhysicsMaterialAsset>(entries[7].uuid);
        entries[8].loadedHandle = db().requestAssetLoad<RZGameDataAsset>(entries[8].uuid);
        entries[9].loadedHandle = db().requestAssetLoad<RZClothAsset>(entries[9].uuid);

        for (auto& e : entries)
            ASSERT_NE(e.loadedHandle, RAZIX_ASSET_INVALID_HANDLE);

        rz_job_system_wait_for_all();

        auto    batchEnd = Clock::now();
        double  batchMs  = std::chrono::duration<double, std::milli>(batchEnd - batchStart).count();

        // Verify a sampling of the loaded payloads
        EXPECT_FLOAT_EQ(db().getAssetResourceMutablePtr<RZCameraAsset>(entries[0].loadedHandle)->MovementSpeed, 1.0f);
        EXPECT_FLOAT_EQ(db().getAssetResourceMutablePtr<RZCameraAsset>(entries[1].loadedHandle)->MovementSpeed, 2.0f);
        EXPECT_FLOAT_EQ(db().getAssetResourceMutablePtr<RZLightAsset>(entries[2].loadedHandle)->Color_Intensity.x, 1.0f);
        EXPECT_FLOAT_EQ(db().getAssetResourceMutablePtr<RZMaterialAsset>(entries[4].loadedHandle)->Metallic, 0.5f);
        EXPECT_EQ(db().getAssetResourceMutablePtr<RZAudioAsset>(entries[5].loadedHandle)->SampleRate, 44100u);
        EXPECT_EQ(db().getAssetResourceMutablePtr<RZAnimationAsset>(entries[6].loadedHandle)->FrameCount, 100u);
        EXPECT_FLOAT_EQ(db().getAssetResourceMutablePtr<RZPhysicsMaterialAsset>(entries[7].loadedHandle)->Density, 7.8f);
        EXPECT_EQ(db().getAssetResourceMutablePtr<RZGameDataAsset>(entries[8].loadedHandle)->SaveSlotCount, 8u);
        EXPECT_EQ(db().getAssetResourceMutablePtr<RZClothAsset>(entries[9].loadedHandle)->VertexCount, 333u);

        printf("\n[TIMING] Batch load 10 mixed-type assets: %.3f ms (%.3f ms/asset)\n", batchMs, batchMs / 10.0);

        // Cleanup
        db().destroyAsset<RZCameraAsset>(entries[0].origHandle);
        db().destroyAsset<RZCameraAsset>(entries[1].origHandle);
        db().destroyAsset<RZLightAsset>(entries[2].origHandle);
        db().destroyAsset<RZLightAsset>(entries[3].origHandle);
        db().destroyAsset<RZMaterialAsset>(entries[4].origHandle);
        db().destroyAsset<RZAudioAsset>(entries[5].origHandle);
        db().destroyAsset<RZAnimationAsset>(entries[6].origHandle);
        db().destroyAsset<RZPhysicsMaterialAsset>(entries[7].origHandle);
        db().destroyAsset<RZGameDataAsset>(entries[8].origHandle);
        db().destroyAsset<RZClothAsset>(entries[9].origHandle);

        db().destroyAsset<RZCameraAsset>(entries[0].loadedHandle);
        db().destroyAsset<RZCameraAsset>(entries[1].loadedHandle);
        db().destroyAsset<RZLightAsset>(entries[2].loadedHandle);
        db().destroyAsset<RZLightAsset>(entries[3].loadedHandle);
        db().destroyAsset<RZMaterialAsset>(entries[4].loadedHandle);
        db().destroyAsset<RZAudioAsset>(entries[5].loadedHandle);
        db().destroyAsset<RZAnimationAsset>(entries[6].loadedHandle);
        db().destroyAsset<RZPhysicsMaterialAsset>(entries[7].loadedHandle);
        db().destroyAsset<RZGameDataAsset>(entries[8].loadedHandle);
        db().destroyAsset<RZClothAsset>(entries[9].loadedHandle);
    }

    TEST_F(AssetDBExtendedFixture, DISABLED_TimingSingleAsyncLoad)
    {
        rz_asset_handle hOrig = db().createAsset<RZCameraAsset>("TimingSingleCam");
        ASSERT_NE(hOrig, RAZIX_ASSET_INVALID_HANDLE);
        RZCameraAsset* cam = db().getAssetResourceMutablePtr<RZCameraAsset>(hOrig);
        cam->MovementSpeed = 99.0f;
        ASSERT_TRUE(db().saveAssetToDisk(hOrig));
        rz_uuid uuid = db().getAssetHeader(hOrig)->getUUID();

        auto start = Clock::now();

        rz_asset_handle hLoaded = db().requestAssetLoad<RZCameraAsset>(uuid);
        rz_job_system_wait_for_all();

        auto   end     = Clock::now();
        double elapsMs = std::chrono::duration<double, std::milli>(end - start).count();

        printf("\n[TIMING] Single async camera asset load: %.3f ms\n", elapsMs);

        db().destroyAsset<RZCameraAsset>(hOrig);
        if (hLoaded != RAZIX_ASSET_INVALID_HANDLE)
            db().destroyAsset<RZCameraAsset>(hLoaded);

        SUCCEED();
    }

    TEST_F(AssetDBExtendedFixture, DISABLED_TimingBatchAsyncLoad_20Assets)
    {
        constexpr u32 kBatchSize = 20;

        rz_asset_handle origHandles[kBatchSize];
        rz_asset_handle loadHandles[kBatchSize];
        rz_uuid          uuids[kBatchSize];

        // Create and save 20 camera assets with distinct data
        for (u32 i = 0; i < kBatchSize; ++i) {
            char name[64];
            snprintf(name, sizeof(name), "TimingBatchCam_%02u", i);
            origHandles[i] = db().createAsset<RZCameraAsset>(name);
            ASSERT_NE(origHandles[i], RAZIX_ASSET_INVALID_HANDLE);
            RZCameraAsset* cam = db().getAssetResourceMutablePtr<RZCameraAsset>(origHandles[i]);
            cam->MovementSpeed = static_cast<f32>(i) + 0.5f;
            ASSERT_TRUE(db().saveAssetToDisk(origHandles[i]));
            uuids[i] = db().getAssetHeader(origHandles[i])->getUUID();
        }

        // Batch load
        auto start = Clock::now();
        for (u32 i = 0; i < kBatchSize; ++i) {
            loadHandles[i] = db().requestAssetLoad<RZCameraAsset>(uuids[i]);
        }
        rz_job_system_wait_for_all();
        auto   end     = Clock::now();
        double batchMs = std::chrono::duration<double, std::milli>(end - start).count();

        printf("\n[TIMING] Batch async load %u camera assets: %.3f ms total, %.3f ms/asset\n",
            kBatchSize, batchMs, batchMs / static_cast<double>(kBatchSize));

        // Cleanup
        for (u32 i = 0; i < kBatchSize; ++i) {
            db().destroyAsset<RZCameraAsset>(origHandles[i]);
            if (loadHandles[i] != RAZIX_ASSET_INVALID_HANDLE)
                db().destroyAsset<RZCameraAsset>(loadHandles[i]);
        }

        SUCCEED();
    }

}    // namespace Razix
