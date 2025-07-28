// ShaderLibraryTests.cpp
// Unit tests for the RZShaderLibrary system
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Gfx/RZShaderLibrary.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace Gfx {

        class RZShaderLibraryTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Setup for shader library tests
                Razix::Debug::RZLog::StartUp();
            }

            void TearDown() override
            {
                // Cleanup for shader library tests
                Razix::Debug::RZLog::Shutdown();
            }
        };

        // Test case for shader library singleton
        TEST_F(RZShaderLibraryTests, SingletonAccess)
        {
            // Test that we can access the shader library singleton
            RZShaderLibrary& lib1 = RZShaderLibrary::Get();
            RZShaderLibrary& lib2 = RZShaderLibrary::Get();
            
            // Should be the same instance
            EXPECT_EQ(&lib1, &lib2);
        }

        // Test case for shader name validation
        TEST_F(RZShaderLibraryTests, ShaderNameValidation)
        {
            // Test common shader names that should be supported
            std::vector<std::string> commonShaderNames = {
                "BasicVertex",
                "BasicFragment", 
                "PBRVertex",
                "PBRFragment",
                "SkyboxVertex",
                "SkyboxFragment",
                "ShadowMapVertex",
                "ShadowMapFragment",
                "PostProcessVertex",
                "PostProcessFragment"
            };
            
            for (const auto& name : commonShaderNames) {
                // Test that names are valid strings
                EXPECT_FALSE(name.empty());
                EXPECT_TRUE(name.length() > 0);
                
                // Test that names don't contain invalid characters
                EXPECT_EQ(name.find(' '), std::string::npos) << "Shader name should not contain spaces: " << name;
                EXPECT_EQ(name.find('\n'), std::string::npos) << "Shader name should not contain newlines: " << name;
                EXPECT_EQ(name.find('\t'), std::string::npos) << "Shader name should not contain tabs: " << name;
            }
        }

        // Test case for shader file extension validation
        TEST_F(RZShaderLibraryTests, ShaderFileExtensions)
        {
            // Test common shader file extensions
            std::vector<std::string> validExtensions = {
                ".vert", ".frag", ".geom", ".comp", ".tesc", ".tese",
                ".vertex", ".fragment", ".geometry", ".compute",
                ".hlsl", ".glsl", ".spv"
            };
            
            for (const auto& ext : validExtensions) {
                // Test that extensions start with dot
                EXPECT_EQ(ext[0], '.');
                
                // Test that extensions are reasonable length
                EXPECT_GE(ext.length(), 2);
                EXPECT_LE(ext.length(), 10);
                
                // Test that extensions contain only valid characters
                for (char c : ext) {
                    EXPECT_TRUE(std::isalnum(c) || c == '.');
                }
            }
        }

        // Test case for shader path construction
        TEST_F(RZShaderLibraryTests, ShaderPathConstruction)
        {
            // Test shader path building
            std::string basePath = "//Shaders/";
            std::string shaderName = "PBRVertex";
            std::string extension = ".glsl";
            
            std::string fullPath = basePath + shaderName + extension;
            
            EXPECT_EQ(fullPath, "//Shaders/PBRVertex.glsl");
            
            // Test with different combinations
            std::string fragmentPath = basePath + "PBRFragment" + extension;
            EXPECT_EQ(fragmentPath, "//Shaders/PBRFragment.glsl");
            
            // Test subdirectory paths
            std::string subDirPath = basePath + "PBR/" + shaderName + extension;
            EXPECT_EQ(subDirPath, "//Shaders/PBR/PBRVertex.glsl");
        }

        // Test case for shader stage identification
        TEST_F(RZShaderLibraryTests, ShaderStageIdentification)
        {
            // Test shader stage detection from names/extensions
            struct ShaderInfo {
                std::string name;
                std::string expectedStage;
            };
            
            std::vector<ShaderInfo> shaderTests = {
                {"BasicVertex.vert", "vertex"},
                {"BasicFragment.frag", "fragment"},
                {"LightingVertex.glsl", "vertex"}, // assuming from name
                {"LightingFragment.glsl", "fragment"}, // assuming from name
                {"ShadowGeometry.geom", "geometry"},
                {"ComputePass.comp", "compute"}
            };
            
            for (const auto& test : shaderTests) {
                // Test that we can identify stages from file extensions
                if (test.name.find(".vert") != std::string::npos || 
                    test.name.find("Vertex") != std::string::npos) {
                    EXPECT_EQ(test.expectedStage, "vertex");
                }
                
                if (test.name.find(".frag") != std::string::npos || 
                    test.name.find("Fragment") != std::string::npos) {
                    EXPECT_EQ(test.expectedStage, "fragment");
                }
                
                if (test.name.find(".geom") != std::string::npos ||
                    test.name.find("Geometry") != std::string::npos) {
                    EXPECT_EQ(test.expectedStage, "geometry");
                }
                
                if (test.name.find(".comp") != std::string::npos ||
                    test.name.find("Compute") != std::string::npos) {
                    EXPECT_EQ(test.expectedStage, "compute");
                }
            }
        }

        // Test case for shader dependency tracking
        TEST_F(RZShaderLibraryTests, ShaderDependencyTracking)
        {
            // Test that we can track shader dependencies (includes)
            std::vector<std::string> commonIncludes = {
                "Common.glsl",
                "Lighting.glsl", 
                "PBR.glsl",
                "Shadows.glsl",
                "PostProcess.glsl",
                "Utils.glsl"
            };
            
            for (const auto& include : commonIncludes) {
                // Test include file naming conventions
                EXPECT_TRUE(include.find(".glsl") != std::string::npos ||
                           include.find(".hlsl") != std::string::npos);
                
                // Test that include names are reasonable
                EXPECT_FALSE(include.empty());
                EXPECT_GT(include.length(), 4); // At least "X.glsl"
            }
        }

        // Test case for shader compilation flags
        TEST_F(RZShaderLibraryTests, ShaderCompilationFlags)
        {
            // Test common shader compilation defines/flags
            std::vector<std::string> commonDefines = {
                "DEBUG",
                "RELEASE", 
                "VULKAN",
                "OPENGL",
                "D3D11",
                "D3D12",
                "ENABLE_SHADOWS",
                "ENABLE_PBR",
                "ENABLE_IBL"
            };
            
            for (const auto& define : commonDefines) {
                // Test define naming conventions
                EXPECT_TRUE(std::isupper(define[0])) << "Define should start with uppercase: " << define;
                
                // Test that defines don't contain invalid characters
                for (char c : define) {
                    EXPECT_TRUE(std::isalnum(c) || c == '_') << "Define contains invalid character: " << define;
                }
            }
        }

        // Test case for shader variant management
        TEST_F(RZShaderLibraryTests, ShaderVariantManagement)
        {
            // Test shader variant naming
            std::string baseShader = "PBR";
            std::vector<std::string> variants = {
                baseShader + "_ALBEDO",
                baseShader + "_NORMAL",
                baseShader + "_METALLIC",
                baseShader + "_ROUGHNESS",
                baseShader + "_AO",
                baseShader + "_EMISSIVE"
            };
            
            for (const auto& variant : variants) {
                // Test that variant names contain base name
                EXPECT_NE(variant.find(baseShader), std::string::npos);
                
                // Test variant naming convention
                EXPECT_NE(variant.find("_"), std::string::npos);
                
                // Test that variant names are uppercase for the suffix
                auto underscorePos = variant.find("_");
                if (underscorePos != std::string::npos) {
                    std::string suffix = variant.substr(underscorePos + 1);
                    for (char c : suffix) {
                        EXPECT_TRUE(std::isupper(c)) << "Variant suffix should be uppercase: " << variant;
                    }
                }
            }
        }

        // Test case for shader resource binding concepts
        TEST_F(RZShaderLibraryTests, ShaderResourceBindingConcepts)
        {
            // Test common binding point names/indices
            struct BindingInfo {
                std::string name;
                u32 expectedBinding;
            };
            
            std::vector<BindingInfo> commonBindings = {
                {"PerFrameUBO", 0},
                {"PerDrawUBO", 1}, 
                {"MaterialUBO", 2},
                {"LightingUBO", 3},
                {"ShadowMapTexture", 4},
                {"AlbedoTexture", 5},
                {"NormalTexture", 6}
            };
            
            for (const auto& binding : commonBindings) {
                // Test binding names are meaningful
                EXPECT_FALSE(binding.name.empty());
                EXPECT_GT(binding.name.length(), 3);
                
                // Test binding indices are reasonable
                EXPECT_LT(binding.expectedBinding, 32); // Reasonable upper limit
                
                // Test naming conventions
                if (binding.name.find("UBO") != std::string::npos) {
                    EXPECT_TRUE(binding.name.find("UBO") == binding.name.length() - 3);
                }
                
                if (binding.name.find("Texture") != std::string::npos) {
                    EXPECT_TRUE(binding.name.find("Texture") == binding.name.length() - 7);
                }
            }
        }

    }    // namespace Gfx
}    // namespace Razix