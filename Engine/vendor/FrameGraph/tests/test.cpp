#include "fg/FrameGraph.hpp"
#include "fg/Blackboard.hpp"
#include "catch.hpp"
#include <fstream>

struct BadResource {
  struct Desc {};

  void create() {}
  void destroy() {}
};
static_assert(
#if _HAS_CXX20
  not Virtualizable<BadResource>
#else
  !is_resource<BadResource>()
#endif
);

struct FrameGraphTexture {
  struct Desc {
    uint32_t width;
    uint32_t height;
  };

  FrameGraphTexture() = default;
  explicit FrameGraphTexture(int32_t _id) : id{_id} {}
  FrameGraphTexture(FrameGraphTexture &&) noexcept = default;

  void create(const Desc &, void *) {
    static auto lastId = 0;
    id = ++lastId;
  }
  void destroy(const Desc &, void *) {}

  void preRead(const Desc &, uint32_t flags, void *) {}
  void preWrite() {}

  static const char *toString(const Desc &desc) { return "<I>texture</I>"; }

  int32_t id{-1};
};

#if _HAS_CXX20
static_assert(Virtualizable<FrameGraphTexture>);
static_assert(has_preRead<FrameGraphTexture>);
static_assert(!has_preWrite<FrameGraphTexture>);
#else
static_assert(is_resource<FrameGraphTexture>());
static_assert(has_preRead<FrameGraphTexture>::value);
static_assert(!has_preWrite<FrameGraphTexture>::value);
#endif

//
// Runtime tests:
//

