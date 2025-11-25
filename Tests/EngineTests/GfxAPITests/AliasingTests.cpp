// TransientAllocatorTests.cpp
// AI-generated unit tests for the RZHandle class
#define RAZIX_DEFINE_NO_ENGINE_ENTRY_POINT
#include <Razix.h>
#include <Razix/Gfx/FrameGraph/RZTransientAllocator.h>
#include <gtest/gtest.h>

namespace Razix {
    using namespace Gfx;

    TEST(AliaisngTests, AliasingEndTimeQueueTestInsertAndFind)
    {
        AliasingEndTimeQueue queue;
        queue.insert(0, 5);
        queue.insert(1, 10);
        queue.insert(2, 3);

        // Should return groupID 3, which ends at 10, so available after that
        EXPECT_EQ(queue.findFirstFree(7), 2);
        // No group ends before 2, so invalid
        EXPECT_EQ(queue.findFirstFree(2), UINT32_MAX);
    }

    TEST(AliaisngTests, AliasingEndTimeQueueTestUpdate)
    {
        AliasingEndTimeQueue queue;
        queue.insert(0, 5);
        queue.update(0, 2);
        EXPECT_EQ(queue.findFirstFree(3), 0);
    }

    TEST(AliaisngTests, AliasingGroupTestFitsAndAdd)
    {
        AliasingGroup      group(1);
        RZResourceLifetime lifetime1{0, 2, 4};
        RZResourceLifetime lifetime2{1, 1, 3};

        EXPECT_TRUE(group.fits(lifetime1));
        group.add(lifetime1);
        EXPECT_FALSE(group.fits(lifetime2));
    }

    TEST(AliaisngTests, AliasingBookTestBuildAssignsGroups)
    {
        AliasingBook                              book;
        Razix::RZDynamicArray<RZResourceLifetime> lifetimes = {
            {0, 0, 2},
            {1, 3, 5},
            {2, 1, 4}};

        book.build(lifetimes);
        const auto& groups = book.getGroups();
        EXPECT_GE(groups.size(), 2);
        EXPECT_NE(book.getGroupIDForResource(0), UINT32_MAX);
        EXPECT_NE(book.getGroupIDForResource(1), UINT32_MAX);
        EXPECT_NE(book.getGroupIDForResource(0), UINT32_MAX);
    }
}    // namespace Razix
