cmake_minimum_required (VERSION 3.2)

project(cachesrv CXX C)

if( NOT DEFINED CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE release CACHE STRING
       "Choose the type of build, options are: None Debug release RelWithDebInfo MinsizeRel."
       FORCE )
  message(STATUS "Setting build type - ${CMAKE_BUILD_TYPE}")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if(UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
endif(UNIX)

### external dependencies

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
if(BUILD_TESTS)
    find_package(Boost 1.71 REQUIRED COMPONENTS unit_test_framework)
    add_definitions(-DBOOST_TEST_DYN_LINK) 
else()
    find_package(Boost 1.71 REQUIRED)
endif()

#### compiler-specific options
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    add_definitions(-D_WIN32_WINNT=0x0A00) # Target is windows 10
    add_definitions(-DPOCO_DLL)
    add_compile_options("/std:c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /MP")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /MDd")
    set(CMAKE_CXX_FLAGS_release "${CMAKE_CXX_FLAGS_release} /Zi /MD")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG /OPT:REF /OPT:ICF")
endif()

if(UNIX)
    find_package(Threads)
    add_definitions(-DNDEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O2 -std=c99 -Wall -D_DEFAULT_SOURCE")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -std=c++17 -lrt -lpthread")
endif()

#### setup external projects
set(EXT_PROJECTS_DIR "${PROJECT_SOURCE_DIR}/external")

add_subdirectory("${EXT_PROJECTS_DIR}/poco")
add_subdirectory("${EXT_PROJECTS_DIR}/gtest")

# projects

set(COMMON_PROJECT_DIR "${PROJECT_SOURCE_DIR}/common")

add_subdirectory("${PROJECT_SOURCE_DIR}/core")
add_subdirectory("${PROJECT_SOURCE_DIR}/producer")
add_subdirectory("${PROJECT_SOURCE_DIR}/consumer")
