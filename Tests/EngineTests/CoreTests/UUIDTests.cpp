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
        bool isValidUUID(const RZString& uuid)
        {
            const std::regex uuidRegex("^[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[4][a-fA-F0-9]{3}-[89aAbB][a-fA-F0-9]{3}-[a-fA-F0-9]{12}$");
            return std::regex_match(uuid.c_str(), uuidRegex);
        }
    };

    // Test case for default constructor
    TEST_F(RZUUIDTests, DefaultConstructor)
    {
        rz_uuid uuid = rz_uuid_generate();
        char    uuid_str[37];
        rz_uuid_to_pretty_str(&uuid, uuid_str);
        RZString prettyStr(uuid_str);
        EXPECT_TRUE(isValidUUID(prettyStr)) << "Generated UUID does not match the expected format: " << prettyStr.c_str();
    }

    // Test case for copy constructor (using struct assignment)
    TEST_F(RZUUIDTests, CopyConstructor)
    {
        rz_uuid uuid1 = rz_uuid_generate();
        rz_uuid uuid2 = uuid1;

        EXPECT_EQ(uuid1, uuid2) << "Copy failed to create an equivalent UUID.";
    }

    // Test case for rz_uuid_from_pretty_str
    TEST_F(RZUUIDTests, FromPrettyStrFactory)
    {
        rz_uuid uuid1 = rz_uuid_generate();
        char    uuid_str[37];
        rz_uuid_to_pretty_str(&uuid1, uuid_str);
        rz_uuid uuid2 = rz_uuid_from_pretty_str(uuid_str);
        EXPECT_EQ(uuid1, uuid2) << "rz_uuid_from_pretty_str failed to recreate the UUID from its pretty string representation.";
    }

    TEST_F(RZUUIDTests, DeserializeSerializePrettyStringRoundTrip)
    {
        RZString originalPretty = "09471372-a82c-430c-b79a-be29e88e3577";

        rz_uuid  uuid              = rz_uuid_from_pretty_str(originalPretty.c_str());
        char     uuid_str[37];
        rz_uuid_to_pretty_str(&uuid, uuid_str);
        RZString regeneratedPretty(uuid_str);
        EXPECT_EQ(regeneratedPretty, originalPretty) << "Deserializes and Serialization pretty string round trip is not accurate";
    }

    // Test case for equality operator
    TEST_F(RZUUIDTests, EqualityOperator)
    {
        rz_uuid uuid1 = rz_uuid_generate();
        rz_uuid uuid2 = uuid1;

        EXPECT_TRUE(uuid1 == uuid2) << "Equality operator failed for equivalent UUIDs.";
        EXPECT_FALSE(uuid1 != uuid2) << "Inequality operator failed for equivalent UUIDs.";
    }

    // Test case for ordering operators
    TEST_F(RZUUIDTests, OrderingOperators)
    {
        rz_uuid uuid1 = rz_uuid_generate();
        rz_uuid uuid2 = rz_uuid_generate();

        EXPECT_TRUE((uuid1 < uuid2) || (uuid2 < uuid1) || (uuid1 == uuid2)) << "Ordering operators did not produce consistent results.";
    }

    // Test case for uniqueness of UUIDs
    TEST_F(RZUUIDTests, Uniqueness)
    {
        constexpr int      numUUIDs = 1000;
        std::set<std::string> uuidSet;

        for (int i = 0; i < numUUIDs; ++i) {
            rz_uuid uuid = rz_uuid_generate();
            char    uuid_str[37];
            rz_uuid_to_pretty_str(&uuid, uuid_str);
            uuidSet.insert(std::string(uuid_str));
        }

        EXPECT_EQ(uuidSet.size(), numUUIDs) << "Generated UUIDs are not unique.";
    }

    // Test case for hash function
    TEST_F(RZUUIDTests, HashFunction)
    {
        rz_uuid uuid1 = rz_uuid_generate();
        rz_uuid uuid2 = uuid1;

        auto hash1 = rz_uuid_hash(&uuid1);
        auto hash2 = rz_uuid_hash(&uuid2);

        EXPECT_EQ(hash1, hash2) << "Hash function mismatch for equivalent UUIDs.";
        
        rz_uuid uuid3 = rz_uuid_generate();
        // Probability of collision is low, but not zero. Still a good test for non-equality
        if (uuid1 != uuid3) {
            EXPECT_NE(rz_uuid_hash(&uuid1), rz_uuid_hash(&uuid3));
        }
    }
}    // namespace Razix
