#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EAAssertTest CXX)
include(CTest)

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_CHAR16T)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/packages/EASTL/scripts/CMake")
include(CommonCppFlags)

#-------------------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------------------
file(GLOB EAASSERTTEST_SOURCES "TestEAAssert.cpp")
set(SOURCES ${EAASSERTTEST_SOURCES})

#-------------------------------------------------------------------------------------------
# Executable definition
#-------------------------------------------------------------------------------------------
add_executable(EAAssertTest ${SOURCES})

#-------------------------------------------------------------------------------------------
# Dependencies 
#-------------------------------------------------------------------------------------------
add_subdirectory(packages/EABase)
add_subdirectory(packages/EAMain)
add_subdirectory(packages/EASTL)
add_subdirectory(packages/EAStdC)
add_subdirectory(packages/EATest)
add_subdirectory(packages/EAThread)

target_link_libraries(EAAssertTest EABase)
target_link_libraries(EAAssertTest EAMain)
target_link_libraries(EAAssertTest EASTL)
target_link_libraries(EAAssertTest EAStdC)
target_link_libraries(EAAssertTest EATest)
target_link_libraries(EAAssertTest EAThread)

#-------------------------------------------------------------------------------------------
# Run Unit tests and verify the results.
#-------------------------------------------------------------------------------------------
add_test(EAAssertTestRuns EAAssertTest)
set_tests_properties (EAAssertTestRuns PROPERTIES PASS_REGULAR_EXPRESSION "RETURNCODE=0")

