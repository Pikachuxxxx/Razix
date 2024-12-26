#pragma once

#include "Razix/Gfx/Resources/IRZResource.h"

#include "Razix/Gfx/RHI/API/RZBufferLayout.h"
#include "Razix/Gfx/RHI/API/RZDescriptorSet.h"

namespace Razix {
    namespace Gfx {

#define PUSH_CONSTANT_REFLECTION_NAME_PREFIX "PushConstant"
#define PUSH_CONSTANT_REFLECTION_NAME_VK     PUSH_CONSTANT_REFLECTION_NAME_PREFIX
#define PUSH_CONSTANT_REFLECTION_NAME_DX12   "PushConstantBuffer"

        struct SceneDrawParams
        {
            bool           enableFrameData              = false;
            bool           enableMaterials              = false;
            bool           enableLights                 = false;
            bool           enableBindlessTextures       = false;
            DescriptorSets userSets                     = {};
            void*          overridePushConstantData     = nullptr;
            u32            overridePushConstantDataSize = 0;
            // TODO: Add support for Pixel PC data
        };

        class RZShaderLibrary;
        enum class ShaderBuiltin : u32;

        // This is the only exception of using enum
        /* The stage which the shader corresponds to in the graphics pipeline */
        enum ShaderStage : u32
        {
            kNone                  = 0,
            kVertex                = 1 << 0,
            kPixel                 = 1 << 1,
            kCompute               = 1 << 2,
            kGeometry              = 1 << 3,
            kTesselationControl    = 1 << 4,
            kTesselationEvaluation = 1 << 5,
            COUNT                  = 6
        };

        /* Used the engine to find the right shader cache based on shader file name, forward declaring future API feature */
        enum class ShaderSourceType
        {
            NONE = -1,
            GLSL,
            SPIRV,
            HLSL,
            PSSL,
            CG
        };

        struct ShaderBindVars
        {
            std::unordered_map<std::string, RZDescriptor*> m_BindMap; /* Holds the references to the descriptors in m_DescriptorsPerHeap using a string map */

            RZDescriptor* operator[](const std::string& name)
            {
                if (m_BindMap.find(name) != m_BindMap.end())
                    return m_BindMap[name];
                else
                    return nullptr;
            }
        };

        static std::unordered_map<ShaderStage, const char*> g_ShaderStageEntryPointNameMap = {
            {ShaderStage::kVertex, "VS_MAIN"},
            {ShaderStage::kPixel, "PS_MAIN"},
            {ShaderStage::kCompute, "CS_MAIN"},
            {ShaderStage::kGeometry, "GS_MAIN"}};

        /* 
         * Razix Shader that will be passed to the GPU at various stages 
         * 
         * @Note: As of now since the engine doesn't support shader cross-compilation nor an extensive shader detection API
         * all shaders are provided in SPIR-V form and reflected from it only (for OpenGL and Vulkan), hlsl maybe used for 
         * D3D12 if the support is added, in that case only the shader name is provided in later API so that the engine 
         * can search on it's own for the appropriate shader cache based on the render API being used to resolve and reflect
         * 
         * @Note: Also the shader takes pre-compiled binary blobs only to reflect as of now in later revision the engine
         * can compile and cache and do all the fookin possible cases on it's own in the most efficient cache-friendly
         * and also serialize reflection data for faster loading
         */
        class RAZIX_API RZShader : public IRZResource<RZShader>
        {
            friend class RZShaderLibrary;

        public:
            RZShader() {}
            /* Virtual destructor enables the API implementation to delete it's resources */
            RAZIX_VIRTUAL_DESCTURCTOR(RZShader);
            /* Unless we have variants we don't want it to be copied around */
            RAZIX_NONCOPYABLE_CLASS(RZShader);

            GET_INSTANCE_SIZE;

            /**
             * Parse RZSF shaders into individual API shader modules
             */
            static std::map<ShaderStage, std::string> ParseRZSF(const std::string& filePath);

            /* Generates user descriptor sets that are not created/bound by the engine implicitly */
            virtual void GenerateUserDescriptorHeaps() = 0;

            inline const std::string&    getName() const { return m_Desc.name; }
            inline const u32&            getInputStride() const { return m_VertexInputStride; }
            inline DescriptorsPerHeapMap getDescriptorsPerHeapMap() { return m_DescriptorsPerHeap; }
            // TODO: Instead of one PushConstant per entire shader, have one per shader stage instead, implement it when it comes to that
            inline std::vector<RZPushConstant>& getPushConstants(ShaderStage stage) { return m_PushConstants; }
            inline const SceneDrawParams&       getSceneDrawParams() { return m_SceneParams; }
            inline void                         overrideSceneDrawParams(SceneDrawParams& sceneDrawParams) { m_SceneParams = sceneDrawParams; }
            inline ShaderBindVars&              getBindVars() { return m_BindVars; }
            inline void                         setBindVars(ShaderBindVars& vars) { m_BindVars = vars; }
            inline std::string                  getShaderFilePath() const { return m_Desc.filePath; }

            /* Updates the descriptor sets with the bind variables aka filled descriptors */
            void updateBindVarsHeaps();

        protected:
            RZShaderDesc                       m_Desc       = {};
            ShaderSourceType                   m_SourceType = ShaderSourceType::SPIRV;
            std::map<ShaderStage, std::string> m_ParsedRZSF;
            RZBufferLayout                     m_BufferLayout;
            DescriptorsPerHeapMap              m_DescriptorsPerHeap;
            ShaderBindVars                     m_BindVars;
            SceneDrawParams                    m_SceneParams;
            std::vector<RZPushConstant>        m_PushConstants;
            u32                                m_VertexInputStride = 0;

        private:
            /**
             * Creates a shader resource from the given file
             * 
             * @param filePath The Virtual/absolute path to the compiled binary shader file
             */
            static void Create(void* where, const RZShaderDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            friend class RZResourceManager;
        };
    }    // namespace Gfx
}    // namespace Razix
