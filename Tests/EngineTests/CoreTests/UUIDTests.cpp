// UUIDTests.cpp
// AI-generated unit tests for the RZUUID classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/UUID/RZUUID.h"

#include <gtest/gtest.h>
#include <regex>
#include <set>

namespace Razix {

    class RZUUIDTests : public ::testing::Test
    {
    protected:
        // Helper to check valid UUID string format [8-4-4-4-12]
        bool isValidUUID(const std::string& uuid)
        {
            const std::regex uuidRegex("^[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[4][a-fA-F0-9]{3}-[89aAbB][a-fA-F0-9]{3}-[a-fA-F0-9]{12}$");
            return std::regex_match(uuid, uuidRegex);
        }
    };

    // Test case for default constructor
    TEST_F(RZUUIDTests, DefaultConstructor)
    {
        Razix::RZUUID uuid;
        std::string   prettyStr = uuid.prettyString();
        EXPECT_TRUE(isValidUUID(prettyStr)) << "Generated UUID does not match the expected format: " << prettyStr;
    }

    // Test case for copy constructor
    TEST_F(RZUUIDTests, CopyConstructor)
    {
        Razix::RZUUID uuid1;
        Razix::RZUUID uuid2(uuid1);

        EXPECT_EQ(uuid1, uuid2) << "Copy constructor failed to create an equivalent UUID.";
    }

    // Test case for FromPrettyStrFactory
    TEST_F(RZUUIDTests, FromPrettyStrFactory)
    {
        Razix::RZUUID uuid1;
        std::string   prettyStr = uuid1.prettyString();
        Razix::RZUUID uuid2     = Razix::RZUUID::FromPrettyStrFactory(prettyStr);
        EXPECT_EQ(uuid1, uuid2) << "FromPrettyStrFactory failed to recreate the UUID from its pretty string representation.";
    }

    TEST_F(RZUUIDTests, DeserializeSerializePrettyStringRoundTrip)
    {
        std::string originalPretty    = "09471372-a82c-430c-b79a-be29e88e3577";

        RZUUID      uuid              = RZUUID::FromPrettyStrFactory(originalPretty);
        std::string regeneratedPretty = uuid.prettyString();
        EXPECT_EQ(regeneratedPretty, originalPretty) << "Deserializes and Serialization pretty string round trip is not accurate";
    }

    // Test case for equality operator
    TEST_F(RZUUIDTests, EqualityOperator)
    {
        Razix::RZUUID uuid1;
        Razix::RZUUID uuid2(uuid1);

        EXPECT_TRUE(uuid1 == uuid2) << "Equality operator failed for equivalent UUIDs.";
        EXPECT_FALSE(uuid1 != uuid2) << "Inequality operator failed for equivalent UUIDs.";
    }

    // Test case for ordering operators
    TEST_F(RZUUIDTests, OrderingOperators)
    {
        Razix::RZUUID uuid1, uuid2;

        EXPECT_TRUE((uuid1 < uuid2) || (uuid1 > uuid2) || (uuid1 == uuid2)) << "Ordering operators did not produce consistent results.";
    }

    // Test case for uniqueness of UUIDs
    TEST_F(RZUUIDTests, Uniqueness)
    {
        constexpr int         numUUIDs = 1000;
        std::set<std::string> uuidSet;

        for (int i = 0; i < numUUIDs; ++i) {
            Razix::RZUUID uuid;
            uuidSet.insert(uuid.prettyString());
        }

        EXPECT_EQ(uuidSet.size(), numUUIDs) << "Generated UUIDs are not unique.";
    }

    // Test case for hash function
    TEST_F(RZUUIDTests, HashFunction)
    {
        Razix::RZUUID uuid1, uuid2;

        auto hash1 = uuid1.hash();
        auto hash2 = uuid2.hash();

        if (uuid1 == uuid2) {
            EXPECT_EQ(hash1, hash2) << "Hash function mismatch for equivalent UUIDs.";
        } else {
            EXPECT_NE(hash1, hash2) << "Hash function collision for distinct UUIDs.";
        }
    }

    // Test case for ostream and istream operators
    TEST_F(RZUUIDTests, StreamOperators)
    {
        Razix::RZUUID uuid1;

        std::stringstream ss;
        ss << uuid1;

        Razix::RZUUID uuid2;
        ss >> uuid2;

        EXPECT_EQ(uuid1, uuid2) << "Stream operators failed to serialize/deserialize the UUID.";
    }
}    // namespace Razix