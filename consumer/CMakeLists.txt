cmake_minimum_required (VERSION 3.2)

project(consumer CXX C)

include_directories(
     include 
     src
     ${COMMON_PROJECT_DIR}/include 
     "${POCO_INCLUDE_DIRS}")

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${POCO_LIBRARY_DIRS}
    ${GTEST_LIBRARY_DIRS}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

file(GLOB_RECURSE COMMON_SRC
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     ${COMMON_PROJECT_DIR}/src/*.cpp
     ${COMMON_PROJECT_DIR}/include/*.hpp)

file(GLOB_RECURSE CONSUMER_SRC
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

source_group(Sources FILES ${CONSUMER_SRC})
source_group(Common FILES ${COMMON_SRC})

add_executable(${PROJECT_NAME} ${COMMON_SRC} ${CONSUMER_SRC})
add_dependencies(${PROJECT_NAME} poco)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} ${POCO_LIBRARIES})
endif(UNIX)

target_link_libraries(
    ${PROJECT_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# ENABLE WITH:  -DBUILD_TESTS=ON
option (BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    include(consumer-tests.cmake)
endif()
