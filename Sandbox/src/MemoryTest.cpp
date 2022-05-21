#include <Razix.h>

using namespace Razix;

class MemTestClass : public Razix::RZMemoryBase
{
public:
    uint32_t member_1 = 0;
    char     member_3 = 'A';
};

class MemoryTest : public Razix::RZApplication
{
public:
    MemoryTest()
        : RZApplication("/Sandbox/", "MemoryTest")
    {
    }

    void OnStart() override
    {
        RAZIX_TRACE("sizeof uint32_t : {0}", sizeof(uint32_t));
        RAZIX_TRACE("sizeof std::string : {0}", sizeof(std::string));
        RAZIX_TRACE("sizeof char : {0}", sizeof(char));
        RAZIX_TRACE("sizeof MemTestClass : {0}", sizeof(MemTestClass));
        memtestClass = new MemTestClass;
    }

    void OnUpdate(const RZTimestep& dt) override
    {
    }

    void OnRender() override
    {
    }

    void OnQuit() override
    {
    }

    void OnResize(uint32_t width, uint32_t height) override
    {
    }

private:
    MemTestClass* memtestClass;
};

Razix::RZApplication* Razix::CreateApplication()
{
    RAZIX_INFO("Creating Razix Sandbox Application [MemoryTest]");
    return new MemoryTest();
}