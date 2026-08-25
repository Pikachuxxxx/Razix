// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZRenderThread.h"

#include <Core/Log/RZLog.h>
#include <Core/Profiling/RZProfiling.h>
#include <Core/RZEngine.h>
#include <Core/std/atomics.h>
#include <Core/std/thread.h>
#include <Gfx/RZWorld.h>

#include "Razix/Core/App/RZApplication.h"
#include "Razix/Gfx/Renderers/RZWorldRenderer.h"

#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>

namespace Razix {
    namespace Gfx {

        //--------------------------------------------
        WorldRingBuffer g_RenderThreadRingBuffer;
        rz_atomic_u32 g_RenderThreadIsRunning = false;
        //--------------------------------------------
        RAZIX_TLS u64 renderThreadReadCounter = 0;

        static void RenderRuntimeAssetsIconsOnImGui()
        {
#if 0
            // Guizmo Controls for an Entity
            if (m_EnableGuizmoEditing) {
                auto currentScene = RZSceneManager::Get().getCurrentSceneMutablePtr();
                //auto&          registry     = currentScene->getRegistry();
                //auto           cameraView   = registry.view<CameraComponent>();
                //RZSceneCamera* cam          = nullptr;
                //if (!cameraView.empty()) {
                //    // By using front we get the one and only or the first one in the list of camera entities
                //    cam = &cameraView.get<CameraComponent>(cameraView.front()).Camera;
                //}

//            auto& cam = currentScene->getSceneCamera();

                // Guizmo Editing Here
                TransformComponent& tc              = m_GuizmoEntity.GetComponent<TransformComponent>();
                float4x4           transformMatrix = tc.GetLocalTransform();
//            float4x4           deltaMatrix     = float4x4(1.0f);

                //ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, value_ptr(transformMatrix));

                // https://github.com/CedricGuillemet/ImGuizmo/issues/237
                //ImGuizmo::Manipulate(value_ptr(cam.getViewMatrix()), value_ptr(cam.getProjectionRaw()), (ImGuizmo::OPERATION) m_GuizmoOperation, (ImGuizmo::MODE) m_GuizmoMode, value_ptr(transformMatrix), value_ptr(deltaMatrix), &m_GuizmoSnapAmount);

                f32 matrixTranslation[3], matrixRotation[3], matrixScale[3];
                //ImGuizmo::DecomposeMatrixToComponents(&(transformMatrix[0][0]), matrixTranslation, matrixRotation, matrixScale);

                tc.Translation = float3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
                tc.Rotation    = float3(radians(matrixRotation[0]), radians(matrixRotation[1]), radians(matrixRotation[2]));
                tc.Scale       = float3(matrixScale[0], matrixScale[1], matrixScale[2]);
                tc.Transform   = transformMatrix;
            }

            // TODO: As for Icons of the components or any other entities we will get them using the entt
            // Get their position in the worldspace and check it against the camera frustum and
            // convert it to screen space and render a non-clickable ImGui::Button with the FontIcon as image
#endif
        }