TEST_CASE("Pass without data", "[FrameGraph]") {
  FrameGraph fg;
  fg.addCallbackPass(
    "Dummy", [&](FrameGraph::Builder &builder, auto &) {},
    [=](const auto &, FrameGraphPassResources &resources, void *) {});
}
TEST_CASE("Basic graph with side-effect", "[FrameGraph]") {
  FrameGraph fg;

  struct TestPass {
    FrameGraphResource foo;
    FrameGraphResource bar;
    mutable bool executed{false};
  };
  auto &testPass = fg.addCallbackPass<TestPass>(
    "Test pass",
    [&](FrameGraph::Builder &builder, TestPass &data) {
      data.foo = builder.create<FrameGraphTexture>("foo", {128, 128});
      data.foo = builder.write(data.foo);
      REQUIRE(fg.isValid(data.foo));

      data.bar = builder.create<FrameGraphTexture>("bar", {256, 256});
      data.bar = builder.write(data.bar);
      REQUIRE(fg.isValid(data.bar));

      builder.setSideEffect();
    },
    [=](const TestPass &data, FrameGraphPassResources &resources, void *) {
      CHECK(resources.get<FrameGraphTexture>(data.foo).id == 1);
      CHECK(resources.get<FrameGraphTexture>(data.bar).id == 2);

      data.executed = true;
    });

  fg.compile();
  std::ofstream{"basic_graph.dot"} << fg;

  fg.execute();
  REQUIRE(testPass.executed);
}
TEST_CASE("Imported resource", "[FrameGraph]") {
  constexpr auto kBackbufferId = 777;

  FrameGraph fg;

  const auto backbuffer =
    fg.import("Backbuffer", {1280, 720}, FrameGraphTexture{kBackbufferId});
  REQUIRE(fg.isValid(backbuffer));

  struct TestPass {
    FrameGraphResource backbuffer;
    mutable bool executed{false};
  };
  auto &testPass = fg.addCallbackPass<TestPass>(
    "Test pass",
    [&](FrameGraph::Builder &builder, TestPass &data) {
      const auto temp = backbuffer;
      data.backbuffer = builder.write(backbuffer);
      REQUIRE(fg.isValid(data.backbuffer));
      REQUIRE_FALSE(fg.isValid(temp));
    },
    [=](const TestPass &data, FrameGraphPassResources &resources, void *) {
      CHECK(resources.get<FrameGraphTexture>(data.backbuffer).id ==
            kBackbufferId);
      data.executed = true;
    });

  fg.compile();
  std::ofstream{"imported_resource.dot"} << fg;

  fg.execute();
  REQUIRE(testPass.executed);
}
TEST_CASE("Renamed resource", "[FrameGraph]") {
  FrameGraph fg;

  struct PassData {
    FrameGraphResource foo;
    mutable bool executed{false};
  };
  auto &pass1 = fg.addCallbackPass<PassData>(
    "Pass1",
    [&](FrameGraph::Builder &builder, PassData &data) {
      data.foo = builder.create<FrameGraphTexture>("foo", {});
      data.foo = builder.write(data.foo);
    },
    [=](const PassData &data, FrameGraphPassResources &, void *) {
      data.executed = true;
    });

  auto &pass2 = fg.addCallbackPass<PassData>(
    "Pass2",
    [&](FrameGraph::Builder &builder, PassData &data) {
      data.foo = builder.write(builder.read(pass1.foo));
      REQUIRE_FALSE(fg.isValid(pass1.foo));
      REQUIRE(fg.isValid(data.foo));

      builder.setSideEffect();
    },
    [=](const PassData &data, FrameGraphPassResources &, void *) {
      data.executed = true;
    });

  fg.compile();
  std::ofstream{"renamed_resource.dot"} << fg;

  fg.execute();
  REQUIRE(pass1.executed);
  REQUIRE(pass2.executed);
}
TEST_CASE("Culled pass", "[FrameGraph]") {
  FrameGraph fg;

  struct TestPass {
    mutable bool executed{false};
  };
  auto &testPass = fg.addCallbackPass<TestPass>(
    "Test pass", [&](FrameGraph::Builder &, TestPass &) {},
    [=](const TestPass &data, FrameGraphPassResources &, void *) {
      data.executed = true;
    });

  fg.compile();
  std::ofstream{"culled_pass.dot"} << fg;

  fg.execute();
  REQUIRE_FALSE(testPass.executed);
}
TEST_CASE("Deferred pipeline", "[FrameGraph]") {
  FrameGraph fg;
  auto backbufferId =
    fg.import("Backbuffer", {1280, 720}, FrameGraphTexture{117});

  const auto &desc = fg.getDescriptor<FrameGraphTexture>(backbufferId);

  struct DepthPass {
    FrameGraphResource depth;
    mutable bool executed{false};
  };
  auto &depthPass = fg.addCallbackPass<DepthPass>(
    "Depth pass",
    [&](FrameGraph::Builder &builder, DepthPass &data) {
      data.depth = builder.create<FrameGraphTexture>("DepthBuffer", desc);
      data.depth = builder.write(data.depth);
    },
    [=](const DepthPass &data, FrameGraphPassResources &, void *) {
      data.executed = true;
    });

  struct GBufferPass {
    FrameGraphResource depth;
    FrameGraphResource position;
    FrameGraphResource normal;
    FrameGraphResource albedo;

    mutable bool executed{false};
  };
  auto &gbufferPass = fg.addCallbackPass<GBufferPass>(
    "GBuffer pass",
    [&](FrameGraph::Builder &builder, GBufferPass &data) {
      data.depth = builder.read(depthPass.depth);
      data.position =
        builder.create<FrameGraphTexture>("GBuffer/ Position", desc);
      data.position = builder.write(data.position);
      data.normal = builder.create<FrameGraphTexture>("GBuffer/ Normal", desc);
      data.normal = builder.write(data.normal);
      data.albedo = builder.create<FrameGraphTexture>("GBuffer/ Albedo", desc);
      data.albedo = builder.write(data.albedo);
    },
    [](const GBufferPass &data, FrameGraphPassResources &, void *) {
      data.executed = true;
    });

  struct LightingPass {
    FrameGraphResource position;
    FrameGraphResource normal;
    FrameGraphResource albedo;
    FrameGraphResource output;
    mutable bool executed{false};
  };
  auto &lightingPass = fg.addCallbackPass<LightingPass>(
    "Lighting pass",
    [&](FrameGraph::Builder &builder, LightingPass &data) {
      data.position = builder.read(gbufferPass.position);
      data.normal = builder.read(gbufferPass.normal);
      data.albedo = builder.read(gbufferPass.albedo);
      data.output = builder.write(backbufferId);
    },
    [=](const LightingPass &data, FrameGraphPassResources &, void *) {
      data.executed = true;
    });

  struct Dummy {
    mutable bool executed{false};
  };
  auto &dummyPass = fg.addCallbackPass<Dummy>(
    "Dummy pass", [&](FrameGraph::Builder &, Dummy &) {},
    [=](const Dummy &data, FrameGraphPassResources &, void *) {
      data.executed = true;
    });

  fg.compile();
  std::ofstream{"deferred_pipeline.dot"} << fg;

  fg.execute();
  REQUIRE(depthPass.executed);
  REQUIRE(gbufferPass.executed);
  REQUIRE(lightingPass.executed);
  REQUIRE_FALSE(dummyPass.executed);
}

TEST_CASE("Blackboard", "[Blackboard]") {
  FrameGraphBlackboard bb;

  struct FooData {
    int32_t x, y, z;
  };
  bb.add<FooData>() = {1, 2, 3};
  REQUIRE(bb.has<FooData>());
  REQUIRE(bb.try_get<FooData>());

  auto *foo = bb.try_get<FooData>();

  struct BarData {
    int32_t i, j, k;
  };
  bb.add<BarData>(9, 8, 7);
  REQUIRE(bb.has<BarData>());

  CHECK(bb.get<BarData>().i == 9);
  CHECK(bb.get<BarData>().j == 8);
  CHECK(bb.get<BarData>().k == 7);

  CHECK(foo->x == 1);
  CHECK(foo->y == 2);
  CHECK(foo->z == 3);

  foo->x = 100;
  CHECK(bb.get<FooData>().x == 100);
}

int main(int argc, char *argv[]) { return Catch::Session().run(argc, argv); }
