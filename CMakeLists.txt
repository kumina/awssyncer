cmake_minimum_required(VERSION 2.8)
project(awssyncer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(contrib/googletest-release-1.8.0)
include_directories(contrib/googletest-release-1.8.0/googletest/include)

add_library(libawssyncer STATIC
            filesystem_dirt.cc)

add_executable(awssyncer awssyncer.cc)

add_executable(awssyncer_test
               filesystem_dirt_test.cc)
target_link_libraries(awssyncer_test gtest gtest_main libawssyncer)
