#pragma once

#include <spirv_reflect.h>
#include <map>

#include "Razix/Graphics/API/RZDescriptorSet.h"

namespace Razix {
    namespace Graphics {

        /* The stage which the shader corresponds to in the graphics pipeline */
        enum class ShaderStage : int32_t
        {
            NONE = -1, VERTEX, PIXEL, GEOMETRY, TCS, TES, COMPUTE
        };

        /* Used the engine to find the right shader cache based on shader file name, forward declaring future API feature */
        enum class ShaderSourceType
        {
            NONE, GLSL, SPIRV, HLSL, PSSL, CG
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
        class RAZIX_API RZShader
        {
        public:
            RZShader() { }
            virtual ~RZShader() {};

            /**
             * Creates a shader resource from the given file
             * 
             * @param filePath The Virtual/absolute path to the compiled binary shader file
             */
            static RZShader* Create(const std::string& filePath);

            virtual void Bind() const = 0;
            virtual void Unbind() const = 0;
            virtual void CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType) = 0;

            static std::map<ShaderStage, std::string> ParseRZSF(const std::string& filePath);

            /* Gets the stage of the pipeline that shader is bound/being used with */
            inline const ShaderStage& getStage() { return m_ShaderStage; }

        protected:
            ShaderStage                         m_ShaderStage = ShaderStage::NONE;          /* The shader stage to which the shader will be bound to    */
            ShaderSourceType                    m_SourceType = ShaderSourceType::SPIRV;     /* The source type of the shader                            */
            std::string                         m_ShaderFilePath;                           /* Virtual file location of the shader file                 */
            std::string                         m_Name;                                     /* The name of the shader                                   */
            std::map<ShaderStage, std::string>  m_ParsedRZSF;                               /* The razix shader file that was parsed                    */
        };
    
    } 
}

