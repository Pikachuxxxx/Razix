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
        void                       FreeRootSigDescMemAllocs(rz_gfx_root_signature_desc* rootSigDesc);
        void                       CopyReflectedInputElements(const rz_gfx_shader_reflection* src, rz_gfx_input_element** dst, u32* elementCount);
        void                       FreeInputElementsMemAllocs(rz_gfx_input_element* inputElements, u32 numElements);

        //-----------------------------------------------------------------------------------
        // Shader Bind Map for Descriptor table management
        //-----------------------------------------------------------------------------------

        // Bind tables for these descriptors are ignored by the shader bind map
        constexpr u32 RZ_SHADER_BIND_MAP_IGNORE_TABLE_IDX = 0xffffffff;
        struct DescriptorBlacklist
        {
            std::string              name;
            std::vector<std::string> blacklistNames;
        };
        static const DescriptorBlacklist s_SystemDescriptorsBlacklistPreset    = {"SystemDescriptors (FrameData/SceneLightsData)", {"FrameData", "SceneLightsData"}};
        static const DescriptorBlacklist s_MaterialsDescriptorsBlacklistPreset = {"MaterialDescriptors", {"Material", "albedoMap", "normalMap", "metallicMap", "roughnessMap", "specularMap", "emissiveMap", "aoMap"}};

        /**
         * Manages descriptor tables creation and binding alongside reflection data
         */
        class RAZIX_API RZShaderBindMap
        {
        public:
            struct NamedResView
            {
                std::string                 name;
                rz_gfx_resource_view_handle resourceViewHandle;
            };

            enum BindMapValidationErr
            {
                BIND_MAP_VALIDATION_SUCCESS                 = 0,
                BIND_MAP_VALIDATION_DESCRIPTOR_MISMATCH     = 1 << 1,
                BIND_MAP_VALIDATION_BAD_TABLE_IDX           = 1 << 2,    // How do I use this?
                BIND_MAP_VALIDATION_INVALID_DESCRIPTOR      = 1 << 3,
                BIND_MAP_VALIDATION_FAILED                  = 1 << 4,
                BIND_MAP_VALIDATION_UNFULFILLED_DESCRIPTORS = 1 << 5,
            };

            RZShaderBindMap() = default;

            static RZShaderBindMap& RegisterBindMap(const rz_gfx_shader_handle& shaderHandle);
            static RZShaderBindMap& Create(void* where, const rz_gfx_shader_handle& shaderHandle);

            RZShaderBindMap& setResourceView(const std::string& shaderResName, const rz_gfx_resource_view& resourceView);
            RZShaderBindMap& setResourceView(const std::string& shaderResName, const rz_gfx_resource_view_handle& resourceViewHandle);
            RZShaderBindMap& setDescriptorTable(const rz_gfx_descriptor_table& descriptorTable);
            RZShaderBindMap& setDescriptorTable(const rz_gfx_descriptor_table_handle& descriptorTableHandle);
            RZShaderBindMap& setDescriptorBlacklist(const DescriptorBlacklist& blacklist);
            RZShaderBindMap& setDescriptorBlacklist(const std::string& name, const std::vector<std::string>& blacklistNames);
            RZShaderBindMap& validate();
            RZShaderBindMap& build();
            RZShaderBindMap& clear();
            RZShaderBindMap& clearBlacklist();
            RZShaderBindMap& destroy();

            void                 bind(rz_gfx_cmdbuf_handle cmdBufHandle, rz_gfx_pipeline_type pipelineType);
            BindMapValidationErr error();

            inline const rz_gfx_shader_reflection&                    getShaderReflection() const { return m_ShaderReflection; }
            inline const rz_gfx_shader_handle&                        getShaderHandle() const { return m_ShaderHandle; }
            inline const std::vector<rz_gfx_descriptor_table_handle>& getDescriptorTableHandles() const { return m_DescriptorTables; }
            inline const rz_gfx_descriptor_table_handle&              getDescriptorTableHandleAt(u32 idx) const { return m_DescriptorTables[idx]; }

        private:
            rz_gfx_shader_reflection                           m_ShaderReflection        = {};
            rz_gfx_shader_handle                               m_ShaderHandle            = {};
            std::map<std::string, rz_gfx_resource_view_handle> m_ResourceViewHandleRefs  = {};
            std::vector<DescriptorBlacklist>                   m_BlacklistDescriptors    = {};
            std::map<u32, std::vector<NamedResView>>           m_TableBuilderResViewRefs = {};
            std::vector<rz_gfx_descriptor_table_handle>        m_DescriptorTables        = {};
            BindMapValidationErr                               m_LastError               = BIND_MAP_VALIDATION_FAILED;
            rz_gfx_root_signature_handle                       m_RootSigHandle           = {};
            union
            {
                u32 statusFlags = 0xffffffff;
                struct
                {
                    u32 validated : 1;
                    u32 built : 1;
                    u32 dirty : 1;
                };
            };

        private:
            RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(RZShaderBindMap);

            RZShaderBindMap(rz_gfx_shader_handle shaderHandle);
        };
    }    // namespace Gfx
}    // namespace Razix