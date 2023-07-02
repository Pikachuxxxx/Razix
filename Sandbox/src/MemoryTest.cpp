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
        // Testing the HeapAllocator
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
    }
};
 
Razix::RZApplication* Razix::CreateApplication(int argc, char** argv)
{
    RAZIX_INFO("Creating Razix Sandbox Application [MemoryTest]");
    return new MemoryTest();
}

int main(int argc, char** argv)
{
    EngineMain(argc, argv);

    while (Razix::RZApplication::Get().RenderFrame()) {}

    Razix::RZApplication::Get().Quit();
    Razix::RZApplication::Get().SaveApp();

    EngineExit();

    return EXIT_SUCCESS;
}
#endif