#pragma once

#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"
#include "Razix/Graphics/RHI/API/RZVertexBufferLayout.h"

namespace Razix {
    namespace Graphics {

        /* The stage which the shader corresponds to in the graphics pipeline */
        enum class ShaderStage 
        {
            NONE = -1,
            VERTEX,
            PIXEL,
            GEOMETRY,
            TCS,
            TES,
            COMPUTE
        };

        /* Used the engine to find the right shader cache based on shader file name, forward declaring future API feature */
        enum class ShaderSourceType
        {
            NONE,
            GLSL,
            SPIRV,
            HLSL,
            PSSL,
            CG
        };

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
        class RAZIX_API RZShader : public RZRoot
        {
        public:
            RZShader() {}
            virtual ~RZShader() {}

            /**
             * Creates a shader resource from the given file
             * 
             * @param filePath The Virtual/absolute path to the compiled binary shader file
             */
            static RZShader* Create(const std::string& filePath RZ_DEBUG_NAME_TAG_E_ARG);

            virtual void Bind() const                                                                                     = 0;
            virtual void Unbind() const                                                                                   = 0;
            virtual void CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType) = 0;
            virtual void Destroy()                                                                                        = 0;

            static std::map<ShaderStage, std::string> ParseRZSF(const std::string& filePath);

            inline const std::string& getName() { return m_Name; }
            /* Gets the stage of the pipeline that shader is bound/being used with */
            inline const ShaderStage&           getStage() { return m_ShaderStage; }
            inline const uint32_t&              getInputStride() const { return m_VertexInputStride; }
            inline DescriptorSetsCreateInfos    getSetsCreateInfos() { return m_DescriptorSetsCreateInfos; }
            inline std::vector<RZPushConstant>& getPushConstants() { return m_PushConstants; }

        protected:
            ShaderStage                        m_ShaderStage = ShaderStage::NONE;       /* The shader stage to which the shader will be bound to                                                    */
            ShaderSourceType                   m_SourceType  = ShaderSourceType::SPIRV; /* The source type of the shader                                                                            */
            std::string                        m_ShaderFilePath;                        /* Virtual file location of the shader file                                                                 */
            std::string                        m_Name;                                  /* The name of the shader                                                                                   */
            std::map<ShaderStage, std::string> m_ParsedRZSF;                            /* The razix shader file that was parsed                                                                    */
            RZVertexBufferLayout               m_BufferLayout;                          /* Detailed description of the input data format of the vertex buffer that has been extracted from shader   */
            DescriptorSetsCreateInfos          m_DescriptorSetsCreateInfos;             /* Encapsulates the descriptors corresponding to a set with binding and resource information                */
            std::vector<RZPushConstant>        m_PushConstants;                         /* The list of the the push constants                                                                       */
            uint32_t                           m_VertexInputStride = 0;                 /* The stride of the vertex data that is extracted from the information                                     */
        };
    }    // namespace Graphics
}    // namespace Razix
