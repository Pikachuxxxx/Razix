#if 1
    #include <Razix.h>

using namespace Razix;

class MemTestClass : public Razix::RZMemoryRoot
{
public:
    uint32_t member_1 = 0;
    char     member_3 = 'A';

    MemTestClass() {}
    ~MemTestClass() {}

    uint32_t getMember_1() { return member_1; }
};

class MemoryTest : public Razix::RZApplication
{
public:
    MemoryTest()
        //: RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "SponzaSandbox")
        : RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "ShadowsSandbox")

    {
        Razix::RZInput::SelectGLFWInputManager();
        Razix::RZApplication::Get().Init();

        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------

        // Init Graphics Context
        //-------------------------------------------------------------------------------------
        // Creating the Graphics Context and Initialize it
        RAZIX_CORE_INFO("Creating Graphics Context...");
        Razix::Graphics::RZGraphicsContext::Create(RZApplication::Get().getWindowProps(), RZApplication::Get().getWindow());
        RAZIX_CORE_INFO("Initializing Graphics Context...");
        Razix::Graphics::RZGraphicsContext::GetContext()->Init();
        //-------------------------------------------------------------------------------------
    }

    void OnStart() override
    {
        // Testing the HeapAllocator
        {
            Razix::Memory::RZHeapAllocator heapAlloc;
            RAZIX_INFO("Allocating 16 Mb of Heap memory");
            heapAlloc.init(static_cast<size_t>(16_Mib));

            void* alloc_1 = heapAlloc.allocate(245_Kib, 16);
            void* alloc_2 = heapAlloc.allocate(128_Kib, 16);
            void* alloc_3 = heapAlloc.allocate(512_Kib, 16);

            heapAlloc.deallocate(alloc_1);
            heapAlloc.deallocate(alloc_2);
            heapAlloc.deallocate(alloc_3);

            heapAlloc.shutdown();
        }

        // Testing the RingAllocator
        {
            //Razix::Memory::RZRingAllocator<uint32_t> some_ints_in_ring;
            //some_ints_in_ring.init(25);

            //// Test 1: simple insert and immediate read!
            //for (uint32_t i = 0; i < 45; i++) {
            //    some_ints_in_ring.put(i);
            //    RAZIX_TRACE("Ring buffer value at : {0} | head : {1}, tail : {2}", some_ints_in_ring.get(), some_ints_in_ring.getHead(), some_ints_in_ring.getTail());
            //    if (some_ints_in_ring.isFull())
            //        RAZIX_WARN("Ring Allocator is Full!");
            //}
            //some_ints_in_ring.shutdown();
        }

        {
            //Razix::Memory::RZRingAllocator<std::unique_ptr<Graphics::RZCommandBuffer>> frame_command_buffers;
            //frame_command_buffers.init(3);
            //for (uint32_t i = 0; i < 45; i++) {
            //    frame_command_buffers.put(std::make_unique<Graphics::RZCommandBuffer>(Razix::Graphics::RZCommandBuffer::Create()));
            //    RAZIX_TRACE("Ring buffer value at : {0} | head : {1}, tail : {2}", fmt::ptr(frame_command_buffers.get()), frame_command_buffers.getHead(), frame_command_buffers.getTail());
            //    if (frame_command_buffers.isFull())
            //        RAZIX_WARN("Ring Allocator is Full!");
            //}
        }

        // Add a directional light for test
        auto lightEnitties = RZSceneManager::Get().getCurrentScene()->GetComponentsOfType<Razix::LightComponent>();
        if (!lightEnitties.size()) {
            auto directionalLightEntity = RZSceneManager::Get().getCurrentScene()->createEntity("Directional Light");
            directionalLightEntity.AddComponent<Razix::LightComponent>();
            directionalLightEntity.GetComponent<Razix::LightComponent>().light.setDirection(glm::vec3(1.0f));
        }

        auto scripts = RZSceneManager::Get().getCurrentScene()->GetComponentsOfType<Razix::LuaScriptComponent>();
        if (!scripts.size()) {
            Razix::RZEntity imguiEntity = RZSceneManager::Get().getCurrentScene()->createEntity("imgui_script_entity");
            imguiEntity.AddComponent<Razix::LuaScriptComponent>();
            if (imguiEntity.HasComponent<Razix::LuaScriptComponent>()) {
                Razix::LuaScriptComponent& lsc = imguiEntity.GetComponent<Razix::LuaScriptComponent>();
                lsc.loadScript("//Scripts/imgui_test.lua");
            }
        }

        // Camera Entity
        auto cameras = RZSceneManager::Get().getCurrentScene()->GetComponentsOfType<Razix::CameraComponent>();
        if (!cameras.size()) {
            Razix::RZEntity camera = RZSceneManager::Get().getCurrentScene()->createEntity("Camera");
            camera.AddComponent<Razix::CameraComponent>();
            if (camera.HasComponent<Razix::CameraComponent>()) {
                Razix::CameraComponent& cc = camera.GetComponent<Razix::CameraComponent>();
                cc.Camera.setViewportSize(getWindow()->getWidth(), getWindow()->getHeight());
            }
        }

    #if 0
        // PBR materials test
        {
            int nrRows    = 7;
            int nrColumns = 7;
            int spacing   = 2.5f;

            Razix::Graphics::MaterialProperties mat;
            mat.albedoColor      = glm::vec3(1.0f, 0.3f, 0.75f);
            mat.ambientOcclusion = 1.0f;

            for (int row = 0; row < nrRows; ++row) {
                float metallic = (float) row / (float) nrRows;
                for (int col = 0; col < nrColumns; ++col) {
                    float roughness                                             = glm::clamp((float) col / (float) nrColumns, 0.075f, 1.0f);
                    auto  pos                                                   = glm::vec3((col - (nrColumns / 2)) * spacing, (row - (nrRows / 2)) * spacing, 0.0f);
                    auto  sphereEntity                                          = RZSceneManager::Get().getCurrentScene()->createEntity("Sphere");
                    sphereEntity.GetComponent<TransformComponent>().Translation = pos;
                    sphereEntity.GetComponent<TransformComponent>().Scale       = glm::vec3(0.75f);
                    auto& mrc                                                   = sphereEntity.AddComponent<MeshRendererComponent>(Graphics::MeshPrimitive::Sphere);
                    auto  material                                              = mrc.Mesh->getMaterial();
                    mat.metallicColor                                           = metallic;
                    mat.roughnessColor                                          = roughness;
                    material->setProperties(mat);
                    // Save to file (assign a new and unique name)
                    auto name = "PBR_Test_Mat_M_" + std::to_string(metallic) + "_R_" + std::to_string(roughness);
                    material->setName(name);
                    material->saveToFile();
                }
            }
        }
    #endif
    }

    void OnRender() override
    {
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Sandbox Application [MemoryTest]");
    return new MemoryTest();
}

RAZIX_PLATFORM_MAIN
#endif