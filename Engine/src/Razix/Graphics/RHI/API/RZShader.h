#pragma once

#include "Razix/Graphics/Resources/IRZResource.h"

#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"
#include "Razix/Graphics/RHI/API/RZVertexBufferLayout.h"

namespace Razix {
    namespace Graphics {

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

        /* The stage which the shader corresponds to in the graphics pipeline */
        enum class ShaderStage
        {
            NONE = -1,
            Vertex,
            Pixel,
            Compute,
            Geometry,
            TCS,
            TES,
            COUNT
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
            RAZIX_VIRTUAL_DESCTURCTOR(RZShader)
            /* Unless we have variants we don't want it to be copied around */
            RAZIX_NONCOPYABLE_CLASS(RZShader)

            GET_INSTANCE_SIZE;

            /**
             * Parse RZSF shaders into individual API shader modules
             */
            static std::map<ShaderStage, std::string> ParseRZSF(const std::string& filePath);

            /* Bind the shader to the pipeline */
            virtual void Bind() const = 0;
            /* Unbind the shader from the pipeline */
            virtual void Unbind() const = 0;
            /* Cross compile shaders from src type to current API shader language */
            virtual void CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType) = 0;
            /* Generates descriptor sets, useful when resize events or shader reload occurs */
            virtual void GenerateDescriptorHeaps() = 0;

            /* Gets the name of the shader file */
            RAZIX_INLINE const std::string& getName() { return m_Desc.name; }
            /* Gets the stage of the pipeline that shader is bound/being used with */
            RAZIX_INLINE const ShaderStage& getStage() { return m_ShaderStage; }
            /* Gets the input stride of the vertex layout */
            RAZIX_INLINE const u32& getInputStride() const { return m_VertexInputStride; }
            /* Gets per set descriptors info */
            RAZIX_INLINE DescriptorsPerHeapMap getDescriptorsPerHeapMap() { return m_DescriptorsPerHeap; }
            /* Gets the push constants in the shader */
            RAZIX_INLINE std::vector<RZPushConstant>& getPushConstants() { return m_PushConstants; }
            /* Gets the scene draw parameters */
            RAZIX_INLINE const SceneDrawParams& getSceneDrawParams() { return m_SceneParams; }
            RAZIX_INLINE void                   overrideSceneDrawParams(SceneDrawParams& sceneDrawParams) { m_SceneParams = sceneDrawParams; }
            /* Gets the bind variables for shader */
            RAZIX_INLINE ShaderBindVars& getBindVars() { return m_BindVars; }
            RAZIX_INLINE void            setBindVars(ShaderBindVars& vars) { m_BindVars = vars; }
            /* Updates the descriptor sets with the bind variables aka filled descriptors */
            void updateBindVarsHeaps();

            RAZIX_INLINE std::string getShaderFilePath() const { return m_Desc.filePath; }
            RAZIX_INLINE void        setShaderFilePath(std::string val) { m_Desc.filePath = val; }
            // TODO: Expose internal Vertex Attributes and Layout functions in a engine wide style

        protected:
            RZShaderDesc                       m_Desc        = {};
            ShaderStage                        m_ShaderStage = ShaderStage::NONE;       /* The shader stage to which the shader will be bound to                                                    */
            ShaderSourceType                   m_SourceType  = ShaderSourceType::SPIRV; /* The source type of the shader                                                                            */
            std::map<ShaderStage, std::string> m_ParsedRZSF;                            /* The razix shader file that was parsed                                                                    */
            RZVertexBufferLayout               m_BufferLayout;                          /* Detailed description of the input data format of the vertex buffer that has been extracted from shader   */
            DescriptorsPerHeapMap              m_DescriptorsPerHeap;                    /* Encapsulates the descriptors corresponding to a set with binding and resource information                */
            ShaderBindVars                     m_BindVars;                              /* Descriptors and name maps for updating descriptors                                                       */
            SceneDrawParams                    m_SceneParams;                           /* Some params to help with scene drawing                                                                   */
            std::vector<RZPushConstant>        m_PushConstants;                         /* The list of the the push constants                                                                       */
            u32                                m_VertexInputStride = 0;                 /* The stride of the vertex data that is extracted from the information                                     */

        private:
            /**
             * Creates a shader resource from the given file
             * 
             * @param filePath The Virtual/absolute path to the compiled binary shader file
             */
            static void Create(void* where, const RZShaderDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);

            friend class RZResourceManager;
        };
    }    // namespace Graphics
}    // namespace Razix
