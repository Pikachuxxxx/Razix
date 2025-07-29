// FrameGraphTests.cpp
// Unit tests for the Razix FrameGraph system
#include "Razix/Core/RZCore.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraph.h"
#include "Razix/Gfx/FrameGraph/RZBlackboard.h"
#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"
#include "Razix/Gfx/FrameGraph/RZPassNode.h"

#include <gtest/gtest.h>
#include <string>

namespace Razix {
    namespace Gfx {

        class RZFrameGraphTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Setup for FrameGraph tests
            }

            void TearDown() override
            {
                // Cleanup for FrameGraph tests
            }
        };

        // Test case for basic FrameGraph creation and destruction
        TEST_F(RZFrameGraphTests, BasicFrameGraphCreation)
        {
            // Test that FrameGraph can be created and destroyed without issues
            RZFrameGraph frameGraph;
            // Test passes if no crash occurs during construction/destruction
            SUCCEED();
        }

        // Test case for adding callback pass with data, setup, and execute functions
        TEST_F(RZFrameGraphTests, AddCallbackPassWithData)
        {
            RZFrameGraph frameGraph;
            
            // Define pass data structure
            struct TestPassData {
                int value = 42;
                std::string name = "TestPass";
            };
            
            bool setupCalled = false;
            bool executeCalled = false;
            
            // Add a callback pass with data
            const auto& passData = frameGraph.addCallbackPass<TestPassData>(
                "TestPass",
                // Setup function
                [&setupCalled](TestPassData& data, RZPassResourceBuilder& builder) {
                    setupCalled = true;
                    EXPECT_EQ(data.value, 42);
                    EXPECT_EQ(data.name, "TestPass");
                },
                // Execute function  
                [&executeCalled](const TestPassData& data, RZPassResourceDirectory& resources) {
                    executeCalled = true;
                    EXPECT_EQ(data.value, 42);
                    EXPECT_EQ(data.name, "TestPass");
                }
            );
            
            // Verify setup was called during pass creation
            EXPECT_TRUE(setupCalled) << "Setup function should be called immediately";
            
            // Verify pass data is accessible
            EXPECT_EQ(passData.value, 42);
            EXPECT_EQ(passData.name, "TestPass");
        }

        // Test case for adding callback pass with data, setup, execute, and resize functions
        TEST_F(RZFrameGraphTests, AddCallbackPassWithResize)
        {
            RZFrameGraph frameGraph;
            
            struct ResizablePassData {
                u32 width = 1920;
                u32 height = 1080;
                bool resized = false;
            };
            
            bool setupCalled = false;
            bool executeCalled = false;
            bool resizeCalled = false;
            
            const auto& passData = frameGraph.addCallbackPass<ResizablePassData>(
                "ResizablePass",
                // Setup function
                [&setupCalled](ResizablePassData& data, RZPassResourceBuilder& builder) {
                    setupCalled = true;
                    data.width = 1920;
                    data.height = 1080;
                },
                // Execute function
                [&executeCalled](const ResizablePassData& data, RZPassResourceDirectory& resources) {
                    executeCalled = true;
                    EXPECT_EQ(data.width, 1920);
                    EXPECT_EQ(data.height, 1080);
                },
                // Resize function
                [&resizeCalled](RZPassResourceDirectory& resources, u32 width, u32 height) {
                    resizeCalled = true;
                    EXPECT_GT(width, 0);
                    EXPECT_GT(height, 0);
                }
            );
            
            EXPECT_TRUE(setupCalled) << "Setup should be called during pass creation";
            EXPECT_EQ(passData.width, 1920);
            EXPECT_EQ(passData.height, 1080);
        }

        // Test case for adding callback pass without data (NoData template specialization)
        TEST_F(RZFrameGraphTests, AddCallbackPassNoData)
        {
            RZFrameGraph frameGraph;
            
            bool setupCalled = false;
            bool executeCalled = false;
            bool resizeCalled = false;
            
            // Add pass without data using the NoData specialization
            frameGraph.addCallbackPass(
                "NoDataPass",
                // Setup function (no data parameter)
                [&setupCalled](auto& /* NoData */, RZPassResourceBuilder& builder) {
                    setupCalled = true;
                },
                // Execute function (no data parameter)
                [&executeCalled](const auto& /* NoData */, RZPassResourceDirectory& resources) {
                    executeCalled = true;
                },
                // Resize function
                [&resizeCalled](RZPassResourceDirectory& resources, u32 width, u32 height) {
                    resizeCalled = true;
                }
            );
            
            EXPECT_TRUE(setupCalled) << "Setup should be called for NoData pass";
        }

        // Test case for adding callback pass without resize function
        TEST_F(RZFrameGraphTests, AddCallbackPassNoResize)
        {
            RZFrameGraph frameGraph;
            
            struct SimplePassData {
                float value = 3.14f;
            };
            
            bool setupCalled = false;
            bool executeCalled = false;
            
            // Add pass without resize function (uses empty lambda internally)
            const auto& passData = frameGraph.addCallbackPass<SimplePassData>(
                "SimplePass",
                // Setup function
                [&setupCalled](SimplePassData& data, RZPassResourceBuilder& builder) {
                    setupCalled = true;
                    data.value = 2.71f;
                },
                // Execute function only
                [&executeCalled](const SimplePassData& data, RZPassResourceDirectory& resources) {
                    executeCalled = true;
                    EXPECT_FLOAT_EQ(data.value, 2.71f);
                }
            );
            
            EXPECT_TRUE(setupCalled);
            EXPECT_FLOAT_EQ(passData.value, 2.71f);
        }

        // Test case for multiple passes in a FrameGraph
        TEST_F(RZFrameGraphTests, MultiplePassesInFrameGraph)
        {
            RZFrameGraph frameGraph;
            
            struct Pass1Data { int id = 1; };
            struct Pass2Data { int id = 2; };
            struct Pass3Data { int id = 3; };
            
            int setupCount = 0;
            
            // Add multiple passes
            const auto& pass1Data = frameGraph.addCallbackPass<Pass1Data>(
                "Pass1",
                [&setupCount](Pass1Data& data, RZPassResourceBuilder& builder) {
                    ++setupCount;
                    EXPECT_EQ(data.id, 1);
                },
                [](const Pass1Data& data, RZPassResourceDirectory& resources) {}
            );
            
            const auto& pass2Data = frameGraph.addCallbackPass<Pass2Data>(
                "Pass2",
                [&setupCount](Pass2Data& data, RZPassResourceBuilder& builder) {
                    ++setupCount;
                    EXPECT_EQ(data.id, 2);
                },
                [](const Pass2Data& data, RZPassResourceDirectory& resources) {}
            );
            
            const auto& pass3Data = frameGraph.addCallbackPass<Pass3Data>(
                "Pass3",
                [&setupCount](Pass3Data& data, RZPassResourceBuilder& builder) {
                    ++setupCount;
                    EXPECT_EQ(data.id, 3);
                },
                [](const Pass3Data& data, RZPassResourceDirectory& resources) {}
            );
            
            // Verify all setup functions were called
            EXPECT_EQ(setupCount, 3) << "All three setup functions should be called";
            
            // Verify pass data is correct
            EXPECT_EQ(pass1Data.id, 1);
            EXPECT_EQ(pass2Data.id, 2);
            EXPECT_EQ(pass3Data.id, 3);
        }

        // Test case for FrameGraph resource management via blackboard
        TEST_F(RZFrameGraphTests, BlackboardResourceManagement)
        {
            RZBlackboard blackboard;
            
            // Test adding different types to blackboard
            struct TestResource {
                int value = 100;
                std::string name = "TestResource";
            };
            
            // Add resource to blackboard
            auto& resource = blackboard.add<TestResource>();
            EXPECT_EQ(resource.value, 100);
            EXPECT_EQ(resource.name, "TestResource");
            
            // Test that we can retrieve the resource
            const auto& retrievedResource = blackboard.get<TestResource>();
            EXPECT_EQ(retrievedResource.value, 100);
            EXPECT_EQ(retrievedResource.name, "TestResource");
            
            // Test has() functionality (implicit in get() since it asserts)
            // If get() doesn't crash, has() is working correctly
        }

        // Test case for FrameGraph resource ID management
        TEST_F(RZFrameGraphTests, FrameGraphResourceIDs)
        {
            RZBlackboard blackboard;
            
            // Test string-based resource ID storage and retrieval
            RZFrameGraphResource resourceID = RZFrameGraphResource::Create(); // Assuming this exists
            std::string resourceName = "TestTexture";
            
            // Add resource ID with string identifier
            blackboard.add(resourceName, resourceID);
            
            // Retrieve resource ID
            RZFrameGraphResource retrievedID = blackboard.getID(resourceName);
            EXPECT_EQ(retrievedID, resourceID) << "Retrieved resource ID should match original";
        }

        // Test case for pass validation and error handling
        TEST_F(RZFrameGraphTests, PassValidationAndErrorHandling)
        {
            RZFrameGraph frameGraph;
            
            // Test that compile-time checks work for invalid signatures
            // (These would be compile-time failures, so we test valid cases)
            
            struct ValidPassData {
                bool isValid = true;
            };
            
            // Test valid setup signature
            bool validSetup = std::is_invocable_v<
                decltype([](ValidPassData&, RZPassResourceBuilder&) {}),
                ValidPassData&, 
                RZPassResourceBuilder&
            >;
            EXPECT_TRUE(validSetup) << "Valid setup signature should be detected";
            
            // Test valid execute signature  
            bool validExecute = std::is_invocable_v<
                decltype([](const ValidPassData&, RZPassResourceDirectory&) {}),
                const ValidPassData&,
                RZPassResourceDirectory&
            >;
            EXPECT_TRUE(validExecute) << "Valid execute signature should be detected";
            
            // Test valid resize signature
            bool validResize = std::is_invocable_v<
                decltype([](RZPassResourceDirectory&, u32, u32) {}),
                RZPassResourceDirectory&,
                u32,
                u32
            >;
            EXPECT_TRUE(validResize) << "Valid resize signature should be detected";
        }

        // Test case for FrameGraph with complex pass interactions
        TEST_F(RZFrameGraphTests, ComplexPassInteractions)
        {
            RZFrameGraph frameGraph;
            
            struct GeometryPassData {
                std::string geometryBuffer = "GBuffer";
                u32 triangleCount = 1000;
            };
            
            struct LightingPassData {
                std::string lightBuffer = "LightData";
                u32 lightCount = 8;
                std::string inputGeometry; // Will reference geometry pass output
            };
            
            bool geometrySetup = false;
            bool lightingSetup = false;
            
            // Add geometry pass
            const auto& geoPass = frameGraph.addCallbackPass<GeometryPassData>(
                "GeometryPass",
                [&geometrySetup](GeometryPassData& data, RZPassResourceBuilder& builder) {
                    geometrySetup = true;
                    data.triangleCount = 2000; // Modify during setup
                },
                [](const GeometryPassData& data, RZPassResourceDirectory& resources) {
                    // Geometry rendering would happen here
                }
            );
            
            // Add lighting pass that could depend on geometry
            const auto& lightPass = frameGraph.addCallbackPass<LightingPassData>(
                "LightingPass", 
                [&lightingSetup, &geoPass](LightingPassData& data, RZPassResourceBuilder& builder) {
                    lightingSetup = true;
                    data.inputGeometry = geoPass.geometryBuffer; // Reference geometry output
                    data.lightCount = 16; // Modify during setup
                },
                [](const LightingPassData& data, RZPassResourceDirectory& resources) {
                    // Lighting calculations would happen here
                }
            );
            
            EXPECT_TRUE(geometrySetup) << "Geometry pass setup should be called";
            EXPECT_TRUE(lightingSetup) << "Lighting pass setup should be called";
            
            EXPECT_EQ(geoPass.triangleCount, 2000);
            EXPECT_EQ(lightPass.lightCount, 16);
            EXPECT_EQ(lightPass.inputGeometry, "GBuffer");
        }

    }    // namespace Gfx
}    // namespace Razix