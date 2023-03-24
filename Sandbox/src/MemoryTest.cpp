#if 0
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
        : RZApplication(std::string(RAZIX_STRINGIZE(RAZIX_ROOT_DIR) + std::string("/Sandbox/")), "MemoryTest")
    {
        //-------------------------------------------------------------------------------------
        // Override the Graphics API here! for testing
        Razix::Graphics::RZGraphicsContext::SetRenderAPI(Razix::Graphics::RenderAPI::VULKAN);
        //-------------------------------------------------------------------------------------

        Razix::RZInput::SelectGLFWInputManager();
        Razix::RZApplication::Get().Init();

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
        Razix::Memory::RZHeapAllocator heapAlloc;
        RAZIX_INFO("Allocating 16 Mb of Heap memory");
        heapAlloc.init(static_cast<size_t>(16 * 1024 * 1024));

        void* alloc_1 = heapAlloc.allocate(245 * 1024, 16);
        void* alloc_2 = heapAlloc.allocate(128 * 1024, 16);
        void* alloc_3 = heapAlloc.allocate(512 * 1024, 16);

        heapAlloc.deallocate(alloc_1);
        heapAlloc.deallocate(alloc_2);
        heapAlloc.deallocate(alloc_3);

        heapAlloc.shutdown();
    }
};

Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Sandbox Application [MemoryTest]");
    return new MemoryTest();
}

void main(int argc, char** argv)
{
    EngineMain(argc, argv);

    while (Razix::RZApplication::Get().RenderFrame()) {}

    Razix::RZApplication::Get().Quit();
    Razix::RZApplication::Get().SaveApp();

    EngineExit();
}
#endif