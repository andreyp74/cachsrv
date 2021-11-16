cmake_minimum_required (VERSION 3.2)

### source files
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${POCO_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${GMOCK_INCLUDE_DIRS}
    ${COMMON_PROJECT_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

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

file(GLOB_RECURSE CACHESRV_SRC
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

source_group(Sources FILES ${CACHESRV_SRC})
source_group(Common FILES ${COMMON_SRC})

# add_library(${PROJECT_NAME} SHARED ${COMMON_SRC} ${CACHESRV_SRC})
add_executable(${PROJECT_NAME} ${COMMON_SRC} ${CACHESRV_SRC})
add_dependencies(${PROJECT_NAME} poco)

target_sources(${PROJECT_NAME} PRIVATE ${COMMON_SRC} ${CACHESRV_SRC})

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
