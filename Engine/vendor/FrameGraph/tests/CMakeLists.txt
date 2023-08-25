find_package(Catch2 CONFIG REQUIRED)

add_executable(tests "test.cpp")
target_link_libraries(tests PRIVATE fg::FrameGraph Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(tests)
