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

### source files
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${POCO_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${GMOCK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${POCO_LIBRARY_DIRS}
    ${GTEST_LIBRARY_DIRS}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

file(GLOB_RECURSE CACHESRV_SRC
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

source_group(Sources FILES ${CACHESRV_SRC})

# add_library(${PROJECT_NAME} SHARED ${CACHESRV_SRC})
add_executable(${PROJECT_NAME} ${CACHESRV_SRC})
add_dependencies(${PROJECT_NAME} poco)

target_sources(${PROJECT_NAME} PRIVATE ${CACHESRV_SRC})

if(UNIX)
    target_link_libraries(${PROJECT_NAME} 
        ${POCO_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )
endif(UNIX)

#set_target_properties(${PROJECT_NAME}
#    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# ENABLE WITH:  -DBUILD_TESTS=ON
option (BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    include(cachesrv_tests.cmake)
endif()
