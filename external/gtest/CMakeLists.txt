cmake_minimum_required(VERSION 3.2)
project(gtest_builder C CXX)

include(ExternalProject)

ExternalProject_Add(googletest
    URL https://github.com/google/googletest/archive/release-1.8.1.zip
    #URL_MD5 ef5e700c8a0f3ee123e2e0209b8b4961
    CMAKE_ARGS -DBUILD_GMOCK=ON
               -Dgtest_force_shared_crt=ON
               -Dgtest_build_tests=OFF
               -DBUILD_SHARED_LIBS=ON
               -DLIBRARY_OUTPUT_PATH:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
               -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
               -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}

    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    INSTALL_COMMAND ""
    DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/.downloaded"
)

# Specify include dir
ExternalProject_Get_Property(googletest SOURCE_DIR)
set(GTEST_INCLUDE_DIRS ${SOURCE_DIR}/googletest/include PARENT_SCOPE)
set(GMOCK_INCLUDE_DIRS ${SOURCE_DIR}/googlemock/include PARENT_SCOPE)

ExternalProject_Get_Property(googletest BINARY_DIR)
set(GTEST_LIBRARY_DIRS ${BINARY_DIR} PARENT_SCOPE)

set(GTEST_LIBRARIES
  ${CMAKE_SHARED_LIBRARY_PREFIX}gtestd${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${CMAKE_SHARED_LIBRARY_PREFIX}gtest_maind${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${CMAKE_SHARED_LIBRARY_PREFIX}gmockd${CMAKE_SHARED_LIBRARY_SUFFIX}
  ${CMAKE_SHARED_LIBRARY_PREFIX}gmock_maind${CMAKE_SHARED_LIBRARY_SUFFIX}
  PARENT_SCOPE
)
