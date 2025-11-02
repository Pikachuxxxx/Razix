// STD_MovesemanticsTests.cpp
// AI-generated unit tests for the RZUUID classes
#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/std/utility.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace Razix {

    //  Tests for rz_remove_reference
    // ============================================================================
    TEST(RZTypeTraits, RemoveReferenceBasic)
    {
        static_assert(std::is_same_v<rz_remove_reference<int>::type, int>);
        static_assert(std::is_same_v<rz_remove_reference<int&>::type, int>);
        static_assert(std::is_same_v<rz_remove_reference<int&&>::type, int>);
    }

    TEST(RZTypeTraits, RemoveReferenceConst)
    {
        static_assert(std::is_same_v<rz_remove_reference<const int&>::type, const int>);
        static_assert(std::is_same_v<rz_remove_reference<const int&&>::type, const int>);
    }

    // ============================================================================
    //  Compile-time correctness of rz_move
    // ============================================================================
    TEST(RZMove, TypeDeduction)
    {
        static_assert(std::is_same_v<decltype(rz_move(std::declval<int&>())), int&&>);
        static_assert(std::is_same_v<decltype(rz_move(std::declval<int&&>())), int&&>);
        static_assert(std::is_same_v<decltype(rz_move(std::declval<const int&>())), const int&&>);
    }

    // ============================================================================
    //  Runtime move vs copy tests
    // ============================================================================
    struct MoveTracker
    {
        static inline int copies = 0;
        static inline int moves  = 0;

        MoveTracker() = default;
        MoveTracker(const MoveTracker&) { ++copies; }
        MoveTracker(MoveTracker&&) noexcept { ++moves; }
        MoveTracker& operator=(const MoveTracker&)
        {
            ++copies;
            return *this;
        }
        MoveTracker& operator=(MoveTracker&&) noexcept
        {
            ++moves;
            return *this;
        }
    };

    TEST(RZMove, InvokesMoveConstructor)
    {
        MoveTracker::copies = MoveTracker::moves = 0;
        MoveTracker a;
        MoveTracker b = rz_move(a);
        EXPECT_EQ(MoveTracker::copies, 0);
        EXPECT_EQ(MoveTracker::moves, 1);
    }

    TEST(RZMove, InvokesMoveAssignment)
    {
        MoveTracker::copies = MoveTracker::moves = 0;
        MoveTracker a, b;
        b = rz_move(a);
        EXPECT_EQ(MoveTracker::copies, 0);
        EXPECT_EQ(MoveTracker::moves, 1);
    }

    // ============================================================================
    //  noexcept and constexpr properties
    // ============================================================================
    TEST(RZMove, IsNoexcept)
    {
        EXPECT_TRUE(noexcept(rz_move(std::declval<int&&>())));
        struct Dummy
        {};
        EXPECT_TRUE(noexcept(rz_move(std::declval<Dummy&&>())));
    }

    struct S
    {
        int x;
    };

    constexpr S make()
    {
        S s{5};
        return rz_move(s);
    }

    TEST(RZMove, ConstexprUsable)
    {
        constexpr S s = make();
        static_assert(s.x == 5);
        EXPECT_EQ(s.x, 5);
    }

    // ============================================================================
    //  Lvalue, Rvalue, and Const type behavior
    // ============================================================================
    TEST(RZMove, LvalueBecomesRvalue)
    {
        int    x = 42;
        auto&& r = rz_move(x);
        static_assert(std::is_same_v<decltype(r), int&&>);
        EXPECT_EQ(x, 42);
    }

    TEST(RZMove, RvalueRemainsRvalue)
    {
        // Rvalue input -> remains rvalue
        static_assert(std::is_same_v<decltype(rz_move(123)), int&&>);
    }

    TEST(RZMove, ConstTypePreservesConstness)
    {
        const int cx = 10;
        auto&&    r  = rz_move(cx);
        static_assert(std::is_same_v<decltype(r), const int&&>);
        EXPECT_EQ(r, 10);
    }

    // ============================================================================
    //  Reference collapsing and forwarding rules
    // ============================================================================
    TEST(RZMove, ReferenceCollapsingRules)
    {
        using LRef = int&;
        using RRef = int&&;

        static_assert(std::is_same_v<decltype(rz_move(std::declval<LRef>())), int&&>);
        static_assert(std::is_same_v<decltype(rz_move(std::declval<RRef>())), int&&>);
    }

    // ============================================================================
    //  Validity after move (moved-from object remains valid)
    // ============================================================================
    struct MoveAware
    {
        bool moved  = false;
        MoveAware() = default;
        MoveAware(const MoveAware&) {}
        MoveAware(MoveAware&& other) noexcept { other.moved = true; }
    };

    TEST(RZMove, LeavesMovedFromValid)
    {
        MoveAware a;
        MoveAware b = rz_move(a);
        EXPECT_TRUE(a.moved);
    }

// ============================================================================
//  Interoperability with containers (optional but practical)
// ============================================================================
#include <vector>

    TEST(RZMove, WorksWithContainers)
    {
        std::vector<int> a = {1, 2, 3};
        std::vector<int> b = rz_move(a);
        EXPECT_TRUE(a.empty() || a.size() == 0);
        EXPECT_EQ(b.size(), 3);
    }

    // ============================================================================
    //  Compile-time static checks (meta tests)
    // ============================================================================
    static_assert(std::is_same_v<decltype(rz_move(std::declval<const int&>())), const int&&>);
    static_assert(std::is_same_v<decltype(rz_move(std::declval<int&>())), int&&>);
    static_assert(noexcept(rz_move(std::declval<int&&>())));

}    // namespace Razix
