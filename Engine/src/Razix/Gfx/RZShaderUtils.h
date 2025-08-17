#pragma once

#include "Razix/Gfx/RHI/RHI.h"

namespace Razix {
    namespace Gfx {

        //-----------------------------------------------------------------------------------
        // Shader Reflection Function Pointer Types
        //-----------------------------------------------------------------------------------

        typedef void (*ReflectShaderBlobBackendFn)(const rz_gfx_shader_stage_blob*, rz_gfx_shader_reflection*);

        //-----------------------------------------------------------------------------------
        // Vulkan Shader Reflection
        //-----------------------------------------------------------------------------------

#ifdef RAZIX_RENDER_API_VULKAN

        void VulkanReflectShaderBlob(const rz_gfx_shader_stage_blob* stageBlob, rz_gfx_shader_reflection* outReflection);

#endif    // RAZIX_RENDER_API_VULKAN

        //-----------------------------------------------------------------------------------
        // DirectX 12 Shader Reflection
        //-----------------------------------------------------------------------------------

#ifdef RAZIX_RENDER_API_DIRECTX12

        u32                    DX12GetComponentCount(u32 mask);
        DXGI_FORMAT            DX12GetFormatFromComponent(D3D_REGISTER_COMPONENT_TYPE componentType, UINT componentCount);
        u32                    DX12GetDxgiFormatSize(DXGI_FORMAT format);
        rz_gfx_descriptor_type DX12ConvertInputTypeToDescriptorType(D3D_SHADER_INPUT_TYPE inputType);
        void                   DX12ReflectShaderBlob(const rz_gfx_shader_stage_blob* stageBlob, rz_gfx_shader_reflection* outReflection);

#endif    // RAZIX_RENDER_API_DIRECTX12

        //-----------------------------------------------------------------------------------
        // Common Shader Utilities
        //-----------------------------------------------------------------------------------

        ReflectShaderBlobBackendFn GetShaderReflectionFunction();
        void                       FreeShaderReflectionMemAllocs(rz_gfx_shader_reflection* reflection);
        void                       CopyReflectedRootSigDesc(const rz_gfx_shader_reflection* src, rz_gfx_root_signature_desc* dst);

        //-----------------------------------------------------------------------------------
        // Shader Bind Map for Descriptor table management
        //-----------------------------------------------------------------------------------

        // Bind tables for these descriptors are ignored by the shader bind map
        struct DescriptorBlacklist
        {
            std::string              name;
            std::vector<std::string> blacklistNames;
        };

        static DescriptorBlacklist s_SystemDescriptorsBlacklistPreset    = {"SystemDescriptors (FrameData/SceneLightsData)", {"FrameData", "SceneLightsData"}};
        static DescriptorBlacklist s_MaterialsDescriptorsBlacklistPreset = {"MaterialDescriptors", {"Material", "albedoMap", "normalMap", "metallicMap", "roughnessMap", "specularMap", "emissiveMap", "aoMap"}};

        /**
         * Manages descriptor tables creation and binding alongside reflection data
         */
        class RZShaderBindMap
        {
        public:
            RZShaderBindMap() {}
            ~RZShaderBindMap() {}

            RZShaderBindMap& registerBindMap();
            RZShaderBindMap& createFrom(const rz_gfx_shader_handle& shaderHandle);
            RZShaderBindMap& createFrom(const rz_gfx_shader_reflection& shaderReflection);
            RZShaderBindMap& setResourceView(const rz_gfx_resource_view& resourceView);
            RZShaderBindMap& setResourceView(const rz_gfx_resource_view_handle& resourceViewHandle);
            RZShaderBindMap& setDescriptorTable(const rz_gfx_descriptor_table_handle& descriptorTableHandle);
            RZShaderBindMap& setDescriptorTable(const rz_gfx_descriptor_table& descriptorTable);
            RZShaderBindMap& setDescriptorBlacklist(const DescriptorBlacklist& blacklist);
            RZShaderBindMap& setDescriptorBlacklist(const std::string& name, const std::vector<std::string>& blacklistNames);
            RZShaderBindMap& validate();
            RZShaderBindMap& build();
            RZShaderBindMap& clear();
            RZShaderBindMap& clearBlacklist();
            RZShaderBindMap& destroy();

            void bind(rz_gfx_cmdbuf_handle cmdBufHandle) const;
            void unbind(rz_gfx_cmdbuf_handle cmdBufHandle) const;

            inline const rz_gfx_shader_reflection&             getShaderReflection() const { return m_ShaderReflection; }
            inline const rz_gfx_shader_handle&                 getShaderHandle() const { return m_ShaderHandle; }
            inline const std::vector<rz_gfx_descriptor_table>& getDescriptorTables() const { return m_DescriptorTables; }
            inline const rz_gfx_descriptor_table&              getDescriptorTableAt(u32 idx) const { return m_DescriptorTables[idx]; }

        private:
            std::vector<DescriptorBlacklist>     m_BlacklistDescriptors = {};
            std::vector<rz_gfx_descriptor_table> m_DescriptorTables     = {};
            rz_gfx_shader_reflection             m_ShaderReflection     = {};
            rz_gfx_shader_handle                 m_ShaderHandle         = {};
        };

    }    // namespace Gfx
}    // namespace Razix