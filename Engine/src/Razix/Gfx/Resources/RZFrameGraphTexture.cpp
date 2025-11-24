// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraphTexture.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RHI/RHI.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraphResource.h"

#include "Razix/Gfx/Resources/RZResourceManager.h"

namespace Razix {
    namespace Gfx {

        void RZFrameGraphTexture::create(const RZString& name, const Desc& desc, u32 id, const void* transientAllocator)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (transientAllocator)
                m_TextureHandle = TRANSIENT_ALLOCATOR_CAST(transientAllocator)->acquireTransientTexture(name, desc, id);
            else {
                // If no transient allocator is provided, we create a imported persistent resource only ONCE!
                if (!rz_handle_is_valid(&m_TextureHandle))
                    m_TextureHandle = RZResourceManager::Get().createTexture(name.c_str(), desc);
            }
        }

        void RZFrameGraphTexture::destroy(u32 id, const void* transientAllocator)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (transientAllocator)
                TRANSIENT_ALLOCATOR_CAST(transientAllocator)->releaseTransientTexture(m_TextureHandle, id);
            else {
                if (rz_handle_is_valid(&m_TextureHandle))
                    RZResourceManager::Get().destroyTexture(m_TextureHandle);
            }
        }

        void RZFrameGraphTexture::preRead(u32 descriptorType, u32 resViewOpFlags)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (resViewOpFlags & RZ_GFX_RES_VIEW_OP_FLAG_SKIP_BARRIER)
                return;

            rz_gfx_texture* textureResource = RZResourceManager::Get().getTextureResource(m_TextureHandle);
            RAZIX_CORE_ASSERT(textureResource, "Texture Resource is NULL in RZFrameGraphTexture::preRead!");
            // TODO: Check ASM and memory access patterns for this, seems like too much data is being sent in/out
            rz_gfx_resource_state currentState    = textureResource->resource.hot.currentState;
            rz_gfx_resource_state newDeducedState = RZ_GFX_RESOURCE_STATE_SHADER_READ;    // default state for read access
            newDeducedState                       = rzRHI_DeduceResourceState(static_cast<rz_gfx_descriptor_type>(descriptorType), static_cast<rz_gfx_res_view_op_flags>(resViewOpFlags), false);

            RAZIX_CORE_ASSERT(currentState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "[ReadBarrier::Texture] Current State is UNDEFINED, can't read from an undefined resource!");
            RAZIX_CORE_ASSERT(newDeducedState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "[ReadBarrier::Texture] New Deduced State is UNDEFINED, can't transition to an undefined state!");

#ifndef RAZIX_GOLD_MASTER
            if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging) {
                const char* descTypeStr = rzRHI_GetDescriptorTypeString(static_cast<rz_gfx_descriptor_type>(descriptorType));
                RZString    opBuffer;
                opBuffer.reserve(128);
                const char* opFlagsStr   = rzRHI_ResourceOpFlagsString(static_cast<rz_gfx_res_view_op_flags>(resViewOpFlags), opBuffer.data(), opBuffer.capacity());
                const char* currStateStr = rzRHI_GetResourceStateString(currentState);
                const char* newStateStr  = rzRHI_GetResourceStateString(newDeducedState);

                RAZIX_CORE_INFO(
                    "[ReadBarrier::Texture] Resource: {} | Descriptor: {} | Ops: {} | State: {} ---> {}",
                    textureResource->resource.pCold->pName,
                    descTypeStr,
                    opFlagsStr,
                    currStateStr,
                    newStateStr);
            }
#endif
            rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
            rzRHI_InsertImageBarrier(cmdBuffer, m_TextureHandle, currentState, newDeducedState);
        }

        void RZFrameGraphTexture::preWrite(u32 descriptorType, u32 resViewOpFlags)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            if (resViewOpFlags & RZ_GFX_RES_VIEW_OP_FLAG_SKIP_BARRIER)
                return;

            rz_gfx_texture* textureResource = RZResourceManager::Get().getTextureResource(m_TextureHandle);
            RAZIX_CORE_ASSERT(textureResource, "Texture Resource is NULL in RZFrameGraphTexture::preWrite!");
            // TODO: Check ASM and memory access patterns for this, seems like too much data is being sent in/out
            rz_gfx_resource_state currentState    = textureResource->resource.hot.currentState;
            rz_gfx_resource_state newDeducedState = RZ_GFX_RESOURCE_STATE_RENDER_TARGET;
            newDeducedState                       = rzRHI_DeduceResourceState(static_cast<rz_gfx_descriptor_type>(descriptorType), static_cast<rz_gfx_res_view_op_flags>(resViewOpFlags), true);

            RAZIX_CORE_ASSERT(currentState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "[WriteBarrier::Texture] Current State is UNDEFINED, can't write from an undefined resource!");
            RAZIX_CORE_ASSERT(newDeducedState != RZ_GFX_RESOURCE_STATE_UNDEFINED, "[WriteBarrier::Texture] New Deduced State is UNDEFINED, can't transition to an undefined state!");

#ifndef RAZIX_GOLD_MASTER
            if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging) {
                const char* descTypeStr = rzRHI_GetDescriptorTypeString(static_cast<rz_gfx_descriptor_type>(descriptorType));
                RZString    opBuffer;
                opBuffer.reserve(128);
                const char* opFlagsStr   = rzRHI_ResourceOpFlagsString(static_cast<rz_gfx_res_view_op_flags>(resViewOpFlags), opBuffer.data(), opBuffer.capacity());
                const char* currStateStr = rzRHI_GetResourceStateString(currentState);
                const char* newStateStr  = rzRHI_GetResourceStateString(newDeducedState);

                RAZIX_CORE_INFO(
                    "[WriteBarrier::Texture] Resource: {} | Descriptor: {} | Ops: {} | State: {} ---> {}",
                    textureResource->resource.pCold->pName,
                    descTypeStr,
                    opFlagsStr,
                    currStateStr,
                    newStateStr);
            }
#endif
            rz_gfx_cmdbuf_handle cmdBuffer = RZEngine::Get().getWorldRenderer().getCurrCmdBufHandle();
            rzRHI_InsertImageBarrier(cmdBuffer, m_TextureHandle, currentState, newDeducedState);
        }

        void RZFrameGraphTexture::resize(u32 width, u32 height)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            rzRHI_ResizeTexture(getRHIHandle(), width, height);
        }

        RZString RZFrameGraphTexture::toString(const Desc& desc)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return "[" + rz_to_string(int(desc.width)) + ", " + rz_to_string(int(desc.height)) + ", " + rz_to_string(desc.depth) + "]";
        }
    }    // namespace Gfx
}    // namespace Razix
