// SocketsTests.cpp
// Unit tests for the Network sockets system
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Network/RZSockets.h"

#include <gtest/gtest.h>

namespace Razix {
    namespace Network {

        class RZSocketsTests : public ::testing::Test
        {
        protected:
            void SetUp() override
            {
                // Initialize any required setup
            }

            void TearDown() override
            {
                // Clean up any allocated resources
            }
        };

        // Test case for default port constants
        TEST_F(RZSocketsTests, DefaultPortConstants)
        {
            EXPECT_EQ(RAZIX_DEFAULT_SERVER_PORT, 29000) << "Default server port should be 29000.";
            EXPECT_EQ(RAZIX_DEFAULT_CLIENT_PORT, 44000) << "Default client port should be 44000.";
        }

        // Test case for port ranges
        TEST_F(RZSocketsTests, PortRanges)
        {
            // Check that ports are in valid ranges
            EXPECT_GT(RAZIX_DEFAULT_SERVER_PORT, 1024) << "Server port should be above reserved range.";
            EXPECT_LT(RAZIX_DEFAULT_SERVER_PORT, 65536) << "Server port should be within valid range.";
            
            EXPECT_GT(RAZIX_DEFAULT_CLIENT_PORT, 1024) << "Client port should be above reserved range.";
            EXPECT_LT(RAZIX_DEFAULT_CLIENT_PORT, 65536) << "Client port should be within valid range.";
        }

        // Test case for SocketProtocol enum values
        TEST_F(RZSocketsTests, SocketProtocolEnumValues)
        {
            EXPECT_NO_THROW({
                auto tcp = SocketProtocol::TCP;
                auto udp = SocketProtocol::UDP;
            }) << "SocketProtocol enum values should be accessible.";
        }

        // Test case for SocketProtocol enum distinctness
        TEST_F(RZSocketsTests, SocketProtocolEnumDistinctness)
        {
            EXPECT_NE(static_cast<int>(SocketProtocol::TCP), 
                     static_cast<int>(SocketProtocol::UDP)) << "TCP and UDP should have different values.";
        }

        // Test case for SocketStatus enum values
        TEST_F(RZSocketsTests, SocketStatusEnumValues)
        {
            EXPECT_EQ(static_cast<int>(SocketStatus::SUCCESS), 0) << "SUCCESS should be 0.";
            
            EXPECT_NO_THROW({
                auto success = SocketStatus::SUCCESS;
                auto err = SocketStatus::ERR;
                auto timeout = SocketStatus::TIMEOUT;
                auto closed = SocketStatus::CLOSED;
            }) << "SocketStatus enum values should be accessible.";
        }

        // Test case for SocketStatus enum ordering
        TEST_F(RZSocketsTests, SocketStatusEnumOrdering)
        {
            EXPECT_EQ(static_cast<int>(SocketStatus::SUCCESS), 0) << "SUCCESS should be the first (0) status.";
            
            // Ensure all values are distinct
            EXPECT_NE(static_cast<int>(SocketStatus::SUCCESS), static_cast<int>(SocketStatus::ERR));
            EXPECT_NE(static_cast<int>(SocketStatus::SUCCESS), static_cast<int>(SocketStatus::TIMEOUT));
            EXPECT_NE(static_cast<int>(SocketStatus::SUCCESS), static_cast<int>(SocketStatus::CLOSED));
            EXPECT_NE(static_cast<int>(SocketStatus::ERR), static_cast<int>(SocketStatus::TIMEOUT));
            EXPECT_NE(static_cast<int>(SocketStatus::ERR), static_cast<int>(SocketStatus::CLOSED));
            EXPECT_NE(static_cast<int>(SocketStatus::TIMEOUT), static_cast<int>(SocketStatus::CLOSED));
        }

        // Test case for port separation
        TEST_F(RZSocketsTests, PortSeparation)
        {
            // Ensure server and client ports are different
            EXPECT_NE(RAZIX_DEFAULT_SERVER_PORT, RAZIX_DEFAULT_CLIENT_PORT) 
                << "Server and client ports should be different.";
        }

    }    // namespace Network
}    // namespace Razix