cmake_minimum_required(VERSION 2.8)
project(awssyncer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(contrib/googletest-release-1.8.0)
include_directories(
    contrib/googletest-release-1.8.0/googlemock/include
    contrib/googletest-release-1.8.0/googletest/include
)

add_library(libawssyncer STATIC
            src/aws_command_runner.cc
            src/filesystem_dirt.cc
            src/filtering_inotify_poller.cc
            src/multiple_command_runner.cc
            src/nonrecursive_inotify_poller.cc
            src/posix_command_runner.cc
            src/recursive_inotify_poller.cc)

add_executable(awssyncer
               src/awssyncer.cc)
target_link_libraries(awssyncer
                      libawssyncer)

add_executable(awssyncer_test
               src/aws_command_runner_test.cc
               src/filesystem_dirt_test.cc
               src/filtering_inotify_poller_test.cc)
target_link_libraries(awssyncer_test
                      gmock gtest gtest_main libawssyncer)
