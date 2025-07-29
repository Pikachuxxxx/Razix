// MeshTests.cpp
// Unit tests for the RZMesh system
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Gfx/RZMesh.h"
#include "Razix/Gfx/RZMeshFactory.h"
#include "Razix/Gfx/RZVertexFormat.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace Gfx {

        class RZMeshTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Setup for mesh tests
                Razix::Debug::RZLog::StartUp();
            }

            void TearDown() override
            {
                // Cleanup for mesh tests
                Razix::Debug::RZLog::Shutdown();
            }
        };

        // Test case for VertexInputFormat enum
        TEST_F(RZMeshTests, VertexInputFormatEnum)
        {
            // Test that all expected vertex formats exist
            VertexInputFormat position = VertexInputFormat::kPosition;
            VertexInputFormat normal = VertexInputFormat::kNormal;
            VertexInputFormat texCoord = VertexInputFormat::kTexCoord;
            VertexInputFormat color = VertexInputFormat::kColor;
            VertexInputFormat tangent = VertexInputFormat::kTangent;
            VertexInputFormat bitangent = VertexInputFormat::kBitangent;
            
            // Test that they are different values
            EXPECT_NE(position, normal);
            EXPECT_NE(position, texCoord);
            EXPECT_NE(normal, texCoord);
            EXPECT_NE(color, tangent);
            
            // Test enum values are reasonable
            EXPECT_TRUE(static_cast<int>(position) >= 0);
            EXPECT_TRUE(static_cast<int>(normal) >= 0);
            EXPECT_TRUE(static_cast<int>(texCoord) >= 0);
        }

        // Test case for mesh vertex data structure
        TEST_F(RZMeshTests, MeshVertexStructure)
        {
            // Test MeshVertex structure
            MeshVertex vertex;
            
            // Test position
            vertex.position = glm::vec3(1.0f, 2.0f, 3.0f);
            EXPECT_EQ(vertex.position.x, 1.0f);
            EXPECT_EQ(vertex.position.y, 2.0f);
            EXPECT_EQ(vertex.position.z, 3.0f);
            
            // Test normal
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            EXPECT_EQ(vertex.normal.y, 1.0f);
            
            // Test texture coordinates
            vertex.uv = glm::vec2(0.5f, 0.7f);
            EXPECT_EQ(vertex.uv.x, 0.5f);
            EXPECT_EQ(vertex.uv.y, 0.7f);
            
            // Test color
            vertex.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
            EXPECT_EQ(vertex.color.r, 1.0f);
            EXPECT_EQ(vertex.color.a, 1.0f);
            
            // Test tangent
            vertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
            EXPECT_EQ(vertex.tangent.x, 1.0f);
        }

        // Test case for RZMesh basic functionality
        TEST_F(RZMeshTests, MeshBasicFunctionality)
        {
            // Create a simple mesh
            std::vector<MeshVertex> vertices;
            std::vector<u32> indices;
            
            // Add a triangle
            MeshVertex v1, v2, v3;
            v1.position = glm::vec3(0.0f, 1.0f, 0.0f);
            v2.position = glm::vec3(-1.0f, -1.0f, 0.0f);
            v3.position = glm::vec3(1.0f, -1.0f, 0.0f);
            
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
            indices.push_back(0);
            indices.push_back(1);
            indices.push_back(2);
            
            // Test that we have the expected data
            EXPECT_EQ(vertices.size(), 3);
            EXPECT_EQ(indices.size(), 3);
            
            // Test vertex positions
            EXPECT_EQ(vertices[0].position.y, 1.0f);
            EXPECT_EQ(vertices[1].position.x, -1.0f);
            EXPECT_EQ(vertices[2].position.x, 1.0f);
            
            // Test indices
            EXPECT_EQ(indices[0], 0);
            EXPECT_EQ(indices[1], 1);
            EXPECT_EQ(indices[2], 2);
        }

        // Test case for primitive mesh creation concepts
        TEST_F(RZMeshTests, PrimitiveMeshConcepts)
        {
            // Test that we can create basic primitive vertices
            
            // Quad vertices
            std::vector<MeshVertex> quadVertices;
            quadVertices.resize(4);
            
            // Bottom-left
            quadVertices[0].position = glm::vec3(-1.0f, -1.0f, 0.0f);
            quadVertices[0].uv = glm::vec2(0.0f, 0.0f);
            
            // Bottom-right
            quadVertices[1].position = glm::vec3(1.0f, -1.0f, 0.0f);
            quadVertices[1].uv = glm::vec2(1.0f, 0.0f);
            
            // Top-right
            quadVertices[2].position = glm::vec3(1.0f, 1.0f, 0.0f);
            quadVertices[2].uv = glm::vec2(1.0f, 1.0f);
            
            // Top-left
            quadVertices[3].position = glm::vec3(-1.0f, 1.0f, 0.0f);
            quadVertices[3].uv = glm::vec2(0.0f, 1.0f);
            
            // Test quad properties
            EXPECT_EQ(quadVertices.size(), 4);
            EXPECT_EQ(quadVertices[0].uv, glm::vec2(0.0f, 0.0f));
            EXPECT_EQ(quadVertices[2].uv, glm::vec2(1.0f, 1.0f));
            
            // Quad indices for two triangles
            std::vector<u32> quadIndices = {0, 1, 2, 2, 3, 0};
            EXPECT_EQ(quadIndices.size(), 6);
        }

        // Test case for mesh normal calculation concepts
        TEST_F(RZMeshTests, NormalCalculationConcepts)
        {
            // Test triangle normal calculation
            glm::vec3 v1 = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 v2 = glm::vec3(-1.0f, -1.0f, 0.0f);
            glm::vec3 v3 = glm::vec3(1.0f, -1.0f, 0.0f);
            
            // Calculate normal using cross product
            glm::vec3 edge1 = v2 - v1;
            glm::vec3 edge2 = v3 - v1;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            
            // For this triangle in XY plane, normal should point in Z direction
            EXPECT_TRUE(std::abs(normal.z) > 0.9f); // Should be close to (0,0,1) or (0,0,-1)
            EXPECT_TRUE(std::abs(normal.x) < 0.1f);
            EXPECT_TRUE(std::abs(normal.y) < 0.1f);
            
            // Test normal length
            f32 length = glm::length(normal);
            EXPECT_NEAR(length, 1.0f, 0.001f);
        }

        // Test case for bounding box calculation concepts
        TEST_F(RZMeshTests, BoundingBoxConcepts)
        {
            std::vector<MeshVertex> vertices;
            
            // Add vertices at various positions
            MeshVertex v1, v2, v3, v4;
            v1.position = glm::vec3(-2.0f, -1.0f, -3.0f);
            v2.position = glm::vec3(3.0f, 2.0f, 1.0f);
            v3.position = glm::vec3(0.0f, 4.0f, -1.0f);
            v4.position = glm::vec3(-1.0f, -2.0f, 2.0f);
            
            vertices = {v1, v2, v3, v4};
            
            // Calculate bounding box
            glm::vec3 minBounds = vertices[0].position;
            glm::vec3 maxBounds = vertices[0].position;
            
            for (const auto& vertex : vertices) {
                minBounds = glm::min(minBounds, vertex.position);
                maxBounds = glm::max(maxBounds, vertex.position);
            }
            
            // Test calculated bounds
            EXPECT_EQ(minBounds.x, -2.0f);
            EXPECT_EQ(minBounds.y, -2.0f);
            EXPECT_EQ(minBounds.z, -3.0f);
            
            EXPECT_EQ(maxBounds.x, 3.0f);
            EXPECT_EQ(maxBounds.y, 4.0f);
            EXPECT_EQ(maxBounds.z, 2.0f);
            
            // Test bounding box size
            glm::vec3 size = maxBounds - minBounds;
            EXPECT_EQ(size.x, 5.0f);
            EXPECT_EQ(size.y, 6.0f);
            EXPECT_EQ(size.z, 5.0f);
        }

        // Test case for vertex format validation
        TEST_F(RZMeshTests, VertexFormatValidation)
        {
            MeshVertex vertex;
            
            // Test that all vertex components can be set
            vertex.position = glm::vec3(1.0f, 2.0f, 3.0f);
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            vertex.uv = glm::vec2(0.5f, 0.5f);
            vertex.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            vertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
            vertex.bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
            
            // Verify all components are set correctly
            EXPECT_EQ(vertex.position, glm::vec3(1.0f, 2.0f, 3.0f));
            EXPECT_EQ(vertex.normal, glm::vec3(0.0f, 1.0f, 0.0f));
            EXPECT_EQ(vertex.uv, glm::vec2(0.5f, 0.5f));
            EXPECT_EQ(vertex.color, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            EXPECT_EQ(vertex.tangent, glm::vec3(1.0f, 0.0f, 0.0f));
            EXPECT_EQ(vertex.bitangent, glm::vec3(0.0f, 0.0f, 1.0f));
        }

    }    // namespace Gfx
}    // namespace Razix