        static void RenderEngineStatsOnImGui()
        {
            // Engine Stats
            {
                // Engine stats
                ImGuiWindowFlags     window_flags     = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
                const f32            DISTANCE         = 10.0f;
                const ImGuiViewport* viewport         = ImGui::GetMainViewport();
                ImVec2               work_area_pos    = viewport->WorkPos;    // Use work area to avoid menu-bar/task-bar, if any!
                ImVec2               work_area_size   = viewport->WorkSize;
                ImVec2               window_pos       = ImVec2((1 & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) : (work_area_pos.x + DISTANCE), (1 & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
                ImVec2               window_pos_pivot = ImVec2((1 & 1) ? 1.0f : 0.0f, (1 & 2) ? 1.0f : 0.0f);
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                ImGui::SetNextWindowBgAlpha(0.35f);    // Transparent background

                ImGui::Begin("Engine Stats", 0, window_flags);
                {
                    auto& stats = RZEngine::Get().GetStatistics();
                    ImGui::Text("Engine Stats");
                    ImGui::Indent();
                    {
                        // TODO: Add Average timings (CPU + GPU) and avg FPS
                        ImGui::Text("FPS                        : %.5d", stats.FramesPerSecond);
                        ImGui::Text("CPU time                   : %5.2f ms", stats.DeltaTime);

                        ImGui::Separator();
                        ImGui::Text("API calls");

                        ImGui::Text("Total Draw calls           : %d", stats.NumDrawCalls);
                        ImGui::Indent();
                        {
                            ImGui::BulletText("Draws                : %d", stats.Draws);
                            ImGui::BulletText("Indexed Draws        : %d", stats.IndexedDraws);
                            ImGui::BulletText("Compute Dispatches   : %d", stats.ComputeDispatches);
                        }
                        ImGui::Unindent();

                        ImGui::Separator();
                        ImGui::Text("Memory Usage");

                        ImGui::Indent();
                        {
                            ImGui::BulletText("Used VRAM            : %f Gib", stats.GPUMemoryUsed);
                            ImGui::BulletText("Total VRAM           : %f Gib", stats.TotalGPUMemory);
                            ImGui::BulletText("Total RAM            : %f Gib", stats.UsedRAM);
                        }
                        ImGui::Unindent();

                        ImGui::Separator();
                        ImGui::Text("Meshes Renderer            : %d", stats.MeshesRendered);
                        ImGui::Text("Vertices count             : %d", stats.VerticesCount);
                    }
                    ImGui::Unindent();

                    ImGui::Separator();
                    //ImGui::Text("Pass Timings");
                    if (ImGui::TreeNode("Pass Timings")) {
                        f32 Totaldt = 0.0f;
                        {
                            Razix::RZScopedCriticalSection lock(stats.PassTimingsMutex);
                            for (auto& [name, dt]: stats.PassTimings) {
                                Totaldt += dt;
                                ImGui::BulletText("%-23s : %5.2f ms", name.c_str(), dt);
                            }
                        }
                        ImGui::Separator();
                        ImGui::BulletText("%-23s : %5.2f ms", "Passes Sum", Totaldt);
                        ImGui::BulletText("%-23s : %5.2f ms", "Acquire + Flip", stats.DeltaTime - Totaldt);
                        ImGui::TreePop();
                    }
                }
                ImGui::End();
            }
            ImGui::Render();
        }

        static void RenderEngineImGuiElements()
        {
            RenderRuntimeAssetsIconsOnImGui();
            RenderEngineStatsOnImGui();
        }

        static void RenderThreadRenderGUI()
        {
            auto ctx = ImGui::GetCurrentContext();
            if (!ctx || RZEngine::Get().isEngineInTestMode())
                return;

            ImGui_ImplGlfw_NewFrame();

            ImGuiIO& io = ImGui::GetIO();
            (void) io;

            ImGui::NewFrame();

            RZEngine::Get().getWorldRenderer().OnImGui();

            RZEngine::Get().getScriptHandler().OnImGui(NULL);

            RZApplication::Get().OnImGui();

            RenderEngineImGuiElements();
        }

        static void RenderThreadRunRenderLoop(void* pUserData)
        {
            const char* pThreadName = g_ThreadNames_Tanu[RZ_THREAD_NAME_RENDER];
            RAZIX_PROFILE_SETTHREADNAME(pThreadName);

            RZWorldRenderer& worldRenderer = RZEngine::Get().getWorldRenderer();
            while (rz_atomic32_load(&g_RenderThreadIsRunning, RZ_MEMORY_ORDER_RELAXED))
            {
                RAZIX_PROFILE_SCOPEC("RenderThreadLoop", RZ_PROFILE_COLOR_RENDERERS);
                
                u64 currGameWriteIdx = rz_atomic64_load(&g_RenderThreadRingBuffer.m_GameThreadWorldDataWriteCounter, RZ_MEMORY_ORDER_ACQUIRE);
                RAZIX_CORE_TRACE("Reading atomic curr game write idx: {0}", currGameWriteIdx);
                RAZIX_CORE_TRACE("current TLS render thread read counter: {0}", renderThreadReadCounter);

                // yay we got new data
                if(currGameWriteIdx != renderThreadReadCounter || renderThreadReadCounter == 0)
                {
                    RAZIX_CORE_TRACE("Got new world data from game thread, world renderer is free to render this now.");
                    renderThreadReadCounter = currGameWriteIdx;
                    RAZIX_CORE_TRACE("Updating render thread read counter to : {0}, now this will be blocked by m_RenderThreadWorldDataReadCounter so that game thread cannot write to this world buffer slot.");
                    
                    // immediately update the read index so that game thread knows we cannot write to this slot
                    rz_atomic64_store(&g_RenderThreadRingBuffer.m_RenderThreadWorldDataReadCounter, renderThreadReadCounter, RZ_MEMORY_ORDER_RELEASE);

                    // now get the world corresponding to this index
                    u32 worldBufferIdx = currGameWriteIdx % RAZIX_WORLDS_IN_FLIGHT;
                    RAZIX_CORE_TRACE("Reading world data from frin inflight buffer at idx: {0}", worldBufferIdx);
                    const RZWorld& world = g_RenderThreadRingBuffer.worldBuffers[worldBufferIdx];

                    RenderThreadRenderGUI();

                    // worldRenderer.buildFrameGraph(world);
                    worldRenderer.drawFrame(world);
                }
            }

            // Note: only flush here, do NOT destroy the world renderer's resources - the render
            // thread is done submitting work, but the app's OnQuit() (called by the main thread
            // after this thread has been joined) may still need to read back GPU-produced data
            // (e.g. swapchain readback) and destroy its own pass-owned resources. The world
            // renderer itself is destroyed afterwards by RZApplication::Quit().
            Razix::RZEngine::Get().getWorldRenderer().flushGPUWork();
        }

        static u64 s_DummyRenderThreadUserData = 0;

        rz_thread_handle RenderThreadCreate()
        {
            rz_thread_handle handle;
            RAZIX_CORE_TRACE("Creating render thread!");

            rz_atomic32_store(&g_RenderThreadIsRunning, true, RZ_MEMORY_ORDER_RELAXED); // we just want it published dont really care about ordering
           
            const char* pRenderThreadName = g_ThreadNames_Tanu[RZ_THREAD_NAME_RENDER];
            handle = rz_thread_create(pRenderThreadName, RZ_THREAD_PRIORITY_NORMAL, RZ_THREAD_AFFINITY_RENDER, RenderThreadRunRenderLoop, &s_DummyRenderThreadUserData);
            // Note: not detached, RenderThreadDestroy joins it so callers can rely on the
            // render thread (and its GPU teardown) having fully finished before returning.
            return handle;
        }

        void RenderThreadDestroy(rz_thread_handle handle)
        {
            // be safe than sorry, kill render thread loop anyways
            rz_atomic32_store(&g_RenderThreadIsRunning, false, RZ_MEMORY_ORDER_RELEASE);

            // block until the render thread has actually exited its loop and finished
            // flushGPUWork()/destroy() before we let the caller continue tearing down
            // the rest of the engine (RHI backend, resource pools, etc.)
            rz_thread_join(handle);
        }
    }
}    // namespace Razix
