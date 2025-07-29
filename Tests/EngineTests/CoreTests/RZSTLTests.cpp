// RZSTLTests.cpp
// Unit tests for the RZSTL components
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/RZSTL/smart_pointers.h"
#include "Razix/Core/RZSTL/vector.h"
#include "Razix/Core/RZSTL/string.h"
#include "Razix/Core/RZSTL/unordered_map.h"
#include "Razix/Core/RZSTL/ring_buffer.h"

#include <gtest/gtest.h>

namespace Razix {

    class RZSTLTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            // Setup for RZSTL tests
        }

        void TearDown() override
        {
            // Cleanup for RZSTL tests
        }
    };

    // Test case for rzstl::vector functionality
    TEST_F(RZSTLTests, VectorFunctionality)
    {
        rzstl::vector<int> vec;
        
        // Test basic operations
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.size(), 0);
        
        // Test adding elements
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        
        EXPECT_FALSE(vec.empty());
        EXPECT_EQ(vec.size(), 3);
        EXPECT_EQ(vec[0], 1);
        EXPECT_EQ(vec[1], 2);
        EXPECT_EQ(vec[2], 3);
        
        // Test iterators
        int count = 1;
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            EXPECT_EQ(*it, count++);
        }
        
        // Test clear
        vec.clear();
        EXPECT_TRUE(vec.empty());
        EXPECT_EQ(vec.size(), 0);
    }

    // Test case for rzstl::string functionality
    TEST_F(RZSTLTests, StringFunctionality)
    {
        rzstl::string str;
        
        // Test empty string
        EXPECT_TRUE(str.empty());
        EXPECT_EQ(str.length(), 0);
        
        // Test assignment
        str = "Hello";
        EXPECT_FALSE(str.empty());
        EXPECT_EQ(str.length(), 5);
        EXPECT_EQ(str, "Hello");
        
        // Test concatenation
        rzstl::string str2 = " World";
        str += str2;
        EXPECT_EQ(str, "Hello World");
        EXPECT_EQ(str.length(), 11);
        
        // Test substring
        auto substr = str.substr(0, 5);
        EXPECT_EQ(substr, "Hello");
        
        // Test find
        auto pos = str.find("World");
        EXPECT_NE(pos, rzstl::string::npos);
        EXPECT_EQ(pos, 6);
    }

    // Test case for rzstl::unordered_map functionality
    TEST_F(RZSTLTests, UnorderedMapFunctionality)
    {
        rzstl::unordered_map<rzstl::string, int> map;
        
        // Test empty map
        EXPECT_TRUE(map.empty());
        EXPECT_EQ(map.size(), 0);
        
        // Test insertion
        map["one"] = 1;
        map["two"] = 2;
        map["three"] = 3;
        
        EXPECT_FALSE(map.empty());
        EXPECT_EQ(map.size(), 3);
        
        // Test access
        EXPECT_EQ(map["one"], 1);
        EXPECT_EQ(map["two"], 2);
        EXPECT_EQ(map["three"], 3);
        
        // Test find
        auto it = map.find("two");
        EXPECT_NE(it, map.end());
        EXPECT_EQ(it->second, 2);
        
        // Test erase
        map.erase("two");
        EXPECT_EQ(map.size(), 2);
        EXPECT_EQ(map.find("two"), map.end());
        
        // Test clear
        map.clear();
        EXPECT_TRUE(map.empty());
        EXPECT_EQ(map.size(), 0);
    }

    // Test case for smart pointer functionality
    TEST_F(RZSTLTests, SmartPointerFunctionality)
    {
        // Test rzstl::SharedPtr
        {
            auto ptr1 = rzstl::CreateSharedPtr<int>(42);
            EXPECT_NE(ptr1.get(), nullptr);
            EXPECT_EQ(*ptr1, 42);
            EXPECT_EQ(ptr1.use_count(), 1);
            
            auto ptr2 = ptr1;
            EXPECT_EQ(ptr1.use_count(), 2);
            EXPECT_EQ(ptr2.use_count(), 2);
            EXPECT_EQ(ptr1.get(), ptr2.get());
            
            ptr2.reset();
            EXPECT_EQ(ptr1.use_count(), 1);
            EXPECT_EQ(ptr2.get(), nullptr);
        }
        
        // Test rzstl::UniquePtr
        {
            auto ptr = rzstl::CreateUniquePtr<int>(24);
            EXPECT_NE(ptr.get(), nullptr);
            EXPECT_EQ(*ptr, 24);
            
            // Test move semantics
            auto ptr2 = std::move(ptr);
            EXPECT_EQ(ptr.get(), nullptr);
            EXPECT_NE(ptr2.get(), nullptr);
            EXPECT_EQ(*ptr2, 24);
        }
    }

    // Test case for ring buffer functionality
    TEST_F(RZSTLTests, RingBufferFunctionality)
    {
        rzstl::ring_buffer<int, 4> buffer;
        
        // Test empty buffer
        EXPECT_TRUE(buffer.empty());
        EXPECT_EQ(buffer.size(), 0);
        EXPECT_EQ(buffer.capacity(), 4);
        
        // Test adding elements
        buffer.push_back(1);
        buffer.push_back(2);
        buffer.push_back(3);
        
        EXPECT_FALSE(buffer.empty());
        EXPECT_EQ(buffer.size(), 3);
        EXPECT_EQ(buffer.front(), 1);
        EXPECT_EQ(buffer.back(), 3);
        
        // Test accessing elements
        EXPECT_EQ(buffer[0], 1);
        EXPECT_EQ(buffer[1], 2);
        EXPECT_EQ(buffer[2], 3);
        
        // Test wrapping around
        buffer.push_back(4);
        buffer.push_back(5); // Should overwrite the first element
        
        EXPECT_EQ(buffer.size(), 4);
        EXPECT_EQ(buffer.front(), 2); // First element should now be 2
        EXPECT_EQ(buffer.back(), 5);
        
        // Test pop
        buffer.pop_front();
        EXPECT_EQ(buffer.size(), 3);
        EXPECT_EQ(buffer.front(), 3);
    }

    // Test case for ref_counter functionality
    TEST_F(RZSTLTests, RefCounterFunctionality)
    {
        rzstl::ref_counter counter;
        
        // Test initial state
        EXPECT_EQ(counter.use_count(), 0);
        
        // Test increment/decrement
        counter.add_ref();
        EXPECT_EQ(counter.use_count(), 1);
        
        counter.add_ref();
        EXPECT_EQ(counter.use_count(), 2);
        
        counter.release();
        EXPECT_EQ(counter.use_count(), 1);
        
        counter.release();
        EXPECT_EQ(counter.use_count(), 0);
    }

}    // namespace Razix