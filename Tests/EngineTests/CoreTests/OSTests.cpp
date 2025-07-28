// OSTests.cpp
// Unit tests for the RZOS class
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/OS/RZOS.h"

#include <gtest/gtest.h>

namespace Razix {

    // Mock OS implementation for testing
    class MockOS : public RZOS
    {
    public:
        MockOS() = default;
        
        void Init() override
        {
            m_InitCalled = true;
        }
        
        void Begin() override
        {
            m_BeginCalled = true;
        }
        
        bool IsInitCalled() const { return m_InitCalled; }
        bool IsBeginCalled() const { return m_BeginCalled; }
        
    private:
        bool m_InitCalled = false;
        bool m_BeginCalled = false;
    };

    class RZOSTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Clean up any existing instance
            RZOS::Destroy();
        }

        void TearDown() override
        {
            // Clean up after tests
            RZOS::Destroy();
        }
    };

    // Test case for OS instance management
    TEST_F(RZOSTests, InstanceManagement)
    {
        // Initially, instance should be null
        EXPECT_EQ(RZOS::GetInstance(), nullptr);
        
        // Create and set an instance
        MockOS* mockOS = new MockOS();
        RZOS::SetInstance(mockOS);
        
        // Verify instance is set
        EXPECT_EQ(RZOS::GetInstance(), mockOS);
        
        // Destroy and verify cleanup
        RZOS::Destroy();
        EXPECT_EQ(RZOS::GetInstance(), nullptr);
    }

    // Test case for singleton pattern
    TEST_F(RZOSTests, SingletonPattern)
    {
        MockOS* mockOS1 = new MockOS();
        MockOS* mockOS2 = new MockOS();
        
        // Set first instance
        RZOS::SetInstance(mockOS1);
        EXPECT_EQ(RZOS::GetInstance(), mockOS1);
        
        // Set second instance (should replace first)
        RZOS::SetInstance(mockOS2);
        EXPECT_EQ(RZOS::GetInstance(), mockOS2);
        EXPECT_NE(RZOS::GetInstance(), mockOS1);
        
        // Clean up
        delete mockOS1; // Mock cleanup since RZOS doesn't own this in our test
        RZOS::Destroy(); // This should clean up mockOS2
    }

    // Test case for abstract interface implementation
    TEST_F(RZOSTests, AbstractInterfaceImplementation)
    {
        MockOS* mockOS = new MockOS();
        RZOS::SetInstance(mockOS);
        
        // Initially, methods should not have been called
        EXPECT_FALSE(mockOS->IsInitCalled());
        EXPECT_FALSE(mockOS->IsBeginCalled());
        
        // Call abstract methods through the interface
        RZOS* osInstance = RZOS::GetInstance();
        ASSERT_NE(osInstance, nullptr);
        
        osInstance->Init();
        EXPECT_TRUE(mockOS->IsInitCalled());
        
        osInstance->Begin();
        EXPECT_TRUE(mockOS->IsBeginCalled());
        
        RZOS::Destroy();
    }

    // Test case for lifecycle management
    TEST_F(RZOSTests, LifecycleManagement)
    {
        MockOS* mockOS = new MockOS();
        RZOS::SetInstance(mockOS);
        
        RZOS* osInstance = RZOS::GetInstance();
        ASSERT_NE(osInstance, nullptr);
        
        // Test proper initialization sequence
        osInstance->Init();
        osInstance->Begin();
        
        EXPECT_TRUE(mockOS->IsInitCalled());
        EXPECT_TRUE(mockOS->IsBeginCalled());
        
        // Test destruction
        RZOS::Destroy();
        EXPECT_EQ(RZOS::GetInstance(), nullptr);
    }

    // Test case for multiple init/begin calls
    TEST_F(RZOSTests, MultipleCallsHandling)
    {
        MockOS* mockOS = new MockOS();
        RZOS::SetInstance(mockOS);
        
        RZOS* osInstance = RZOS::GetInstance();
        ASSERT_NE(osInstance, nullptr);
        
        // Call Init multiple times
        osInstance->Init();
        osInstance->Init();
        EXPECT_TRUE(mockOS->IsInitCalled());
        
        // Call Begin multiple times
        osInstance->Begin();
        osInstance->Begin();
        EXPECT_TRUE(mockOS->IsBeginCalled());
        
        RZOS::Destroy();
    }

    // Test case for null instance safety
    TEST_F(RZOSTests, NullInstanceSafety)
    {
        // Ensure no instance is set
        RZOS::Destroy();
        EXPECT_EQ(RZOS::GetInstance(), nullptr);
        
        // Setting null instance should be safe
        RZOS::SetInstance(nullptr);
        EXPECT_EQ(RZOS::GetInstance(), nullptr);
        
        // Destroying when no instance should be safe
        EXPECT_NO_THROW(RZOS::Destroy());
    }

    // Test case for instance replacement
    TEST_F(RZOSTests, InstanceReplacement)
    {
        MockOS* firstOS = new MockOS();
        MockOS* secondOS = new MockOS();
        
        // Set first instance and initialize
        RZOS::SetInstance(firstOS);
        RZOS::GetInstance()->Init();
        EXPECT_TRUE(firstOS->IsInitCalled());
        
        // Replace with second instance
        RZOS::SetInstance(secondOS);
        EXPECT_EQ(RZOS::GetInstance(), secondOS);
        EXPECT_FALSE(secondOS->IsInitCalled()); // New instance not initialized
        
        // Initialize second instance
        RZOS::GetInstance()->Init();
        EXPECT_TRUE(secondOS->IsInitCalled());
        
        // Clean up
        delete firstOS; // Manual cleanup for replaced instance
        RZOS::Destroy(); // Cleanup current instance
    }

    // Test case for OS class hierarchy
    TEST_F(RZOSTests, ClassHierarchy)
    {
        MockOS* mockOS = new MockOS();
        
        // Test that MockOS is properly derived from RZOS
        RZOS* basePtr = mockOS;
        EXPECT_NE(basePtr, nullptr);
        
        // Test polymorphic behavior
        RZOS::SetInstance(mockOS);
        RZOS* instance = RZOS::GetInstance();
        
        // Should be able to call virtual methods through base pointer
        EXPECT_NO_THROW(instance->Init());
        EXPECT_NO_THROW(instance->Begin());
        
        RZOS::Destroy();
    }

}    // namespace Razix